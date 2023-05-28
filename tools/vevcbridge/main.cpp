#include "buildcarbide.hpp"

#include <codecvt>
#include <vector>

#include "configuration.hpp"
#include "options.hpp"
#include "build/build.hpp"
#include "file_utils/file_utils.hpp"
#include "project/project.hpp"
#include "toolchain/clang.hpp"

#define PLATFORM_HEADER_WITH_SHELL 1
// ReSharper disable once IdentifierTypo
#define PLATFORM_HEADER_WITH_SHLOBJ 1
#include "platform/platform_headers.hpp"
#undef PLATFORM_HEADER_WITH_SHLOBJ
#undef PLATFORM_HEADER_WITH_SHELL

#include "tinyxml2/tinyxml2.h"

using std::wstring;
using std::wstring_view;
using namespace std::literals::string_view_literals;

using namespace buildcarbide;

namespace buildcarbide {
	namespace {
		constexpr void replace(wstring& __restrict str, const wstring_view from, const wstring_view to) noexcept {
			const size_t start_pos = str.find(from);
			if (start_pos == wstring::npos) {
				return;
			}
			str.replace(start_pos, from.length(), to);
		}

		void validate_options(const options& __restrict options) {
			bool failure = false;
			for (
				const std::pair<const wstring& __restrict, const wchar_t * __restrict> checks[] = {
					{ options.project_file, L"No project file provided" },
					{ options.configuration, L"No configuration provided" },
					{ options.platform, L"No platform provided" },
					{ options.command, L"No command provided" },
					{ options.out_file, L"No output file provided" },
				};
				const auto& [value, error_string] : checks
			) {
				if (value.empty()) [[unlikely]] {
					errorf(error_string);
					failure = true;
				}
			}

			if (failure) [[unlikely]] {
				std::exit(-1);  // NOLINT(concurrency-mt-unsafe)
			}
		}
	}
}

// ReSharper disable once IdentifierTypo
int wmain(int argc, const wchar_t **argv) {
	if (argc < 1) [[unlikely]] {
		fail<1>(L"Incorrect number of arguments");
	}

	options build_options = options::build(std::span(argv + 1, argc - 1));
	validate_options(build_options);

	const wstring & intermediate_location = build_options.out_file;

	const wstring base_dir = buildcarbide::file_utils::get_base_path(build_options.project_file);

	_wchdir(base_dir.c_str());

	wstring out_file = build_options.out_file;
	if (out_file.length() < 3 || (out_file[2] != '/' && out_file[2] != '\\')) {
		out_file = file_utils::build_path(base_dir, out_file);
	}

	configuration config = {
		.target = build_options.platform,
		.config = build_options.configuration,
		.base_path = file_utils::fix_up(base_dir),
		.intermediate_dir = file_utils::build_path(base_dir, intermediate_location)
	};

	build builder(
		config,
		[&]() { return get_project(build_options.project_file); }
	);

	const auto is_excluded = [&](const tinyxml2::XMLElement * __restrict element) -> bool {
		if (!element) {
			return false;
		}

		const auto true_false = element->FirstChild();
		if (!true_false) {
			return false;
		}

		const auto true_false_text = true_false->ToText();
		if (!true_false_text) {
			return false;
		}

		if (strcmp(true_false_text->Value(), "false") == 0) {
			return false;
		}

		const char *condition_text = element->Attribute("Condition");
		if (!condition_text) {
			return false;
		}
		const char* const condition_text_base = condition_text;

		// parse the condition format, which looks like this:
		// '$(Configuration)|$(Platform)'=='Release|Win32'

		// this is not a stable solution at all.
		const auto extract_substring = [](const char*& __restrict str) {
			const char ref_char = *str;
			const bool is_quote = ref_char == '\'' || ref_char == '\"';
			if (is_quote) {
				++str;
			}

			std::string result;

			if (is_quote) {
				while (*str != ref_char) {
					if (*str == '\0') [[unlikely]] {
						throw std::exception("Malformed Condition");
					}
					result += *str++;
				}
			}
			else {
				while (!isspace(*str)) {
					if (*str == '\0') [[unlikely]] {
						throw std::exception("Malformed Condition");
					}
					result += *str++;
				}
			}

			return result;
		};
		std::wstring format = to_wstring(extract_substring(condition_text).c_str());

		++condition_text;
		if (condition_text[0] == '\0' || (condition_text[0] != '=' || condition_text[1] != '=')) [[unlikely]] {
			fail<-1>("Syntax error in Condition string '{}'", condition_text_base);
		}
		condition_text += 2;

		const std::wstring values = to_wstring(extract_substring(condition_text));

		replace(format, L"$(Configuration)", build_options.configuration);
		replace(format, L"$(Platform)", build_options.platform);

		return values == format;
	};

	try
	{
		std::vector<wstring> cl_compile;
		std::vector<wstring> cl_include;

		tinyxml2::XMLDocument doc;
		doc.LoadFile(to_string(build_options.project_file).c_str());
		if (doc.Error()) {
			throw 0;
		}

		const auto project = doc.FirstChildElement("Project");
		if (!project) {
			throw 0;
		}

		auto item_group = project->FirstChildElement("ItemGroup");
		if (!item_group) {
			throw 0;
		}

		while (item_group) {
			// Iterate over all item groups.
			// source files
			{
				for (
					auto compile_element = item_group->FirstChildElement("ClCompile");
					compile_element;
					compile_element = compile_element->NextSiblingElement("ClCompile")
				) {
					if (
						const auto exclusion_element = compile_element->FirstChildElement("ExcludedFromBuild");
						is_excluded(exclusion_element)
					) {
						continue;
					}

					const char * const file_name = compile_element->Attribute("Include");
					if (!file_name) {
						continue;
					}
					cl_compile.push_back(file_utils::build_path(base_dir, to_wstring(file_name)));
				}
			}

			// header files
			{
				for (
					auto compile_element = item_group->FirstChildElement("ClInclude");
					compile_element;
					compile_element = compile_element->NextSiblingElement("ClInclude")
				) {
					if (
						const auto exclusion_element = compile_element->FirstChildElement("ExcludedFromBuild");
						is_excluded(exclusion_element)
					) {
						continue;
					}

					const char * const file_name = compile_element->Attribute("Include");
					if (!file_name) {
						continue;
					}
					cl_include.push_back(file_utils::build_path(base_dir, to_wstring(file_name)));
				}
			}

			item_group = item_group->NextSiblingElement("ItemGroup");
		}

		struct build_file final {
			wstring filename;
			uint64_t mod_time;
		};

		std::vector<build_file> build_files;
		bool do_clean = false;
		const uint64_t out_file_build_time = file_utils::get_file_time(file_utils::filetime::modified, out_file).value_or(0);

		// If we are rebuilding, they are all built.
		// In this situation, we also clean.
		if (build_options.command == L"rebuild") {
			for (const wstring &file : cl_compile) {
				build_files.push_back({ file, uint64_t(-1) });
			}
			do_clean = true;
		}
		else if (build_options.command == L"clean") {
			do_clean = true;
		}
		else if (build_options.command == L"build") {
			// TODO we need to do dependency checks. However, for now, we will just check if the headers have changed and flag everything as dirty.
			bool rebuild_all = false;
			for (const wstring &file : cl_include) {
				if (file_utils::get_file_time(file_utils::filetime::modified, file).value_or(0) > out_file_build_time) {
					rebuild_all = true;
					break;
				}
			}

			for (const wstring &file : cl_compile) {
				build_files.push_back({ file, rebuild_all ? uint64(-1) : file_utils::get_file_time(file_utils::filetime::modified, file).value_or(uint64(-1)) });
			}
		}

		if (do_clean) {
			SHFILEOPSTRUCT shell_file = {
				.hwnd = nullptr,
				.wFunc = FO_DELETE,
				.pFrom = intermediate_location.data(),
				.pTo = nullptr,
				.fFlags = FOF_NO_UI,
				.fAnyOperationsAborted = FALSE,
				.hNameMappings = nullptr,
				.lpszProgressTitle = nullptr
			};

			SHFileOperation(&shell_file);

			shell_file.pFrom = out_file.data();

			SHFileOperation(&shell_file);
		}

		if (build_files.empty()) {
			// just return
			return 0;
		}

		std::vector<wstring> object_files;

		toolchain::clang toolchain;

		// Do the build.
		for (const auto &file : build_files) {
			if (file.mod_time <= out_file_build_time) {
				continue;
			}

			wstring object_file;
			const wstring compile_args = toolchain.get_compile_args(build_options, file.filename, build_options.configuration, intermediate_location, object_file);
			object_files.push_back(object_file);

			SHCreateDirectoryEx( 
				nullptr, 
				file_utils::get_base_path(file_utils::build_path(base_dir, object_file)).c_str(), 
				nullptr
			);

			// Do the build.
			fmt::println(L"{}", compile_args);
			_flushall();
			if (
				const int res = _wsystem(compile_args.c_str());
				res != 0
			) {
				fail<2>(L"Error compiling \'{}\'", file.filename);
			}
		}

		if (object_files.empty()) {
			return 0;
		}

		SHCreateDirectoryEx( nullptr, file_utils::get_base_path(out_file).c_str(), nullptr );

		const wstring link_args = toolchain.get_link_args(build_options, object_files, out_file, build_options.configuration);
		fmt::println(L"{}", link_args);
		_flushall();
		if (
			const int res = _wsystem(link_args.c_str());
			res != 0
		) {
			fail<3>(L"Error linking");
		}
	}
	catch (const std::exception& ex) {
		fail<4>(L"Failed to parser project file \'{}\' :: {}", build_options.project_file, ex.what());
	}
	catch (...) {
		fail<4>(L"Failed to parser project file \'{}\'", build_options.project_file);
	}

	return 0;
}
