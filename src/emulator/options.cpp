#include "pch.hpp"
#include "options.hpp"
#include <common.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cwctype>
#include <expected>
#include <limits>
#include <span>
#include <fmt/format.h>
#if UNICODE
#	include <fmt/xchar.h>
#endif

#include "changes.hpp"
#include "string_utils.hpp"


namespace vemips::options
{
	static constexpr version version = { 0, 1, 1 };

#ifndef EMSCRIPTEN
	static _nothrow void print_help(tzstring_view executable_path) noexcept;
#endif

	namespace
	{
		[[nodiscard]]
		static _pure _nothrow std::optional<bool> parse_value_bool(const std::optional<tstring_view>& value) noexcept
		{
			if (!value)
			{
				return {};
			}

			if (
				string_utils::insensitive_equals(*value, TSTR("off")) ||
				string_utils::insensitive_equals(*value, TSTR("no")) ||
				string_utils::insensitive_equals(*value, TSTR("false")) ||
				string_utils::insensitive_equals(*value, TSTR("0"))
			)
			{
				return false;
			}
			else if (
				string_utils::insensitive_equals(*value, TSTR("on")) ||
				string_utils::insensitive_equals(*value, TSTR("yes")) ||
				string_utils::insensitive_equals(*value, TSTR("true")) ||
				string_utils::insensitive_equals(*value, TSTR("1"))
			)
			{
				return true;
			}

			return {};
		}

#ifndef EMSCRIPTEN
		[[nodiscard]]
		static _nothrow std::array<option, 18> get_options(const tzstring_view executable_path) noexcept
		{
			return {
				option{
					{TSTR("-m"), TSTR("--memory")},
					TSTR("Specify how much memory to which the CPU shall have access [default: 2097152]"),
					[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i, std::optional<tstring_view>&& value) {
						const auto memory = [&] {
							if (value)
							{
								// todo : suboptimal
								const tstring value_str {value.value()};
								return string_utils::string_to_ll(value_str.c_str(), 0);
							}

							if (i == args.size() - 1) [[unlikely]] {
								fmt::println(stderr, "Error: No quantity following -m option");
								std::exit(1);
							}
							++i;
							return string_utils::string_to_ll(args[i], 0);
						}();

						if (memory < 4096) [[unlikely]] {
							fmt::println(stderr, "Error: You cannot specify < 4096 bytes of memory");
							std::exit(1);
						}
						xassert(memory >= 0); // Implied by above check
						if (mips::unsigned_cast(memory) > std::numeric_limits<uptr_guest>::max()) [[unlikely]] {
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
								return string_utils::string_to_ll(value_str.c_str(), 0);
							}

							if (i == args.size() - 1) [[unlikely]] {
								fmt::println(stderr, "Error: No quantity following -s option");
								std::exit(1);
							}
							++i;
							return string_utils::string_to_ll(args[i], 0);
						}();

						if (memory < 0) [[unlikely]] {
							fmt::println(stderr, "Error: You cannot specify < 0 bytes of stack memory");
							std::exit(1);
						}
						if (mips::unsigned_cast(memory) > std::numeric_limits<uptr_guest>::max()) [[unlikely]] {
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
								return string_utils::string_to_ll(value_str.c_str(), 0);
							}

							if (i == args.size() - 1) [[unlikely]] {
								fmt::println(stderr, "Error: No port following ---debug option");
								std::exit(1);
							}
							++i;
							return string_utils::string_to_ll(args[i], 0);
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
					[executable_path](argument_data& __restrict, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
						if (value) [[unlikely]]
						{
							fmt::println(stderr, TSTR("Error: Unexpected value provided for `--help`: {}"), *value);
							// std::exit(1);
						}

						print_help(executable_path);
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
					{TSTR("--sxs")},
					TSTR("Executes the interpreter and a backend side-by-side and errors upon a state mismatch. [cannot be combined with --ticks]"),
					[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
						bool b_value = true;

						if (const auto bool_value = parse_value_bool(value))
						{
							b_value = *bool_value;
						}

						argument_data.side_by_side = b_value;
					}
				},
				{
					{TSTR("--strict-noncoherence")},
					TSTR("Toggles instructions being read from shadow memory, simulating non-coherent instruction caches. (deprecated)"),
					[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
						bool b_value = true;

						if (const auto bool_value = parse_value_bool(value))
						{
							b_value = *bool_value;
						}

						if (b_value)
						{
							argument_data.self_modifying_code = mips::system::options::policy::strict;
						}
					}
				},
				{
					{TSTR("--self-modifying-code"), TSTR("--smc")},
					TSTR("Sets the support level for self-modifying code."),
					[](argument_data& __restrict argument_data, const std::span<const tchar*>, std::size_t& __restrict, std::optional<tstring_view>&& value) {
						mips::system::options::policy result = mips::system::options::policy::undefined;

						if (string_utils::insensitive_equals(*value, TSTR("undefined")))
						{
							result = mips::system::options::policy::undefined;
						}
						else if (const auto bool_value = parse_value_bool(value))
						{
							result = (*bool_value) ? mips::system::options::policy::enabled : mips::system::options::policy::disabled;
						}

						argument_data.self_modifying_code = result;
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
					{TSTR("--llsc")},
					TSTR("Specifies which LLSC behavior to use [fine, coarse, none] [default: fine]."),
					[](argument_data& __restrict argument_data, const std::span<const tchar*> args, std::size_t& __restrict i, std::optional<tstring_view>&& value) {
						const auto llsc_v = [&] {
							if (value)
							{
								return value.value();
							}

							if (i == args.size() - 1) [[unlikely]] {
								fmt::println(stderr, "Error: No LLSC behavior following --llsc option");
								std::exit(1);
							}
							++i;
							return tstring_view{args[i]};
						}();
						if (string_utils::insensitive_equals(llsc_v, TSTR("fine"))) {
							argument_data.llsc_type = mips::llsc::fine;
						}
						else if (string_utils::insensitive_equals(llsc_v, TSTR("coarse"))) {
							argument_data.llsc_type = mips::llsc::coarse;
						}
						else if (string_utils::insensitive_equals(llsc_v, TSTR("none"))) {
							argument_data.llsc_type = mips::llsc::none;
						}
						else [[unlikely]] {
							fmt::println(stderr, TSTR("Error: The provided LLSC behavior (\'{}\') is not a valid LLSC behavior"), llsc_v);
							std::exit(1);
						}
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
						if (string_utils::insensitive_equals(mmu_v, TSTR("emulated"))) {
							argument_data.mmu_type = mips::mmu::emulated;
						}
						else if (string_utils::insensitive_equals(mmu_v, TSTR("none"))) {
							argument_data.mmu_type = mips::mmu::none;
						}
						else if (string_utils::insensitive_equals(mmu_v, TSTR("host"))) {
							argument_data.mmu_type = mips::mmu::host;
						}
						else [[unlikely]] {
							fmt::println(stderr, TSTR("Error: The provided MMU (\'{}\') is not a valid MMU type"), mmu_v);
							std::exit(1);
						}
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
							string_utils::insensitive_equals(jit_nv, TSTR("none")) ||
							(bool_value && !*bool_value)
						) {
							argument_data.jit = mips::JitType::None;
						}
						else if (
							string_utils::insensitive_equals(jit_nv, TSTR("jit")) ||
							string_utils::insensitive_equals(jit_nv, TSTR("default")) ||
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
								return string_utils::string_to_ull(value_str.c_str(), 0);
							}

							if (i == args.size() - 1) [[unlikely]] {
								fmt::println(stderr, "Error: No quantity following --ticks option");
								std::exit(1);
							}
							++i;
							return string_utils::string_to_ull(args[i], 0);
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
	}

	_nothrow void print_version() noexcept
	{
		fmt::println(
			"Digital Carbide DCMIPSr6E Emulator {}.{}.{} (" __DATE__ " " __TIME__ ")",
			version.major,
			version.minor,
			version.build
		);
	}

#ifndef EMSCRIPTEN
	static _nothrow void print_help(const tzstring_view executable_path) noexcept {
		print_version();
		fmt::println(
			TSTR("OVERVIEW: Emulator for MIPSr6 binaries\n\n")
			TSTR("USAGE: %s [options] <binary>\n\n")
			TSTR("OPTIONS:"),
			executable_path.c_str()
		);
		for (const auto& option : get_options(executable_path)) {
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

		tzstring_view executable_path { args[0] };
		if (executable_path.empty()) [[unlikely]]
		{
#if _WIN32
			executable_path = TSTR("vemips.exe");
#else
			executable_path = TSTR("vemips");
#endif
		}

		{
			usize current_index = executable_path.length() - 1;
			while (
				current_index != 0 &&
#if _WIN32
				executable_path[current_index] != TSTR('\\') &&
#endif
				executable_path[current_index] != TSTR('/')
			)
			{
				--current_index;
			}

			if (current_index != 0)
			{
				executable_path = executable_path.substr(current_index + 1);
			}
		}

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
			for (const auto& option : get_options(executable_path)) {
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

		// TODO : adjust warnings to apply to self-modifying code state
#if 0
		if (data.mmu_type != mips::mmu::emulated && !data.instruction_cache && data.jit != mips::JitType::None) [[unlikely]] {
			fmt::println(stderr, "--no-mmu requires --icache when being used with a JIT.");
			return std::unexpected(1);
		}

		if (data.mmu_type != mips::mmu::emulated && !data.instruction_cache && data.jit == mips::JitType::None) [[unlikely]] {
			fmt::println(stderr, "warning: --no-mmu requires --icache to achieve behavioral parity with JIT.");
		}
#endif

		print_version();

		const auto bool_to_str = [](const bool value) {
			return value ? "enabled" : "disabled";	
		};

		// print options
		fmt::println("Options:");
		fmt::println("\t{} bytes memory", data.available_memory);
		fmt::println("\t{} bytes reserved for stack", data.stack_memory);
		fmt::println("\tBackend: %s",
			[jit = data.jit] {
				switch (jit) {
				case mips::JitType::None:
					return "interpret";
				case mips::JitType::Jit:
					return "chunking jit 0";
				}

				xunreachable("invalid JIT type");
			}()
		);
		fmt::println("\tROX (Read-only Executable) mode {}", bool_to_str(data.use_rox));
		fmt::println("\tMemory Management Unit: %s",
			[mmu_type = data.mmu_type] {
				switch (mmu_type) {
				case mips::mmu::emulated:
					return "emulated";
				case mips::mmu::none:
					return "none";
				case mips::mmu::host:
					return "host";
				}

				xunreachable("invalid MMU type");
			}()
		);
		fmt::println("\tLLSC Behavior: %s",
			[llsc_type = data.llsc_type] {
				switch (llsc_type) {
				case mips::llsc::fine:
					return "fine";
				case mips::llsc::coarse:
					return "coarse";
				case mips::llsc::none:
					return "host";
				}

				xunreachable("invalid LLSC behavior");
			}()
		);
		fmt::println("\tCollect Statistics {}", bool_to_str(data.collect_statistics));
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
			fmt::println(stderr, TSTR("Could not read binary \"{}\""), data.binary_file);
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
