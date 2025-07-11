#include "pch.hpp"

#include <cassert>
#include <chrono>
#include <cstdio>

#include <functional>
#include <span>
#include <string>
#include <vector>

#include "platform.hpp"
#include "system_vemix.hpp"
#include "elf/elf.hpp"
#include "mips/mips.hpp"


namespace {
	using namespace mips;

#ifdef EMSCRIPTEN
#	include "base64.hpp"
#endif

	struct version final {
		uint32 major;
		uint32 minor;
		uint32 build;
	}
	static constexpr const version = { 1, 0, 0 };

	static void print_version();
	static void print_help();
	static void print_changes();

	template <typename T>
	struct unique_span final {
		std::unique_ptr<T[]> data;
		size_t size = 0;

		_nothrow _forceinline unique_span() = default;

		_nothrow _forceinline unique_span(const size_t size) :
			data(std::make_unique<T[]>(size)),
			size(size) {
		}

		_nothrow _forceinline void resize (const size_t new_size) {
			if (size == new_size) {
				return;
			}

			data = std::make_unique<T[]>(new_size);
			size = new_size;
		}

		_nothrow _forceinline operator std::span<T>() const {
			return { data.get(), data.get() + size };
		}
	};

	struct argument_data final {
		unique_span<char> binary_data;
		uint64 ticks = 0;
		uint32 available_memory = 0x100000 * 2;
		uint32 stack_memory = uint32(-1);
		mips::JitType jit = mips::JitType::Jit;
		mips::mmu mmu_type = mips::mmu::none;
		struct {
			uint16 port = 0;
			bool enabled = false;
		} debug;
		bool collect_statistics = false;
		bool disable_cti = false;
		bool instruction_cache = true;
		bool use_rox = false;
	};

#ifndef EMSCRIPTEN
	struct option final {
		std::vector<tstring_view> option_str;
		tstring_view description;
		std::function<void(argument_data & __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i)> procedure;
	};
#endif

#if UNICODE
#	define tstrcmp(a, b) wcscmp(a, L ## b)
#else
#	define tstrcmp(a, b) std::strcmp(a, b)
#endif

	static long long string_to_ll(const wchar_t * __restrict str, const int base = 10) {
		return std::wcstoll(str, nullptr, base);
	}

	static unsigned long long string_to_ull(const wchar_t * __restrict str, const int base = 10) {
		return std::wcstoull(str, nullptr, base);
	}

	static long long string_to_ll(const char * __restrict str, const int base = 10) {
		return std::strtoll(str, nullptr, base);
	}

	static unsigned long long string_to_ull(const char * __restrict str, const int base = 10) {
		return std::strtoull(str, nullptr, base);
	}

#ifndef EMSCRIPTEN
	static std::array<option, 15> get_options ()
	{
		return {
			option{
				{TSTR("-m"), TSTR("--memory")},
				TSTR("Specify how much memory to which the CPU shall have access [default: 1048576]"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i) {
					if (i == args.size() - 1) [[unlikely]] {
						fmt::println(stderr, "Error: No quantity following -m option");
						std::exit(1);
					}
					++i;
					const auto memory = string_to_ll(args[i], 0);
					if (memory < 4096) [[unlikely]] {
						fmt::println(stderr, "Error: You cannot specify < 4096 bytes of memory");
						std::exit(1);
					}
					if (memory > std::numeric_limits<uint32>::max()) [[unlikely]] {
						fmt::println(stderr, "Error: You cannot specify greater than or equal to 2^32 bytes of memory");
						std::exit(1);
					}
					argument_data.available_memory = uint32(memory);
				}
			},
			{
				{TSTR("-s"), TSTR("--stack")},
				TSTR("Specify how much memory will be reserved for the stack [default: memory / 2]"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i) {
					if (i == args.size() - 1) [[unlikely]] {
						fmt::println(stderr, "Error: No quantity following -s option");
						std::exit(1);
					}
					++i;
					const auto memory = string_to_ll(args[i], 0);
					if (memory < 0) [[unlikely]] {
						fmt::println(stderr, "Error: You cannot specify < 0 bytes of stack memory");
						std::exit(1);
					}
					if (memory > std::numeric_limits<uint32>::max()) [[unlikely]] {
						fmt::println(stderr, "Error: You cannot specify greater than or equal to 2^32 bytes of stack memory");
						std::exit(1);
					}
					argument_data.stack_memory = uint32(memory);
				}
			},
			{
				{TSTR("--debug")},
				TSTR("Enable the debugger. GDB Port must follow."),
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i) {
					if (i == args.size() - 1) [[unlikely]] {
						fmt::println(stderr, "Error: No port following ---debug option");
						std::exit(1);
					}
					++i;
					const auto port = string_to_ll(args[i], 0);
					if (!mips::within(port, std::numeric_limits<uint16>{})) [[unlikely]] {
						fmt::println(stderr, "Error: Provided debugger port number is out of range.");
						std::exit(1);
					}
					argument_data.debug = { uint16(port), true };
				}
			},
			{
				{TSTR("-h"), TSTR("--help")},
				TSTR("Displays tool help information [you are viewing this]"),
				[](argument_data& __restrict, const std::span<const tchar*>, std::size_t& __restrict) {
					print_help();
					std::exit(0);
				}
			},
			{
				{TSTR("-v"), TSTR("--version")},
				TSTR("Displays tool version information"),
				[](argument_data& __restrict, const std::span<const tchar*>, std::size_t& __restrict) {
					print_version();
					std::exit(0);
				}
			},
			{
				{TSTR("--changes")},
				TSTR("Displays most recent changes to build"),
				[](argument_data& __restrict, const std::span<const tchar*>, std::size_t& __restrict) {
					print_changes();
					std::exit(0);
				}
			},
			{
				{TSTR("--rox")},
				TSTR("Makes executable memory read-only. [cannot be combined with --nommu]"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict) {
					argument_data.use_rox = true;
				}
			},
			{
				{TSTR("--no-cti")},
				TSTR("Disable CTI flag checking"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict) {
					argument_data.disable_cti = true;
				}
			},
			{
				{TSTR("--mmu")},
				TSTR("Specifies which MMU to use [emulated, none, host] [default: emulated]."),
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i) {
					if (i == args.size() - 1) [[unlikely]] {
						fmt::println(stderr, "Error: No MMU following --mmu option");
						std::exit(1);
					}
					++i;
					const auto mmu_v = args[i];
					if (tstrcmp(mmu_v, "emulated") == 0) {
						argument_data.mmu_type = mips::mmu::emulated;
					}
					else if (tstrcmp(mmu_v, "none") == 0) {
						argument_data.mmu_type = mips::mmu::none;
					}
					else if (tstrcmp(mmu_v, "host") == 0) {
						argument_data.mmu_type = mips::mmu::host;
					}
					else [[unlikely]] {
						fmt::println(stderr, TSTR("Error: The provided MMU (\'{}\') is not a valid MMU type"), mmu_v);
						std::exit(1);
					}
				}
			},
			{
				{TSTR("--icache")},
				TSTR("Enables instruction cache."),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict) {
					argument_data.instruction_cache = true;
				}
			},
			{
				{TSTR("--jit0")},
				TSTR("Disable JIT usage in emulator"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict) {
					argument_data.jit = mips::JitType::None;
				}
			},
			{
				{TSTR("--jit1")},
				TSTR("Enable JIT in emulator (ticked mode not yet implemented) [default]"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict) {
					argument_data.jit = mips::JitType::Jit;
				}
			},
			{
				{TSTR("--jit2")},
				TSTR("Enable function table in emulator (ticked mode not yet implemented)"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict) {
					argument_data.jit = mips::JitType::FunctionTable;
				}
			},
			{
				{TSTR("--stats")},
				TSTR("Enable capturing of statistics"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict) {
					argument_data.collect_statistics = true;
				}
			},
			{
				{TSTR("-t"), TSTR("--ticks")},
				TSTR("Enables ticked execution of emulator and increments by provided ticks"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i) {
					if (i == args.size() - 1) [[unlikely]] {
						fmt::println(stderr, "Error: No quantity following --ticks option");
						std::exit(1);
					}
					++i;
					argument_data.ticks = string_to_ull(args[i], 0);
					if (argument_data.ticks == 0) [[unlikely]] {
						fmt::println(stderr, "Error: You cannot specify 0 ticks");
						std::exit(1);
					}
				}
			},
		};
	}
#endif

	static void print_changes() {
		fmt::println(
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

	static void print_version() {
		fmt::println("Digital Carbide DCMIPSr6E Emulator {}.{}.{} (" __DATE__ " " __TIME__ ")", version.major, version.minor, version.build);
	}

#ifndef EMSCRIPTEN
	static void print_help() {
		print_version();
		fmt::println(
			"OVERVIEW: Emulator for MIPSr6 binaries\n\n"
			"USAGE: emulator.exe [options] <binary>\n\n"
			"OPTIONS:"
		);
		for (const auto& option : get_options()) {
			bool first = true;
			for (const auto opt : option.option_str) {
				if (first) {
					fmt::println(TSTR("  {}"), opt.data()); // TODO : string_view not guaranteed null-terminated
				}
				else {
					fmt::println(TSTR(" {}"), opt.data()); // TODO : string_view not guaranteed null-terminated
				}
				first = false;
			}
			fmt::println(TSTR("\n		{}"), option.description);
		}
	}
#endif

	static int initialize(const std::span<const tchar*> args, argument_data & __restrict arguments) {
		{
			SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
			ULONG_PTR process_affinity, system_affinity;
			if (
				const BOOL result = GetProcessAffinityMask(GetCurrentProcess(), &process_affinity, &system_affinity);
				result != FALSE && process_affinity != 1
			) {
				process_affinity &= ~1ull;
				SetProcessAffinityMask(GetCurrentProcess(), process_affinity);
				Yield();
			}
		}

		argument_data argument_data;

		const tchar * __restrict binary_file = nullptr;

#ifndef EMSCRIPTEN
		for (std::size_t i = 1; i < args.size(); ++i) {
			bool option_found = false;
			for (const auto& option : get_options()) {
				bool matches_any = false;
				for (const auto& str : option.option_str) {
					if (str == args[i]) {
						matches_any = true;
						break;
					}
				}
				if (matches_any) {
					option.procedure(argument_data, args, i);
					option_found = true;
				}
			}

			if (!option_found) {

				if (args[i][0] == '-') [[unlikely]] {
					fmt::println(stderr, TSTR("Unknown option: {}"), args[i]);
					return 1;
				}
				if (binary_file && binary_file[0] != '\0') [[unlikely]] {
					fmt::println(stderr, "Cannot open multiple binary files");
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

		if ((argument_data.available_memory - argument_data.stack_memory) < 0x1000) [[unlikely]] {
			fmt::println(stderr, "There must be at least 4KiB of memory available to the system after removing stack reservation.");
			return 1;
		}

		if (argument_data.use_rox && argument_data.mmu_type != mips::mmu::emulated) [[unlikely]] {
			fmt::println(stderr, "--rox requires --mmu emulated");
			return 1;
		}

		if (argument_data.mmu_type != mips::mmu::emulated && !argument_data.instruction_cache && argument_data.jit != mips::JitType::None) [[unlikely]] {
			fmt::println(stderr, "--no-mmu requires --icache when being used with a JIT.");
			return 1;
		}

		if (argument_data.mmu_type != mips::mmu::emulated && !argument_data.instruction_cache && argument_data.jit == mips::JitType::None) [[unlikely]] {
			fmt::println(stderr, "warning: --no-mmu requires --icache to achieve behavioral parity with JIT.");
		}

		print_version();

		// print options
		fmt::println("Options:");
		fmt::println("\t{} bytes memory", argument_data.available_memory);
		fmt::println("\t{} bytes reserved for stack", argument_data.stack_memory);
		switch (argument_data.jit) {
			case mips::JitType::None:
			fmt::println("\tInterpreted Mode"); break;
			case mips::JitType::Jit:
			fmt::println("\tJIT1 Mode"); break;
		case mips::JitType::FunctionTable:
			fmt::println("\tFunction Table Mode"); break;
		}
		fmt::println("\tROX (Read-only Executable) mode {}", argument_data.use_rox ? "enabled" : "disabled");
		switch (argument_data.mmu_type) {
		case mips::mmu::emulated:
			fmt::println("\tMemory Management Unit: emulated"); break;
		case mips::mmu::none:
			fmt::println("\tMemory Management Unit: none"); break;
		case mips::mmu::host:
			fmt::println("\tMemory Management Unit: host"); break;
		}
		fmt::println("\tCollect Statistics {}", argument_data.collect_statistics ? "enabled" : "disabled");
		fmt::println("\tInstruction cache {}", argument_data.instruction_cache ? "enabled" : "disabled");
		if (argument_data.ticks == 0) {
			fmt::println("\tRealtime-mode enabled");
		}
		else {
			fmt::println("\tLockstep-mode enabled, {} cycles", argument_data.ticks);
		}

		{
#ifndef EMSCRIPTEN
#	if UNICODE
			FILE* fp = _wfopen(binary_file, L"rb");
#	else
			FILE* fp = std::fopen(binary_file, "rb");
#	endif
			if (!fp) {
				fmt::println(stderr, TSTR("Could not open binary \"{}\""), binary_file);
				return 1;
			}
			_fseeki64(fp, 0, SEEK_END);
			const size_t data_size = _ftelli64(fp);
			std::rewind(fp);
			argument_data.binary_data.resize(data_size);
			if (data_size != std::fread(argument_data.binary_data.data.get(), 1, argument_data.binary_data.size, fp)) [[unlikely]] {
				fmt::println(TSTR("Could not read binary \"{}\""), binary_file);
				std::fclose(fp);
				return 1;
			}
			std::fclose(fp);
#else
			for (;;) {
				char byte = std::fgetc(stdin);
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

	template <typename TChar> requires (std::same_as<TChar, char> || std::same_as<TChar, wchar_t>)
	int main_impl(const std::span<const TChar*> args) {
		argument_data argument_data;

		if (const int result = initialize(args, argument_data); _unlikely(result != 0)) [[unlikely]] {
			return result;
		}

		auto start_time = std::chrono::high_resolution_clock::now();
		uint64 instructions = 0;

		processor::statistics statistics;
		size_t largest_jit_instruction = 0;

		try {
			std::unique_ptr<mips::system> system;

			fmt::println("Processing ELF Binary");
			elf::binary elf_binary{ argument_data.binary_data };
			fmt::println("Configuring VCPU and Configuring VENV");
			
			mips::system::options sys_options = {
				.total_memory = argument_data.available_memory,
				.stack_memory = argument_data.stack_memory,
				.jit_type = argument_data.jit,
				.mmu_type = argument_data.mmu_type,
				.debug_port = argument_data.debug.port,
				.read_only_exec = argument_data.use_rox,
				.collect_statistics = argument_data.collect_statistics,
				.disable_cti = argument_data.disable_cti,
				.ticked = argument_data.ticks != 0,
				.instruction_cache = argument_data.instruction_cache,
				.debug = argument_data.debug.enabled,
				.debug_owned = argument_data.debug.enabled
			};
			try {
				sys_options.validate();
			}
			catch (const std::exception& ex) {
				[[unlikely]]
				fmt::println(stderr, "Failed to validate system options:\n\t{}", ex.what());
				return 1;
			}

			system = std::make_unique<mips::system_vemix>(sys_options, elf_binary);

			fmt::println("Beginning Execution ---");
			std::fflush(stdout);
			start_time = std::chrono::high_resolution_clock::now();
			try {
				for (;;) {
					system->clock(argument_data.ticks);
					//fmt::println("Instructions: {}", processor.get_instruction_count());
				}
			}
			catch (...) {
				instructions = system->get_instruction_count();
				if (argument_data.collect_statistics) {
					statistics.append(*system->get_statistics());
					largest_jit_instruction = system->get_jit_max_instruction_size();
				}
				throw;
			}
		}
		catch (const std::runtime_error& exception) {
			fmt::println(stderr, "\n** Error: {}", exception.what());
		}
		catch (mips::ExecutionCompleteException) {
			fmt::println("\n** 'main' return detected - execution terminated");
		}
		catch (mips::ExecutionFailException) {
			fmt::println(stderr, "\n** Unhandled CPU exception - execution terminated");
		}
		catch (...) {
			fmt::println(stderr, "\n** Error");
		}

		const auto end_time = std::chrono::high_resolution_clock::now();
		const auto duration = end_time - start_time;
		const uint64 ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

		const double seconds = double(ns) / 1'000'000'000.0;
		const double ips = double(instructions) / seconds;
		const uint64 ips_rounded = size_t(round(ips));

		if (const uint64 cpu_frequency = mips::platform::get_host_frequency(); cpu_frequency == 1) {
			fmt::println("** Execution Duration: {} ms ({} ips)", ns / 1'000'000, ips_rounded);
		}
		else {
			const double frequency_ratio = double(cpu_frequency * 1'000'000) / ips;

			fmt::println("** Execution Duration: {} ms ({} ips - 1:{:.2f} guest/host)", ns / 1'000'000, ips_rounded, frequency_ratio);
		}
		fmt::println("** Instructions Executed: {}", instructions);

		if (argument_data.collect_statistics) {
			fmt::println("** Collected Statistics:");

			if (largest_jit_instruction) {
				fmt::println("\tLargest JIT Instruction: {} bytes", largest_jit_instruction);
			}
			fmt::println("\tJIT Transitions: {}", statistics.jit_transitions);
			std::puts("");

			struct final {
				bool operator()(const std::pair<const char *, size_t> & __restrict a, const std::pair<const char *, size_t> & __restrict b) const __restrict {
					return a.second > b.second;
				}	
			} constexpr custom_comparator;

			const auto dump_instructions = [&](const char* const tab, const processor::statistics::instruction_map& data) {
				std::vector<std::pair<const char *, size_t>> instruction_stats;
				instruction_stats.reserve(data.size());
				for (const auto& item : data)
				{
					instruction_stats.emplace_back(item);
				}

				std::ranges::stable_sort(instruction_stats, custom_comparator);

				size_t max_len = 0;
				for (const auto &stat_pair : instruction_stats) {
					max_len = std::max(max_len, std::strlen(stat_pair.first));
				}

				for (const auto &stat_pair : instruction_stats) {
					fmt::println("{}{:<{}} - {}", tab, stat_pair.first, max_len, stat_pair.second);
				}
			};

			std::puts("\tAll Instructions:");
			dump_instructions("\t\t", statistics.instructions);
			if (argument_data.jit != JitType::None)
			{
				std::puts("");
				std::puts("\tEmulated Instructions:");
				dump_instructions("\t\t", statistics.jit_emulated_instructions);
			}
		}

		std::getchar();

		return 0;
	}
}

#ifdef UNICODE
// ReSharper disable once IdentifierTypo
int wmain(const int argc, const wchar_t* argv[]) {
	return main_impl(std::span{ argv, argv + argc });
}
#else
int main(const int argc, const char* argv[]) {
	return main_impl(std::span{ argv, argv + argc });
}
#endif
