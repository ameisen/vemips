#include <string_view>
#include <string>
#include <vector>
#include <format>

#include <Windows.h>
#include <ShlObj.h>
#include <direct.h>

#include "tinyxml2/tinyxml2.h"

#include "buildcarbide.hpp"
#include "project/project.hpp"
#include "fileutils/fileutils.hpp"
#include "options.hpp"
#include "build/build.hpp"
#include "configuration.hpp"

using std::string;
using std::string_view;
using namespace std::literals::string_view_literals;

using namespace buildcarbide;

namespace buildcarbide {
	static constexpr void replace(string& __restrict str, const string& __restrict from, const string& __restrict to) {
		const size_t start_pos = str.find(from);
		if (start_pos == string::npos) {
			return;
		}
		str.replace(start_pos, from.length(), to);
	}

	static constexpr void replace(string& __restrict str, const char * __restrict from, const string& __restrict to) {
		const size_t start_pos = str.find(from);
		if (start_pos == string::npos) {
			return;
		}
		str.replace(start_pos, strlen(from), to);
	}

	template <uintptr_t InvalidValue>
	class system_handle final {
	public:
		static constexpr uintptr_t invalid = uintptr_t(InvalidValue);

	private:
		HANDLE handle_ = HANDLE(invalid);

	public:
		system_handle(const HANDLE handle) : handle_(handle) {
		}
		
		system_handle(const system_handle&) = delete;

		system_handle(system_handle&& handle) noexcept : handle_(handle.handle_) {
			handle.handle_ = handle.invalid;
		}

		~system_handle() {
			if (handle_ != HANDLE(invalid)) {  // NOLINT(performance-no-int-to-ptr)
				CloseHandle(handle_);
			}
		}

		system_handle& operator = (const HANDLE handle) {
			if (handle_ != HANDLE(invalid)) {
				CloseHandle(handle_);
			}
			handle_ = handle;
			return *this;
		}

		system_handle& operator = (const system_handle&) = delete;

		system_handle& operator = (system_handle&& handle) noexcept {
			auto sub_handle = handle.handle_;
			handle.handle_ = handle.invalid;
			return this = sub_handle;
		}

		operator HANDLE() const { return handle_; }

		operator bool() const { return handle_ != HANDLE(invalid); }  // NOLINT(performance-no-int-to-ptr)
	};

	using file_handle = system_handle<uintptr_t(/*INVALID_HANDLE_VALUE*/-1)>;

	static uint64_t get_file_time(const string& __restrict filename, const uint64_t def = uint64_t(-1)) {
		const file_handle file_handle = CreateFile(
			filename.c_str(), 
			GENERIC_READ, 
			FILE_SHARE_READ, 
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
		if (!file_handle) {
			return def;
		}

		FILETIME file_time;
		if (GetFileTime(file_handle, nullptr, nullptr, &file_time) == 0) {
			return def;
		}

		const ULARGE_INTEGER file_time_union = {
			{
				.LowPart = file_time.dwLowDateTime,
				.HighPart = file_time.dwHighDateTime
			}
		};
		return file_time_union.QuadPart;
	}

	static string get_file_name(const string& __restrict path) {
		const size_t last_index = path.rfind('/');
		if (last_index == string::npos) {
			return path;
		}

		return path.substr(last_index + 1);
	}

	static string merge_arg_tokens(const std::span<string> tokens) {
		size_t length = 0;
		for (const auto& token : tokens) {
			length += token.length() + 1;
		}

		string result;
		result.reserve(length);
		for (auto&& token : tokens) {
			result += token;
			result += ' ';
		}

		return result;
	}

	static string get_compile_args(
		const options& __restrict options,
		const string& __restrict filepath,
		const string& __restrict configuration,
		const string& __restrict intermediate,
		string& __restrict object_file
	) {
		// TODO rework this.

		std::vector<string> cfg = {
			"mipsc",
			"-ffunction-sections",
			"-fdata-sections",
			"-mcompact-branches=always",
			"-fasynchronous-unwind-tables",
			"-funwind-tables",
			"-fexceptions",
			"-fcxx-exceptions",
			"-mips32r6"
		};


		if (configuration == "Debug") {
			static constexpr const char* const args[] = {
				"-O0",
				"-glldb"
			};
			cfg.append_range(args);
		}
		else if (configuration == "Release") {
			static constexpr const char* const args[] = {
				"-mno-check-zero-division",
				"-O3",
				"-Os"
			};
			cfg.append_range(args);
		}

		const string incxx = fileutils::build_path(options.sdk_path, "incxx");
		const string inc = fileutils::build_path(options.sdk_path, "inc");

		cfg.push_back(std::format("-I{}", incxx));
		cfg.push_back(std::format("-I{}", inc));
		cfg.emplace_back("-c");

		cfg.push_back(filepath);

		const string filename = get_file_name(filepath);
		object_file = fileutils::build_path(intermediate, (filename + ".o"));

		cfg.emplace_back("-o");
		cfg.push_back(object_file);

		return merge_arg_tokens(cfg);
	}

	static string get_link_args(
		const options& __restrict options,
		const std::vector<string>& __restrict object_files,
		const string& __restrict filepath,
		const string& __restrict configuration
	) {
		// TODO rework this.

		std::vector<string> cfg = {
			"mipsld",
			"-znorelro",
			"--eh-frame-hdr"
		};

		if (configuration == "Debug") {
			static constexpr const char* const args[] = {
				"--discard-none"
			};
			cfg.append_range(args);
		}
		else if (configuration == "Release") {
			static constexpr const char* const args[] = {
				"--discard-all",
				"--gc-sections",
				"--icf=all",
				"--strip-all"
			};
			cfg.append_range(args);
		}

		{
			static constexpr const char* const args[] = {
				"-lc",
				"-lc++",
				"-lc++abi",
				"-lllvmunwind"
			};
			cfg.append_range(args);
		}

		const string libs = fileutils::build_path(options.sdk_path, "lib");
		cfg.push_back(std::format("-L{}", libs));

		cfg.append_range(object_files);

		cfg.emplace_back("-o");
		cfg.push_back(filepath);

		return merge_arg_tokens(cfg);
	}

	static void validate_options(const options& __restrict options) {
		for (
			const std::pair<const string&, const char * __restrict> checks[] = {
			 { options.project_file, "No project file provided" },
			 { options.configuration, "No configuration provided" },
			 { options.platform, "No platform provided" },
			 { options.command, "No command provided" },
			 { options.out_file, "No output file provided" },
			};
			const auto& [value, error_string] : checks
		) {
			if (value.empty()) [[unlikely]] {
				fail<1>(error_string);
			}
		}
	}
}

int main(int argc, const char **argv) {
	if (argc != 6) [[unlikely]] {
		fail<1>("Incorrect number of arguments\n");
	}

	options build_options = options::build(std::span(argv, argc));
	validate_options(build_options);

	const string & intermediate_location = build_options.out_file;

	const string base_dir = buildcarbide::fileutils::get_base_path(build_options.project_file);

	_chdir(base_dir.c_str());

	string out_file = build_options.out_file;
	if (out_file.length() < 3 || (out_file[2] != '/' && out_file[2] != '\\')) {
		out_file = fileutils::build_path(base_dir, out_file);
	}

	configuration config = {
		.target = build_options.platform,
		.config = build_options.configuration,
		.base_path = fileutils::fix_up(base_dir),
		.intermediate_dir = fileutils::build_path(base_dir, intermediate_location)
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
			const bool quote_start = *str == '\'' || *str == '\"';
			if (quote_start) {
				++str;
			}

			string result;

			if (quote_start) {
				while (*str != '\'' && *str != '\"') {
					result += *str++;
				}
			}
			else {
				while (!isspace(*str)) {
					result += *str++;
				}
			}

			return result;
		};
		string format = extract_substring(condition_text);

		++condition_text;
		if (condition_text[0] != '=' || condition_text[1] != '=') {
			fail<-1>("Syntax error in Condition string '%s'\n", condition_text_base);
		}
		condition_text += 2;

		const string values = extract_substring(condition_text);

		replace(format, "$(Configuration)", build_options.configuration);
		replace(format, "$(Platform)", build_options.platform);

		return values == format;
	};

	try
	{
		std::vector<string> cl_compile;
		std::vector<string> cl_include;

		tinyxml2::XMLDocument doc;
		doc.LoadFile(build_options.project_file.c_str());
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
					cl_compile.push_back(fileutils::build_path(base_dir, file_name));
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
					cl_include.push_back(fileutils::build_path(base_dir, file_name));
				}
			}

			item_group = item_group->NextSiblingElement("ItemGroup");
		}

		struct build_file final {
			string filename;
			uint64_t mod_time;
		};

		std::vector<build_file> build_files;
		bool do_clean = false;
		const uint64_t out_file_build_time = get_file_time(out_file, 0);

		// If we are rebuilding, they are all built.
		// In this situation, we also clean.
		if (build_options.command == "rebuild") {
			for (const string &file : cl_compile) {
				build_files.push_back({ file, uint64_t(-1) });
			}
			do_clean = true;
		}
		else if (build_options.command == "clean") {
			do_clean = true;
		}
		else if (build_options.command == "build") {
			// TODO we need to do dependency checks. However, for now, we will just check if the headers have changed and flag everything as dirty.
			bool rebuild_all = false;
			for (const string &file : cl_include) {
				if (get_file_time(file) > out_file_build_time) {
					rebuild_all = true;
					break;
				}
			}

			for (const string &file : cl_compile) {
				build_files.push_back({ file, rebuild_all ? uint64_t(-1) : get_file_time(file) });
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

		std::vector<string> object_files;

		// Do the build.
		for (const auto &build_file : build_files) {
			if (build_file.mod_time <= out_file_build_time) {
				continue;
			}

			string object_file;
			const string compile_args = get_compile_args(build_options, build_file.filename, build_options.configuration, intermediate_location, object_file);
			object_files.push_back(object_file);

			SHCreateDirectoryEx( 
				nullptr, 
				fileutils::get_base_path(fileutils::build_path(base_dir, object_file)).c_str(), 
				nullptr
			);

			// Do the build.
			printf("%s\n", compile_args.c_str());
			if (
				const int res = system(compile_args.c_str());
				res != 0
			) {
				fail<2>("Error compiling \'%s\'\n", build_file.filename.c_str());
			}
		}

		if (object_files.empty()) {
			return 0;
		}

		SHCreateDirectoryEx( nullptr, fileutils::get_base_path(out_file).c_str(), nullptr );

		const string link_args = get_link_args(build_options, object_files, out_file, build_options.configuration);
		printf("%s\n", link_args.c_str());
		if (
			const int res = system(link_args.c_str());
			res != 0
		) {
			fail<3>("Error linking\n");
		}
	}
	catch (...) {
		fail<4>("Failed to parser project file \'%s\'\n", build_options.project_file.c_str());
	}

	return 0;
}
