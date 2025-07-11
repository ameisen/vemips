// ReSharper disable CppClangTidyCertErr33C
#include "tablegen.pch.hpp"

#include "writers.hpp"

#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"

#include <algorithm>
#include <functional>

#include <cstdio>
#include <queue>
#include <span>
#include <optional>
#include <mutex>

#if USE_STATIC_INSTRUCTION_SEARCH
#	error TableGen requires 'USE_STATIC_INSTRUCTION_SEARCH' to be unset
#endif

namespace mips::instructions {
	extern void finish_map_build();
#if !USE_STATIC_INSTRUCTION_SEARCH
	extern StaticInitVars& GetStaticInitVars();
#endif
}

namespace vemips::tablegen {
	FILE* out_stream = stdout;
}

using namespace vemips::tablegen;

namespace {

	//TODO:
	// Traverse the map a first time and build up just a list of procinfos. We will then have the file statically allocate them,
	// so we can reference... references instead of values.
	// We also need to figure out how to actually call the procedures. They are only described in source files and the symbols won't export.
	// Will likely have to add simply-exported names that are pointers to the named procedures which we can extern.

	static _nothrow std::vector<const info_t*> populate_proc_infos(const map_t * const map) {
		std::vector<const info_t*> result;

		std::queue<std::remove_cv_t<decltype(map)>> pending;
		pending.push(map);

		while (!pending.empty()) {
			const auto current = pending.front();
			pending.pop();

			if (!current->IsMap) {
				result.push_back(&current->Info);
				continue;
			}

			if (current->Map.size() == 1 && !current->Default) {
				result.push_back(&current->Map.begin()->second->Info);
				continue;
			}

			for (auto&& sub_map : current->Map) {
				pending.push(sub_map.second);
			}

			if (current->Default) {
				pending.push(current->Default);
			}
		}

		return result;
	}

	struct instruction_compare final {
		using type = const info_t* __restrict;

		_nothrow bool operator() (const type lhs, const type rhs) const __restrict {
			return std::strcmp(lhs->Name, rhs->Name) < 0;
		}
	};

	static std::optional<options> parse_options(const std::span<char* const> args) {
		crstring file_out = nullptr;
		bool write_header = false;
		bool write_source = false;

		const auto extract_option = [&](char* __restrict arg) -> option {
			{
				crstring arg_temp = arg;
				for (char c; (c = *arg_temp) != '\0'; ++arg_temp) {
					if (c == '=' || c == ':') {
						const size_t operand_index = size_t(arg_temp - arg);
						arg[operand_index] = '\0';
						return {
							.tag = arg,
							.value = arg + operand_index + 1
						};
					}
				}
			}

			return {
				.tag = arg
			};
		};

		for (size_t i = 1; i < args.size(); ++i)
		{
			auto&& arg = extract_option(args[i]);

			switch (fnv_hash(arg.tag)) {
			case fnv_hash("--header"):
				if (arg.value) {
					fmt::print(stderr, "Argument does not take value: '{}' with '{}'", arg.tag, arg.value);
					return {};
				}
				write_header = true;
				break;
			case fnv_hash("--source"):
				if (arg.value) {
					fmt::println(stderr, "Argument does not take value: '{}' with '{}'", arg.tag, arg.value);
					return {};
				}
				write_source = true;
				break;
			case fnv_hash("--file"):
				if (!arg.value) {
					// then the next arg
					if (args.size() <= i + 1)
					{
						fmt::println(stderr, "'{}' provided but no argument passed", arg.tag);
						return {};
					}

					arg.value = args[++i];
				}

				file_out = arg.value;
				break;
			default:
				fmt::println(stderr, "Unknown Argument: '{}'", arg.tag);
				return {};
			}
		}

		return options{
			.file_out = file_out,
			.write_header = write_header,
			.write_source = write_source
		};
	}
}

int main(const int argc, char* const* const argv) {
	xassert(argc >= 0);

	auto&& options_optional = parse_options({ argv, size_t(argc) });
	if (!options_optional) {
		return -1;
	}
	const options options = options_optional.value();

	mips::instructions::finish_map_build();

	auto&& proc_infos = populate_proc_infos(&mips::instructions::GetStaticInitVars().g_LookupMap);

	std::stable_sort(proc_infos.begin(), proc_infos.end(), instruction_compare{});

	if (options.write_source) {
		vemips::tablegen::writers::source(options, proc_infos);
	}

	if (options.write_header)
	{
		vemips::tablegen::writers::header(options, proc_infos);
	}
}
