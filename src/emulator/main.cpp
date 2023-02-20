#include "pch.hpp"

#include "elf/elf.hpp"
#include "mips/mips.hpp"
#include "system_vemix.hpp"

#include <chrono>
#include <functional>
#include <string>
#include <vector>

#include <cassert>

#include "platform/platform.hpp"
#include <PowrProf.h>

#define PAIR_TEST 0

static uint64 GetHostFrequency() {
	struct PROCESSOR_POWER_INFORMATION final {
	  ULONG Number;
	  ULONG MaxMhz;
	  ULONG CurrentMhz;
	  ULONG MhzLimit;
	  ULONG MaxIdleState;
	  ULONG CurrentIdleState;
	};

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	std::vector<PROCESSOR_POWER_INFORMATION> PowerInfo;
	PowerInfo.resize(sysInfo.dwNumberOfProcessors);

	size_t OutBufferLength = PowerInfo.size() * sizeof(PROCESSOR_POWER_INFORMATION);
	xassert(OutBufferLength <= std::numeric_limits<ULONG>::max());
	NTSTATUS status = CallNtPowerInformation(
		ProcessorInformation,
		nullptr,
		0,
		PowerInfo.data(),
		ULONG(OutBufferLength)
	);

	if (status != 0) {
		return 1;
	}

	// Also set the max performance state temporarily.

	return PowerInfo[0].MaxMhz;
}

#if EMSCRIPTEN
#	include "base64.hpp"
#endif

static constexpr const uint32_t VerMajor = 1;
static constexpr const uint32_t VerMinor = 0;
static constexpr const uint32_t VerBuild = 0;

static void version();
static void help();
static void changes();

#if !EMSCRIPTEN
struct Option final {
	std::vector<std::string> option_str;
	std::string description;
	std::function<void(int argc, const char **argv, int &i)> procedure;
};
#endif

uint32 AvailableMemory = 1048576 * 2;
uint32 StackMemory = uint32(-1);
mips::JitType JitToUse = mips::JitType::Jit;
bool use_rox = false;
mips::mmu mmu_type = mips::mmu::none;
bool instruction_Stats = false;
bool disable_cti = false;
bool instruction_cache = true;
uint64 ticks = 0;
bool debug = false;
uint16 debug_port = 0;

#if !EMSCRIPTEN
static const std::vector<Option> Options {
	{
		{"-m", "--memory"},
		"Specify how much memory to which the CPU shall have access [default: 1048576]",
		[](int argc, const char **argv, int &i) {
			if (i == argc - 1) {
				fprintf(stderr, "Error: No quantity following -m option\n");
				_exit(1);
			}
			++i;
			auto memory = std::stoll(argv[i], 0, 0);
			if (memory < 4096) {
				fprintf(stderr, "You cannot specify < 4096 bytes of memory to the emulator\n");
				_exit(1);
			}
			if (memory > uint32(-1)) {
				fprintf(stderr, "You cannot specify greater than or equal to 2^32 bytes of memory to the emulator\n");
				_exit(1);
			}
			AvailableMemory = uint32(memory);  
		}
	},
	{
		{"-s", "--stack"},
		"Specify how much memory will be reserved for the stack [default: memory / 2]",
		[](int argc, const char **argv, int &i) {
			if (i == argc - 1) {
				fprintf(stderr, "Error: No quantity following -s option\n");
				_exit(1);
			}
			++i;
			auto memory = std::stoll(argv[i], 0, 0);
			if (memory < 0) {
				fprintf(stderr, "You cannot specify < 0 bytes of stack to the emulator\n");
				_exit(1);
			}
			if (memory > uint32(-1)) {
				fprintf(stderr, "You cannot specify greater than or equal to 2^32 bytes of stack memory\n");
				_exit(1);
			}
			StackMemory = uint32(memory);  
		}
	},
	{
		{"--debug"},
		"Enable the debugger. GDB Port must follow.",
		[](int argc, const char **argv, int &i) {
			if (i == argc - 1) {
				fprintf(stderr, "Error: No port following ---debug option\n");
				_exit(1);
			}
			++i;
			auto port = std::stoll(argv[i], 0, 0);
			if (port < 0 || port > 0xFFFF) {
				fprintf(stderr, "Provided debuggerport number is out of range.\n");
				_exit(1);
			}
			debug_port = uint16(port);
			debug = true;
		}
	},
	{
		{"-h", "--help"},
		"Displays tool help information [you are viewing this]",
		[](int, const char **, int &) {
			help();
			_exit(0);
		}
	},
	{
		{"-v", "--version"},
		"Displays tool version information",
		[](int, const char **, int &) {
			version();
			_exit(0);
		}
	},
	{
		{"--changes"},
		"Displays most recent changes to build",
		[](int, const char **, int &) {
			changes();
			_exit(0);
		}
	},
	{
		{"--rox"},
		"Makes executable memory read-only. [cannot be combined with --nommu]",
		[](int, const char **, int &) {
			use_rox = true;
		}
	},
	 {
		 { "--no-cti" },
		 "Disable CTI flag checking",
		 [](int, const char**, int&) {
				disable_cti = true;
			}
	 },
	{
		{"--mmu"},
		"Specifies which MMU to use [emulated, none, host] [default: emulated].",
		[](int argc, const char **argv, int &i) {
			if (i == argc - 1) {
				printf("Error: No MMU following --mmu option\n");
				_exit(1);
			}
			++i;
			const auto mmu_v = argv[i];
			if (strcmp(mmu_v, "emulated") == 0) {
				mmu_type = mips::mmu::emulated;
			}
			else if (strcmp(mmu_v, "none") == 0) {
				mmu_type = mips::mmu::none;
			}
			else if (strcmp(mmu_v, "host") == 0) {
				mmu_type = mips::mmu::host;
			}
			else {
				fprintf(stderr, "The provided MMU (\'%s\') is not a valid MMU type\n", mmu_v);
				_exit(1);
			}
		}
	},
  {
		{"--icache"},
		"Enables instruction cache.",
		[](int, const char **, int &) {
			instruction_cache = true;
		}
	},
	{
		{"--jit0"},
		"Disable JIT usage in emulator",
		[](int, const char **, int &) {
			JitToUse = mips::JitType::None;
		}
	},
	{
		{"--jit1"},
		"Enable JIT in emulator (ticked mode not yet implemented) [default]",
		[](int, const char **, int &) {
			JitToUse = mips::JitType::Jit;
		}
	},
	{
		{"--jit2"},
		"Enable function table in emulator (ticked mode not yet implemented)",
		[](int, const char**, int&) {
			JitToUse = mips::JitType::FunctionTable;
		}
	},
	{
		{"--stats"},
		"Enable capturing of instruction stats",
		[](int, const char **, int &) {
			instruction_Stats = true;
		}
	},
	{
		{"-t", "--ticks"},
		"Enables ticked execution of emulator and increments by provided ticks",
		[](int argc, const char **argv, int &i) {
			if (i == argc - 1) {
				fprintf(stderr, "Error: No quantity following --ticks option\n");
				_exit(1);
			}
			++i;
			ticks = std::stoull(argv[i], 0, 0);
			if (ticks == 0) {
				fprintf(stderr, "You cannot specify 0 ticks\n");
				_exit(1);
			}
		}
	},
};
#endif

static void changes() {
	puts(
		"Changelist:\n"
		"\t1.0.0\n"
		"Code cleanup, mostly migrated to VS2019.\n"
		"\t0.9.27\n"
		"Improved performance.\n"
		"Added further support for Linux ABIs for runtime libraries\n"
		"Added JIT jump patching. CURRENTLY DOES NOT SUPPORT SELF-MODIFYING CODE.\n"
		"\t0.9.25\n"
		"Added loads and stores to the JIT.\n"
		"Added full delay branch support to the JIT.\n"
		"Added some more basic instructions to the JIT.\n"
		"Improved load instructions and a few others.\n"
		"Added a few more basic COP1 instructions to JIT.\n"
		"Fixed a major bug in ticked mode in JIT.\n"
		"Improved stack usage in JIT.\n"
		"\t0.9.23\n"
		"Fixed errors in instruction counting in JIT.\n"
		"Added ticked execution mode.\n"
		"\t0.9.21\n"
		"All compact branches integrated into JIT.\n"
		"Added a springboard to make more registers available to the JIT.\n"
		"Compacted exceptions into intrinsic jumps.\n"
		"Added optimized delay branch suffixes.\n"
		"Fixed NAL - added NoCTI flag.\n"
		"Added a few more JIT operations.\n"
		"\t0.9.19\n"
		"Replaced hex-based jit with xbyak. A little slower, but far easier to use.\n"
		"\t0.9.17\n"
		"Added the ability for subroutine compact jumps to handle the entire jump within the JIT without a flush.\n"
		"Added the ability for subroutine branch delay jumps to handle the entire jump within the JIT without a flush.\n"
		"Added end-of-cluster implicit jumps. They currently do not patch, though.\n"
		"Made it so the instruction counts reported for interpreted mode and JIT match.\n"
		"\t0.9.15\n"
		"Moved control over CTI detection to the JIT.\n"
		"Fixed some JIT jump length errors caused by 0.9.13's reduction of direct offset size.\n"
		"\t0.9.14\n"
		"Fixed a bug where certain branches couldn't get hit due to an incorrect bitmask on one of the POPsets.\n"
		"Split off several POPsets into their own instructions to improve performance.\n"
		"\t0.9.13\n"
		"Fixed potential JIT crash.\n"
		"Adjusted most direct offsets in JIT to be 1-byte instead of 4-byte.\n"
		"Fixed a MAJOR bug which was generating incorrect bitmasks for FPU operations, causing some FPU ops to fail.\n"
		"\t0.9.12\n"
		"Fixed potential issue if processor offsets were > 256 B.\n"
		"Very slightly improved the efficiency of some JIT addiu translations.\n"
		"\t0.9.11\n"
		"Fixed potential JIT crash when altering memory that is currently executing.\n"
		"Further cleanups to the options system.\n"
		"\t0.9.9\n"
		"Removed old, inefficient JIT attempts in favor of functional JIT.\nMade the JIT work correctly with multiple chunk sizes (internal).\n"
		"Made the JIT honor self-modifying code correctly (needs a lot of optimization).\nAdded --rox flag to disable support for self-modifying code."
	);
}

static void version() {
	printf("Digital Carbide DCMIPSr6E Emulator %u.%u.%u (%s %s)\n", VerMajor, VerMinor, VerBuild, __DATE__, __TIME__);
}

#if !EMSCRIPTEN
static void help() {
	version();
	printf("OVERVIEW: Emulator for MIPSr6 binaries\n\n");
	printf("USAGE: emulator.exe [options] <binary>\n\n");
	printf("OPTIONS:\n");
	for (const auto &option : Options) {
		bool first = true;
		for (const std::string &opt : option.option_str) {
			if (first) {
				printf("  %s", opt.c_str());
			}
			else {
				printf(" %s", opt.c_str());
			}
			first = false;
		}
		printf("\n		%s\n", option.description.c_str());
	}
}
#endif

int main(int argc, const char **argv) {
	{
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
		ULONG_PTR processAffinity, systemAffinity;
		GetProcessAffinityMask(GetCurrentProcess(), &processAffinity, &systemAffinity);
		if (processAffinity != 1) {
			processAffinity &= ~1ull;
			SetProcessAffinityMask(GetCurrentProcess(), processAffinity);
			Yield();
		}
	}

	std::string binary_file;

#if !EMSCRIPTEN
	for (int i = 1; i < argc; ++i) {
		bool optionFound = false;
		for (const auto &option : Options) {
			bool matches_any = false;
			for (const auto &str : option.option_str) {
				if (str == argv[i]) {
					matches_any = true;
					break;
				}
			}
			if (matches_any) {
				option.procedure(argc, argv, i);
				optionFound = true;
			}
		}

		if (!optionFound) {
			if (argv[i][0] == '-') {
				fprintf(stderr, "Unknown option: %s\n", argv[i]);
				return 1;
			}
			if (binary_file.length()) {
				fprintf(stderr, "Cannot open multiple binary files\n");
				return 1;
			}

			binary_file = argv[i];
		}
	}
#else
	binary_file = "tmp/uploaded.bin";
#endif

	if (StackMemory == uint32(-1)) {
		StackMemory = AvailableMemory / 2;
	}

	if ((AvailableMemory - StackMemory) < 4096) {
		fprintf(stderr, "There must be at least 4KiB of memory available to the system after removing stack reservation.\n");
		return 1;
	}

	if (use_rox && mmu_type != mips::mmu::emulated) {
		fprintf(stderr, "--rox requires --mmu emulated\n");
		return 1;
	}

	if (mmu_type != mips::mmu::emulated && !instruction_cache && JitToUse != mips::JitType::None) {
		fprintf(stderr, "--no-mmu requires --icache when being used with a JIT.\n");
		return 1;
	}

	if (mmu_type != mips::mmu::emulated && !instruction_cache && JitToUse == mips::JitType::None) {
		fprintf(stdout, "warning: --no-mmu requires --icache to achieve behavioural parity with JIT.\n");
	}

	version();

	// print options
	fprintf(stdout, "Options:\n");
	fprintf(stdout, "\t%u bytes memory\n", AvailableMemory);
	fprintf(stdout, "\t%u bytes reserved for stack\n", StackMemory);
	switch (JitToUse) {
		case mips::JitType::None:
			fprintf(stdout, "\tInterpreted Mode\n"); break;
		case mips::JitType::Jit:
			fprintf(stdout, "\tJIT1 Mode\n"); break;
		case mips::JitType::FunctionTable:
			fprintf(stdout, "\tFunction Table Mode\n"); break;
	}
	fprintf(stdout, "\tROX (Read-only Executable) mode %s\n", use_rox ? "enabled" : "disabled");
	switch (mmu_type) {
		case mips::mmu::emulated:
			fprintf(stdout, "\tMemory Management Unit: emulated\n"); break;
		case mips::mmu::none:
			fprintf(stdout, "\tMemory Management Unit: none\n"); break;
		case mips::mmu::host:
			fprintf(stdout, "\tMemory Management Unit: host\n"); break;
	}
	fprintf(stdout, "\tInstruction statistics %s\n", instruction_Stats ? "enabled" : "disabled");
	fprintf(stdout, "\tInstruction cache %s\n", instruction_cache ? "enabled" : "disabled");
	if (ticks == 0) {
		fprintf(stdout, "\tRealtime-mode enabled\n");
	}
	else {
		fprintf(stdout, "\tLockstep-mode enabled, %zu cycles\n", ticks);
	}

	std::vector<char> binaryFileData;

	{
#if !EMSCRIPTEN
		FILE *fp = fopen(binary_file.c_str(), "rb");
		if (!fp) {
			fprintf(stderr, "Could not open binary \"%s\"\n", binary_file.c_str());
			return 1;
		}
		fseek(fp, 0, SEEK_END);
		binaryFileData.resize(ftell(fp), 0);
		rewind(fp);
		fread(binaryFileData.data(), 1, binaryFileData.size(), fp);
		fclose(fp);
#else
		for (;;) {
			char byte = fgetc(stdin);
			if (byte == EOF) {
				break;
			}
			binaryFileData.push_back(byte);
		}
		binaryFileData = base64::decode(binaryFileData);
#endif
	}

	auto startTime = std::chrono::high_resolution_clock::now();
	uint64 instructions = 0;

	using statistic = std::pair<const char *, size_t>;

	std::vector<statistic> Stats;
	size_t largest_jit_instruction = 0;

	std::unique_ptr<mips::system> sys;

	try {
		printf("Processing ELF Binary\n");
		elf::binary elf_binary { binaryFileData };
		printf("Configuring VCPU and Configurating VENV\n");
		
		mips::system::options sys_options = {
			.total_memory = AvailableMemory,
			.stack_memory = StackMemory,
			.jit_type = JitToUse,
			.mmu_type = mmu_type,
			.debug_port = debug_port,
			.read_only_exec = use_rox,
			.record_instruction_stats = instruction_Stats,
			.disable_cti = disable_cti,
			.ticked = ticks != 0,
			.instruction_cache = instruction_cache,
			.debug = debug,
			.debug_owned = debug
		};
		try {
			sys_options.validate();
		}
		catch (const std::string ex) {
			fprintf(stderr, "Failed to validate system options:\n\t%s\n", ex.c_str());
			return 1;
		}

		sys = std::make_unique<mips::system_vemix>(sys_options, elf_binary);

		printf("Beginning Execution ---\n\n");
		fflush(stdout);
		startTime = std::chrono::high_resolution_clock::now();
		try {
			for (;;) {
				sys->clock(ticks);
				//printf("Instructions: %llu\n", processor.get_instruction_count());
			}
		}
		catch (...) {
			instructions = sys->get_instruction_count();
			if (instruction_Stats) {
				Stats.reserve(sys->get_stats_map().size());
				for (const auto &statistic : sys->get_stats_map()) {
					Stats.push_back(statistic);
				}
				largest_jit_instruction = sys->get_jit_max_instruction_size();
			}
			throw;
		}
	}
	catch (std::runtime_error exception) {
		fprintf(stderr, "\n** Error: %s\n", exception.what());
	}
	catch (mips::ExecutionCompleteException) {
		printf("\n** 'main' return detected - execution terminated\n");
	}
	catch (mips::ExecutionFailException) {
		printf("\n** Unhandled CPU exception - execution terminated\n");
	}
	catch (...) {
		printf("\n** Error\n");
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = endTime - startTime;
	uint64 ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

	double seconds = double(ns) / 1'000'000'000.0;
	double ips = double(instructions) / seconds;
	uint64 ipsr = size_t(round(ips));

	uint64 CpuFreq = GetHostFrequency();

	if (CpuFreq == 1) {
		printf("** Execution Duration: %llu ms (%llu ips)\n", ns / 1'000'000, ipsr);
	}
	else {
		double FreqRatio = double(CpuFreq * 1'000'000) / ips;

		printf("** Execution Duration: %llu ms (%llu ips - 1:%.2f guest/host)\n", ns / 1'000'000, ipsr, FreqRatio);
	}
	printf("** Instructions Executed: %llu\n", instructions);

	if (instruction_Stats) {
		printf("** Collected Instruction Stats:\n\n");

		if (largest_jit_instruction) {
			printf("\tLargest JIT Instruction: %zu bytes\n\n", largest_jit_instruction);
		}

		struct final {
			bool operator()(const statistic & __restrict a, const statistic & __restrict b) const __restrict {
				return a.second > b.second;
			}	
		} custom_comparator;

		std::sort(Stats.begin(), Stats.end(), custom_comparator);
		for (const statistic &stat_pair : Stats) {
			printf("\t%s - %zu\n", stat_pair.first, stat_pair.second);
		}
	}

	return 0;
}
