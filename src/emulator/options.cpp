#include "pch.hpp"
#include "options.hpp"

#include <cstdlib>
#include <cstdio>
#include <cwchar>
#include <cwctype>

#include <array>
#include <expected>
#include <limits>
#include <span>
#include <fmt/format.h>

#include "changes.hpp"


namespace vemips::options
{
	static constexpr const version version = { 1, 0, 0 };

	static void print_help();

	// ReSharper disable CppDeclaratorNeverUsed
	#pragma warning(suppress : 4505)
	static long long string_to_ll(const wchar_t * __restrict str, const int base = 10) {
		return std::wcstoll(str, nullptr, base);
	}

	#pragma warning(suppress : 4505)
	static unsigned long long string_to_ull(const wchar_t * __restrict str, const int base = 10) {
		return std::wcstoull(str, nullptr, base);
	}

	#pragma warning(suppress : 4505)
	static long long string_to_ll(const char * __restrict str, const int base = 10) {
		return std::strtoll(str, nullptr, base);
	}

	#pragma warning(suppress : 4505)
	static unsigned long long string_to_ull(const char * __restrict str, const int base = 10) {
		return std::strtoull(str, nullptr, base);
	}
	// ReSharper restore CppDeclaratorNeverUsed

	#pragma warning(suppress : 4505)
	static char to_lower_char(const char c)
	{
		return static_cast<char>(std::tolower(c));
	}

	#pragma warning(suppress : 4505)
	static wchar_t to_lower_char(const wchar_t c)
	{
		return static_cast<wchar_t>(std::towlower(c));
	}

	static bool insensitive_equals(const tstring_view& a, const tstring_view& b)
	{
		return std::ranges::equal(a, b, [](const auto& ac, const auto& bc) { return to_lower_char(ac) == to_lower_char(bc); });
	}

	static std::optional<bool> parse_value_bool(const std::optional<tstring_view>& value)
	{
		if (!value)
		{
			return {};
		}

		if (
			insensitive_equals(*value, TSTR("off")) ||
			insensitive_equals(*value, TSTR("no")) ||
			insensitive_equals(*value, TSTR("false")) ||
			insensitive_equals(*value, TSTR("0"))
		)
		{
			return false;
		}
		else if (
			insensitive_equals(*value, TSTR("on")) ||
			insensitive_equals(*value, TSTR("yes")) ||
			insensitive_equals(*value, TSTR("true")) ||
			insensitive_equals(*value, TSTR("1"))
		)
		{
			return true;
		}

		return {};
	}

#ifndef EMSCRIPTEN
	static std::array<option, 17> get_options ()
	{
		return {
			option{
				{TSTR("-m"), TSTR("--memory")},
				TSTR("Specify how much memory to which the CPU shall have access [default: 1048576]"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i, std::optional<tstring_view>&& value) {
					const auto memory = [&] {
						if (value)
						{
							// todo : suboptimal
							const tstring value_str {value.value()};
							return string_to_ll(value_str.c_str(), 0);
						}

						if (i == args.size() - 1) [[unlikely]] {
							fmt::println(stderr, "Error: No quantity following -m option");
							std::exit(1);
						}
						++i;
						return string_to_ll(args[i], 0);
					}();

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
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i, std::optional<tstring_view>&& value) {
					const auto memory = [&] {
						if (value)
						{
							// todo : suboptimal
							const tstring value_str {value.value()};
							return string_to_ll(value_str.c_str(), 0);
						}

						if (i == args.size() - 1) [[unlikely]] {
							fmt::println(stderr, "Error: No quantity following -s option");
							std::exit(1);
						}
						++i;
						return string_to_ll(args[i], 0);
					}();

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
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i, std::optional<tstring_view>&& value) {
					if (const auto bool_value = parse_value_bool(value))
					{
						argument_data.debug.enabled = *bool_value;
					}

					const auto port = [&] {
						if (value)
						{
							// todo : suboptimal
							const tstring value_str {value.value()};
							return string_to_ll(value_str.c_str(), 0);
						}

						if (i == args.size() - 1) [[unlikely]] {
							fmt::println(stderr, "Error: No port following ---debug option");
							std::exit(1);
						}
						++i;
						return string_to_ll(args[i], 0);
					}();

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
				[](argument_data& __restrict, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					if (value) [[unlikely]]
					{
						fmt::println(stderr, TSTR("Error: Unexpected value provided for `--help`: {}"), *value);
						// std::exit(1);
					}

					print_help();
					std::exit(0);
				}
			},
			{
				{TSTR("-v"), TSTR("--version")},
				TSTR("Displays tool version information"),
				[](argument_data& __restrict, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					if (value) [[unlikely]]
					{
						fmt::println(stderr, TSTR("Error: Unexpected value provided for `--version`: {}"), *value);
						// std::exit(1);
					}

					print_version();
					std::exit(0);
				}
			},
			{
				{TSTR("--changes")},
				TSTR("Displays most recent changes to build"),
				[](argument_data& __restrict, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					if (value) [[unlikely]]
					{
						fmt::println(stderr, TSTR("Error: Unexpected value provided for `--changes`: {}"), *value);
						std::exit(1);
					}

					print_changes();
					std::exit(0);
				}
			},
			{
				{TSTR("--rox")},
				TSTR("Makes executable memory read-only. [cannot be combined with --nommu]"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					bool b_value = true;

					if (const auto bool_value = parse_value_bool(value))
					{
						b_value = *bool_value;
					}

					argument_data.use_rox = b_value;
				}
			},
			{
				{TSTR("--strict-noncoherence")},
				TSTR("Toggles instructions being read from shadow memory, simulating non-coherent instruction caches."),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					bool b_value = true;

					if (const auto bool_value = parse_value_bool(value))
					{
						b_value = *bool_value;
					}

					argument_data.strict_noncoherence = b_value;
				}
			},
			{
				{TSTR("--cti")},
				TSTR("Determine CTI flag checking"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					bool b_value = true;

					if (const auto bool_value = parse_value_bool(value))
					{
						b_value = *bool_value;
					}

					argument_data.disable_cti = b_value;
				}
			},
			{
				{TSTR("--mmu")},
				TSTR("Specifies which MMU to use [emulated, none, host] [default: emulated]."),
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i, std::optional<tstring_view>&& value) {
					const auto mmu_v = [&] {
						if (value)
						{
							return value.value();
						}

						if (i == args.size() - 1) [[unlikely]] {
							fmt::println(stderr, "Error: No MMU following --mmu option");
							std::exit(1);
						}
						++i;
						return tstring_view{args[i]};
					}();
					if (insensitive_equals(mmu_v, TSTR("emulated"))) {
						argument_data.mmu_type = mips::mmu::emulated;
					}
					else if (insensitive_equals(mmu_v, TSTR("none"))) {
						argument_data.mmu_type = mips::mmu::none;
					}
					else if (insensitive_equals(mmu_v, TSTR("host"))) {
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
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					bool b_value = true;

					if (const auto bool_value = parse_value_bool(value))
					{
						b_value = *bool_value;
					}

					argument_data.instruction_cache = b_value;
				}
			},
			{
				{TSTR("--jit")},
				TSTR("Specify JIT usage in emulator (`jit` is default)"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i, std::optional<tstring_view>&& value) {
					const auto jit_nv = [&] {
						if (value)
						{
							return value.value();
						}

						if (i == args.size() - 1) [[unlikely]] {
							fmt::println(stderr, "Error: No MMU following --mmu option");
							std::exit(1);
						}
						++i;
						return tstring_view{args[i]};
					}();

					const auto bool_value = parse_value_bool(value);

					if (
						insensitive_equals(jit_nv, TSTR("none")) ||
						(bool_value && !*bool_value)
					) {
						argument_data.jit = mips::JitType::None;
					}
					else if (
						insensitive_equals(jit_nv, TSTR("jit")) ||
						insensitive_equals(jit_nv, TSTR("default")) ||
						(bool_value && !bool_value)
					) {
						argument_data.jit = mips::JitType::Jit;
					}
					else [[unlikely]] {
						fmt::println(stderr, TSTR("Error: The provided JIT (\'{}\') is not a valid JIT type"), jit_nv);
						std::exit(1);
					}
				}
			},
			{
				{TSTR("--jit0")},
				TSTR("Disable JIT usage in emulator (deprecated)"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					if (value) [[unlikely]]
					{
						fmt::println(stderr, TSTR("Error: Unexpected value provided for `--jit0`: {}"), *value);
						std::exit(1);
					}

					argument_data.jit = mips::JitType::None;
				}
			},
			{
				{TSTR("--jit1")},
				TSTR("Enable JIT in emulator (deprecated) [default]"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					if (value) [[unlikely]]
					{
						fmt::println(stderr, TSTR("Error: Unexpected value provided for `--jit1`: {}"), *value);
						std::exit(1);
					}

					argument_data.jit = mips::JitType::Jit;
				}
			},
			{
				{TSTR("--stats")},
				TSTR("Enable capturing of statistics"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
					bool b_value = true;

					if (const auto bool_value = parse_value_bool(value))
					{
						b_value = *bool_value;
					}

					argument_data.collect_statistics = b_value;
				}
			},
			{
				{TSTR("-t"), TSTR("--ticks")},
				TSTR("Enables ticked execution of emulator and increments by provided ticks"),
				[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i, std::optional<tstring_view>&& value) {
					auto ticks_v = [&] {
						if (value)
						{
							tstring value_str { value.value() };
							return string_to_ull(value_str.c_str(), 0);
						}

						if (i == args.size() - 1) [[unlikely]] {
							fmt::println(stderr, "Error: No quantity following --ticks option");
							std::exit(1);
						}
						++i;
						return string_to_ull(args[i], 0);
					}();

					if (const auto bool_value = parse_value_bool(value))
					{
						if (!*bool_value)
						{
							ticks_v = 0;
						}
						else [[unlikely]]
						{
							fmt::println(stderr, TSTR("Error: Unexpected value following --ticks option: `{}`"), *value);
							std::exit(1);
						}
					}

					argument_data.ticks = ticks_v;
				}
			},
		};
	}
#endif

	void print_version()
	{
		fmt::println(
			"Digital Carbide DCMIPSr6E Emulator {}.{}.{} (" __DATE__ " " __TIME__ ")",
			version.major,
			version.minor,
			version.build
		);
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
				fmt::println(TSTR("{} {}"), first ? TSTR(" ") : TSTR(""), opt);
				first = false;
			}
			fmt::println(TSTR("\n		{}"), option.description);
		}
	}
#endif

	std::expected<argument_data, int> parse(const std::span<const tchar*> args)
	{
		argument_data data;

#ifdef EMSCRIPTEN
		data.binary_file = TSTR("tmp/uploaded.bin");
#else
		for (std::size_t i = 1; i < args.size(); ++i) {
			tstring_view arg = args[i];
			std::optional<tstring_view> arg_value;

			const std::size_t colon_index = arg.find(TSTR(':'));
			const std::size_t equal_index = arg.find(TSTR('='));
			std::size_t delimiter_index;
			if (colon_index == tstring_view::npos)
			{
				delimiter_index = equal_index;
			}
			else if (equal_index == tstring_view::npos)
			{
				delimiter_index = colon_index;
			}
			else
			{
				delimiter_index = colon_index < equal_index ? colon_index : equal_index;
			}

			if (delimiter_index != tstring_view::npos)
			{
				arg_value = arg.substr(delimiter_index + 1);
				arg = arg.substr(0, delimiter_index);
			}

			bool option_found = false;
			for (const auto& option : get_options()) {
				bool matches_any = false;
				for (const auto& str : option.option_str) {
					if (str == arg) {
						matches_any = true;
						break;
					}
				}
				if (matches_any) {
					option.procedure(data, args, i, std::move(arg_value));
					option_found = true;
				}
			}

			if (!option_found) {
				if (arg[0] == TSTR('-')) [[unlikely]] {
					fmt::println(stderr, TSTR("Unknown option: {}"), args[i]);
					return std::unexpected(1);
				}
				if (data.binary_file && data.binary_file[0] != TSTR('\0')) [[unlikely]] {
					fmt::println(stderr, "Cannot open multiple binary files");
					return std::unexpected(1);
				}

				data.binary_file = args[i];
			}
		}
#endif

		if (mips::is_max_value(data.stack_memory)) {
			data.stack_memory = data.available_memory / 2;
		}

		if ((data.available_memory - data.stack_memory) < 0x1000) [[unlikely]] {
			fmt::println(stderr, "There must be at least 4KiB of memory available to the system after removing stack reservation.");
			return std::unexpected(1);
		}

		if (data.use_rox && data.mmu_type != mips::mmu::emulated) [[unlikely]] {
			fmt::println(stderr, "--rox requires --mmu emulated");
			return std::unexpected(1);
		}

		if (data.mmu_type != mips::mmu::emulated && !data.instruction_cache && data.jit != mips::JitType::None) [[unlikely]] {
			fmt::println(stderr, "--no-mmu requires --icache when being used with a JIT.");
			return std::unexpected(1);
		}

		if (data.mmu_type != mips::mmu::emulated && !data.instruction_cache && data.jit == mips::JitType::None) [[unlikely]] {
			fmt::println(stderr, "warning: --no-mmu requires --icache to achieve behavioral parity with JIT.");
		}

		print_version();

		// print options
		fmt::println("Options:");
		fmt::println("\t{} bytes memory", data.available_memory);
		fmt::println("\t{} bytes reserved for stack", data.stack_memory);
		switch (data.jit) {
			case mips::JitType::None:
				fmt::println("\tInterpreted Mode"); break;
			case mips::JitType::Jit:
				fmt::println("\tJIT1 Mode"); break;
		}
		fmt::println("\tROX (Read-only Executable) mode {}", data.use_rox ? "enabled" : "disabled");
		switch (data.mmu_type) {
		case mips::mmu::emulated:
			fmt::println("\tMemory Management Unit: emulated"); break;
		case mips::mmu::none:
			fmt::println("\tMemory Management Unit: none"); break;
		case mips::mmu::host:
			fmt::println("\tMemory Management Unit: host"); break;
		}
		fmt::println("\tCollect Statistics {}", data.collect_statistics ? "enabled" : "disabled");
		fmt::println("\tInstruction cache {}", data.instruction_cache ? "enabled" : "disabled");
		if (data.ticks == 0) {
			fmt::println("\tRealtime-mode enabled");
		}
		else {
			fmt::println("\tLockstep-mode enabled, {} cycles", data.ticks);
		}

#ifndef EMSCRIPTEN
#	if UNICODE
		FILE* const fp = _wfopen(data.binary_file, L"rb");
#	else
		FILE* const fp = std::fopen(data.binary_file, "rb");
#	endif
		if (!fp) [[unlikely]]
		{
			fmt::println(stderr, TSTR("Could not open binary \"{}\""), data.binary_file);
			return std::unexpected(1);
		}
		if (_fseeki64(fp, 0, SEEK_END) != 0) [[unlikely]]
		{
			fmt::println(stderr, TSTR("Failed to seek in binary \"{}\""), data.binary_file);
			return std::unexpected(1);
		}
		const size_t data_size = _ftelli64(fp);
		if (_fseeki64(fp, 0, SEEK_SET) != 0) [[unlikely]]
		{
			fmt::println(stderr, TSTR("Failed to seek in binary \"{}\""), data.binary_file);
			return std::unexpected(1);
		}
		data.binary_data.resize(data_size);
		if (data_size != std::fread(data.binary_data.data.get(), 1, data.binary_data.size, fp)) [[unlikely]] {
			fmt::println(TSTR("Could not read binary \"{}\""), data.binary_file);
			std::ignore = std::fclose(fp);
			return std::unexpected(1);
		}
		std::ignore = std::fclose(fp);
#else
		for (;;) {
			char byte = std::fgetc(stdin);
			if (byte == EOF) {
				break;
			}
			data.binary_data.push_back(byte);
		}
		data.binary_data = base64::decode(data.binary_data);
#endif

		return data;
	}
}
