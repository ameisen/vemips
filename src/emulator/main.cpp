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

#include <cstdio>
#include <span>

#define PAIR_TEST 0

namespace {
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

		auto power_info = std::make_unique<PROCESSOR_POWER_INFORMATION[]>(sysInfo.dwNumberOfProcessors);

		const size_t OutBufferLength = sysInfo.dwNumberOfProcessors * sizeof(PROCESSOR_POWER_INFORMATION);
		xassert(OutBufferLength <= std::numeric_limits<ULONG>::max());

		if (const NTSTATUS status = CallNtPowerInformation(
			ProcessorInformation,
			nullptr,
			0,
			power_info.get(),
			ULONG(OutBufferLength)
		); status != 0) {
			return 1;
		}

		// TODO : Also set the max performance state temporarily.

		return power_info[0].MaxMhz;
	}

#if EMSCRIPTEN
#	include "base64.hpp"
#endif

	struct Version final {
		uint32 Major;
		uint32 Minor;
		uint32 Build;
	}
	static constexpr const Version = { 1, 0, 0 };

	static void version();
	static void help();
	static void changes();

	template <typename T>
	struct unique_span final {
		std::unique_ptr<T[]> data;
		size_t size = 0;

		_nothrow _forceinline unique_span() = default;

		_nothrow _forceinline unique_span(const size_t size) :
			data(std::make_unique<T[]>(size)),
			size(size) {
		}

		_nothrow _forceinline void resize (const size_t size) {
			if (this->size == size) {
				return;
			}

			data = std::make_unique<T[]>(size);
			this->size = size;
		}

		_nothrow _forceinline operator std::span<T>() const {
			return { data.get(), data.get() + size };
		}
	};

	struct argument_data final {
		unique_span<char> binary_data;
		uint64 ticks = 0;
		uint32 available_memory = 1048576 * 2;
		uint32 stack_memory = uint32(-1);
		mips::JitType jit = mips::JitType::Jit;
		mips::mmu mmu_type = mips::mmu::none;
		struct {
			uint16 port = 0;
			bool enabled = false;
		} debug;
		bool instruction_stats = false;
		bool disable_cti = false;
		bool instruction_cache = true;
		bool use_rox = false;
	};

#if !EMSCRIPTEN
	struct Option final {
		std::vector<std::string> option_str;
		std::string description;
		std::function<void(argument_data & __restrict argument_data, const std::span<const char*> args, uint32& __restrict i)> procedure;
	};
#endif

#if !EMSCRIPTEN
	static const std::vector<Option> Options{
		{
			{"-m", "--memory"},
			"Specify how much memory to which the CPU shall have access [default: 1048576]",
			[](argument_data& __restrict argument_data, const std::span<const char*> args, uint32& __restrict i) {
				if (i == args.size() - 1) {
					fprintf(stderr, "Error: No quantity following -m option\n");
					_exit(1);
				}
				++i;
				const auto memory = std::stoll(args[i], nullptr, 0);
				if (memory < 4096) {
					fprintf(stderr, "You cannot specify < 4096 bytes of memory to the emulator\n");
					_exit(1);
				}
				if (memory > uint32(-1)) {
					fprintf(stderr, "You cannot specify greater than or equal to 2^32 bytes of memory to the emulator\n");
					_exit(1);
				}
				argument_data.available_memory = uint32(memory);
			}
		},
		{
			{"-s", "--stack"},
			"Specify how much memory will be reserved for the stack [default: memory / 2]",
			[](argument_data& __restrict argument_data, const std::span<const char*> args, uint32& __restrict i) {
				if (i == args.size() - 1) {
					fprintf(stderr, "Error: No quantity following -s option\n");
					_exit(1);
				}
				++i;
				const auto memory = std::stoll(args[i], nullptr, 0);
				if (memory < 0) {
					fprintf(stderr, "You cannot specify < 0 bytes of stack to the emulator\n");
					_exit(1);
				}
				if (memory > uint32(-1)) {
					fprintf(stderr, "You cannot specify greater than or equal to 2^32 bytes of stack memory\n");
					_exit(1);
				}
				argument_data.stack_memory = uint32(memory);
			}
		},
		{
			{"--debug"},
			"Enable the debugger. GDB Port must follow.",
			[](argument_data& __restrict argument_data, const std::span<const char*> args, uint32& __restrict i) {
				if (i == args.size() - 1) {
					fprintf(stderr, "Error: No port following ---debug option\n");
					_exit(1);
				}
				++i;
				const auto port = std::stoll(args[i], nullptr, 0);
				if (!mips::within(port, std::numeric_limits<uint16>{})) {
					fprintf(stderr, "Provided debuggerport number is out of range.\n");
					_exit(1);
				}
				argument_data.debug = { uint16(port), true };
			}
		},
		{
			{"-h", "--help"},
			"Displays tool help information [you are viewing this]",
			[](argument_data& __restrict, const std::span<const char*>, uint32& __restrict) {
				help();
				_exit(0);
			}
		},
		{
			{"-v", "--version"},
			"Displays tool version information",
			[](argument_data& __restrict, const std::span<const char*>, uint32& __restrict) {
				version();
				_exit(0);
			}
		},
		{
			{"--changes"},
			"Displays most recent changes to build",
			[](argument_data& __restrict, const std::span<const char*>, uint32& __restrict) {
				changes();
				_exit(0);
			}
		},
		{
			{"--rox"},
			"Makes executable memory read-only. [cannot be combined with --nommu]",
			[](argument_data& __restrict argument_data, const std::span<const char*>, uint32& __restrict) {
				argument_data.use_rox = true;
			}
		},
		{
			{ "--no-cti" },
			"Disable CTI flag checking",
			[](argument_data& __restrict argument_data, const std::span<const char*>, uint32& __restrict) {
				argument_data.disable_cti = true;
			}
		},
		{
			{"--mmu"},
			"Specifies which MMU to use [emulated, none, host] [default: emulated].",
			[](argument_data& __restrict argument_data, const std::span<const char*> args, uint32& __restrict i) {
				if (i == args.size() - 1) {
					printf("Error: No MMU following --mmu option\n");
					_exit(1);
				}
				++i;
				const auto mmu_v = args[i];
				if (strcmp(mmu_v, "emulated") == 0) {
					argument_data.mmu_type = mips::mmu::emulated;
				}
				else if (strcmp(mmu_v, "none") == 0) {
					argument_data.mmu_type = mips::mmu::none;
				}
				else if (strcmp(mmu_v, "host") == 0) {
					argument_data.mmu_type = mips::mmu::host;
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
			[](argument_data& __restrict argument_data, const std::span<const char*>, uint32& __restrict) {
				argument_data.instruction_cache = true;
			}
		},
		{
			{"--jit0"},
			"Disable JIT usage in emulator",
			[](argument_data& __restrict argument_data, const std::span<const char*>, uint32& __restrict) {
				argument_data.jit = mips::JitType::None;
			}
		},
		{
			{"--jit1"},
			"Enable JIT in emulator (ticked mode not yet implemented) [default]",
			[](argument_data& __restrict argument_data, const std::span<const char*>, uint32& __restrict) {
				argument_data.jit = mips::JitType::Jit;
			}
		},
		{
			{"--jit2"},
			"Enable function table in emulator (ticked mode not yet implemented)",
			[](argument_data& __restrict argument_data, const std::span<const char*>, uint32& __restrict) {
				argument_data.jit = mips::JitType::FunctionTable;
			}
		},
		{
			{"--stats"},
			"Enable capturing of instruction stats",
			[](argument_data& __restrict argument_data, const std::span<const char*>, uint32& __restrict) {
				argument_data.instruction_stats = true;
			}
		},
		{
			{"-t", "--ticks"},
			"Enables ticked execution of emulator and increments by provided ticks",
			[](argument_data& __restrict argument_data, const std::span<const char*> args, uint32& __restrict i) {
				if (i == args.size() - 1) {
					fprintf(stderr, "Error: No quantity following --ticks option\n");
					_exit(1);
				}
				++i;
				argument_data.ticks = std::stoull(args[i], nullptr, 0);
				if (argument_data.ticks == 0) {
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
		printf("Digital Carbide DCMIPSr6E Emulator %u.%u.%u (%s %s)\n", Version.Major, Version.Minor, Version.Build, __DATE__, __TIME__);
	}

#if !EMSCRIPTEN
	static void help() {
		version();
		printf("OVERVIEW: Emulator for MIPSr6 binaries\n\n");
		printf("USAGE: emulator.exe [options] <binary>\n\n");
		printf("OPTIONS:\n");
		for (const auto& option : Options) {
			bool first = true;
			for (const std::string& opt : option.option_str) {
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

	static int initialize(const std::span<const char*> args, argument_data & __restrict arguments) {
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

		argument_data argument_data;

		const char * __restrict binary_file = nullptr;

#if !EMSCRIPTEN
		for (uint32 i = 1; i < args.size(); ++i) {
			bool optionFound = false;
			for (const auto& option : Options) {
				bool matches_any = false;
				for (const auto& str : option.option_str) {
					if (str == args[i]) {
						matches_any = true;
						break;
					}
				}
				if (matches_any) {
					option.procedure(argument_data, args, i);
					optionFound = true;
				}
			}

			if (!optionFound) {

				if (args[i][0] == '-') {
					fprintf(stderr, "Unknown option: %s\n", args[i]);
					return 1;
				}
				if (binary_file && binary_file[0] != '\0') {
					fprintf(stderr, "Cannot open multiple binary files\n");
					return 1;
				}

				binary_file = args[i];
			}
		}
#else
		binary_file = "tmp/uploaded.bin";
#endif

		if (mips::is_max_value(argument_data.stack_memory)) {
			argument_data.stack_memory = argument_data.available_memory / 2;
		}

		if ((argument_data.available_memory - argument_data.stack_memory) < 0x1000) {
			fprintf(stderr, "There must be at least 4KiB of memory available to the system after removing stack reservation.\n");
			return 1;
		}

		if (argument_data.use_rox && argument_data.mmu_type != mips::mmu::emulated) {
			fprintf(stderr, "--rox requires --mmu emulated\n");
			return 1;
		}

		if (argument_data.mmu_type != mips::mmu::emulated && !argument_data.instruction_cache && argument_data.jit != mips::JitType::None) {
			fprintf(stderr, "--no-mmu requires --icache when being used with a JIT.\n");
			return 1;
		}

		if (argument_data.mmu_type != mips::mmu::emulated && !argument_data.instruction_cache && argument_data.jit == mips::JitType::None) {
			fprintf(stdout, "warning: --no-mmu requires --icache to achieve behavioural parity with JIT.\n");
		}

		version();

		// print options
		fprintf(stdout, "Options:\n");
		fprintf(stdout, "\t%u bytes memory\n", argument_data.available_memory);
		fprintf(stdout, "\t%u bytes reserved for stack\n", argument_data.stack_memory);
		switch (argument_data.jit) {
		case mips::JitType::None:
			fprintf(stdout, "\tInterpreted Mode\n"); break;
		case mips::JitType::Jit:
			fprintf(stdout, "\tJIT1 Mode\n"); break;
		case mips::JitType::FunctionTable:
			fprintf(stdout, "\tFunction Table Mode\n"); break;
		}
		fprintf(stdout, "\tROX (Read-only Executable) mode %s\n", argument_data.use_rox ? "enabled" : "disabled");
		switch (argument_data.mmu_type) {
		case mips::mmu::emulated:
			fprintf(stdout, "\tMemory Management Unit: emulated\n"); break;
		case mips::mmu::none:
			fprintf(stdout, "\tMemory Management Unit: none\n"); break;
		case mips::mmu::host:
			fprintf(stdout, "\tMemory Management Unit: host\n"); break;
		}
		fprintf(stdout, "\tInstruction statistics %s\n", argument_data.instruction_stats ? "enabled" : "disabled");
		fprintf(stdout, "\tInstruction cache %s\n", argument_data.instruction_cache ? "enabled" : "disabled");
		if (argument_data.ticks == 0) {
			fprintf(stdout, "\tRealtime-mode enabled\n");
		}
		else {
			fprintf(stdout, "\tLockstep-mode enabled, %zu cycles\n", argument_data.ticks);
		}

		{
#if !EMSCRIPTEN
			FILE* fp = fopen(binary_file, "rb");
			if (!fp) {
				fprintf(stderr, "Could not open binary \"%s\"\n", binary_file);
				return 1;
			}
			_fseeki64(fp, 0, SEEK_END);
			const size_t data_size = _ftelli64(fp);
			rewind(fp);
			argument_data.binary_data.resize(data_size);
			if (data_size != fread(argument_data.binary_data.data.get(), 1, argument_data.binary_data.size, fp)) {
				fprintf(stderr, "Could not read binary \"%s\"\n", binary_file);
				fclose(fp);
				return 1;
			}
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

			arguments = std::move(argument_data);
			return 0;
		}
	}
}

int main(const int argc, const char **argv) {
	argument_data argument_data;

	if (const int result = initialize({ argv, argv + argc }, argument_data); _unlikely(result != 0)) {
		return result;
	}

	auto start_time = std::chrono::high_resolution_clock::now();
	uint64 instructions = 0;

	using statistic = std::pair<const char *, size_t>;

	std::vector<statistic> statistics;
	size_t largest_jit_instruction = 0;

	std::unique_ptr<mips::system> system;

	try {
		printf("Processing ELF Binary\n");
		elf::binary elf_binary{ argument_data.binary_data };
		printf("Configuring VCPU and Configurating VENV\n");
		
		mips::system::options sys_options = {
			.total_memory = argument_data.available_memory,
			.stack_memory = argument_data.stack_memory,
			.jit_type = argument_data.jit,
			.mmu_type = argument_data.mmu_type,
			.debug_port = argument_data.debug.port,
			.read_only_exec = argument_data.use_rox,
			.record_instruction_stats = argument_data.instruction_stats,
			.disable_cti = argument_data.disable_cti,
			.ticked = argument_data.ticks != 0,
			.instruction_cache = argument_data.instruction_cache,
			.debug = argument_data.debug.enabled,
			.debug_owned = argument_data.debug.enabled
		};
		try {
			sys_options.validate();
		}
		catch (const std::string& ex) {
			fprintf(stderr, "Failed to validate system options:\n\t%s\n", ex.c_str());
			return 1;
		}

		system = std::make_unique<mips::system_vemix>(sys_options, elf_binary);

		printf("Beginning Execution ---\n\n");
		fflush(stdout);
		start_time = std::chrono::high_resolution_clock::now();
		try {
			for (;;) {
				system->clock(argument_data.ticks);
				//printf("Instructions: %llu\n", processor.get_instruction_count());
			}
		}
		catch (...) {
			instructions = system->get_instruction_count();
			if (argument_data.instruction_stats) {
				statistics.reserve(system->get_stats_map().size());
				for (const auto &statistic : system->get_stats_map()) {
					statistics.emplace_back(statistic);
				}
				largest_jit_instruction = system->get_jit_max_instruction_size();
			}
			throw;
		}
	}
	catch (const std::runtime_error& exception) {
		fprintf(stderr, "\n** Error: %s\n", exception.what());
	}
	catch (mips::ExecutionCompleteException) {
		printf("\n** 'main' return detected - execution terminated\n");
	}
	catch (mips::ExecutionFailException) {
		fprintf(stderr, "\n** Unhandled CPU exception - execution terminated\n");
	}
	catch (...) {
		fprintf(stderr, "\n** Error\n");
	}

	const auto end_time = std::chrono::high_resolution_clock::now();
	const auto duration = end_time - start_time;
	const uint64 ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

	const double seconds = double(ns) / 1'000'000'000.0;
	const double ips = double(instructions) / seconds;
	const uint64 ips_rounded = size_t(round(ips));

	if (const uint64 cpu_frequency = GetHostFrequency(); cpu_frequency == 1) {
		printf("** Execution Duration: %llu ms (%llu ips)\n", ns / 1'000'000, ips_rounded);
	}
	else {
		const double frequency_ratio = double(cpu_frequency * 1'000'000) / ips;

		printf("** Execution Duration: %llu ms (%llu ips - 1:%.2f guest/host)\n", ns / 1'000'000, ips_rounded, frequency_ratio);
	}
	printf("** Instructions Executed: %llu\n", instructions);

	if (argument_data.instruction_stats) {
		printf("** Collected Instruction Stats:\n\n");

		if (largest_jit_instruction) {
			printf("\tLargest JIT Instruction: %zu bytes\n\n", largest_jit_instruction);
		}

		struct final {
			bool operator()(const statistic & __restrict a, const statistic & __restrict b) const __restrict {
				return a.second > b.second;
			}	
		} constexpr custom_comparator;

		std::sort(statistics.begin(), statistics.end(), custom_comparator);
		for (const statistic &stat_pair : statistics) {
			printf("\t%s - %zu\n", stat_pair.first, stat_pair.second);
		}
	}

	getchar();

	return 0;
}
