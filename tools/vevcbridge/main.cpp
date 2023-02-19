#include "tinyxml2/tinyxml2.h"

#include <string>
#include <string_view>
#include <vector>
#include <Windows.h>
#include <ShlObj.h>
#include <direct.h>
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
		size_t start_pos = str.find(from);
		if (start_pos == string::npos)
			return;
		str.replace(start_pos, from.length(), to);
	}

	static constexpr void replace(string& __restrict str, const char * __restrict from, const string& __restrict to) {
		size_t start_pos = str.find(from);
		if (start_pos == string::npos)
			return;
		str.replace(start_pos, strlen(from), to);
	}

	template <uintptr_t invalid_value>
	class Handle {
	public:
		const HANDLE invalid = HANDLE(invalid_value);

	private:
		const HANDLE handle_ = invalid;

	public:
		Handle(HANDLE handle) : handle_(handle) {
		}
		
		Handle(const Handle&) = delete;

		Handle(Handle&& handle) : handle_(handle.handle_) {
			handle.handle_ = handle.invalid;
		}

		~Handle() {
			if (handle_ != invalid) {
				CloseHandle(handle_);
			}
		}

		Handle& operator = (HANDLE handle) {
			if (handle_ != invalid) {
				CloseHandle(handle_);
			}
			handle_ = handle;
		}

		Handle& operator = (const Handle&) = delete;

		Handle& operator = (Handle&& handle) {
			auto sub_handle = handle.handle_;
			handle.handle_ = handle.invalid;
			return this = sub_handle;
		}

		operator HANDLE() const { return handle_; }

		operator bool() const { return handle_ != invalid; }
	};

	using FileHandle = Handle<uintptr_t(/*INVALID_HANDLE_VALUE*/-1)>;

	static uint64_t get_file_time(const string& __restrict filename, uint64_t def = uint64_t(-1))
	{
		FILETIME file_time;
		FileHandle hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (!hFile)
		{
			return def;
		}

		if (GetFileTime(hFile, nullptr, nullptr, &file_time) == 0)
		{
			return def;
		}

		ULARGE_INTEGER ints = {
			{
				.LowPart = file_time.dwLowDateTime,
				.HighPart = file_time.dwHighDateTime
			}
		};
		return ints.QuadPart;
	}

	static string get_file_name(const string& __restrict path)
	{
		size_t last_index = path.rfind('/');
		if (last_index == string::npos) {
			return path;
		}

		return path.substr(last_index + 1);
	}

	static string get_compile_args(const options& __restrict options, const string& __restrict filepath, const string& __restrict configuration, const string& __restrict intermediate, string& __restrict object_file)
	{
		const string filename = get_file_name(filepath);
		const string filedir = buildcarbide::fileutils::get_base_path(filepath);

		// TODO rework this.

		string cfg;

		string incxx = buildcarbide::fileutils::build_path(options.sdk_path, "incxx");
		string inc = buildcarbide::fileutils::build_path(options.sdk_path, "inc");

		if (configuration == "Debug")
		{
			cfg +=
				"mipsc -ffunction-sections -fdata-sections -mcompact-branches=always -fasynchronous-unwind-tables -funwind-tables "
				"-fexceptions -fcxx-exceptions -mips32r6 -O0 -glldb ";
		}
		else if (configuration == "Release")
		{
			cfg +=
				"mipsc -ffunction-sections -fdata-sections -mno-check-zero-division -mcompact-branches=always -fasynchronous-unwind-tables -funwind-tables "
				"-fexceptions -fcxx-exceptions -mips32r6 -O3 -Os ";
		}

		cfg += string("-I") + incxx + " ";
		cfg += string("-I") + inc + " ";
		cfg += "-c ";

		object_file = buildcarbide::fileutils::build_path(intermediate, (filename + ".o"));

		cfg += filepath + " ";
		cfg += "-o ";
		cfg += object_file;
		return cfg;
	}

	static string get_link_args(const options& __restrict options, const std::vector<string>& __restrict objectfiles, const string& __restrict filepath, const string& __restrict configuration)
	{
		const string filename = get_file_name(filepath);
		const string filedir = buildcarbide::fileutils::get_base_path(filepath);

		// lld --discard-all -znorelro --gc-sections --icf=all --strip-all --eh-frame-hdr -Llib -lc -lc++ -lc++abi -lllvmunwind

		// TODO rework this.

		string libs = buildcarbide::fileutils::build_path(options.sdk_path, "lib");

		string cfg;

		if (configuration == "Debug")
		{
			cfg += "mipsld --discard-none -znorelro --eh-frame-hdr -lc -lc++ -lc++abi -lllvmunwind ";
		}
		else if (configuration == "Release")
		{
			cfg += "mipsld --discard-all -znorelro --gc-sections --icf=all --strip-all --eh-frame-hdr -lc -lc++ -lc++abi -lllvmunwind ";
		}
		cfg += string("-L") + libs + " ";

		for (const string& objectfile : objectfiles)
		{
			cfg += objectfile;
			cfg += " ";
		}

		cfg += "-o ";
		cfg += filepath;
		return cfg;
	}
}

int main(int argc, const char **argv)
{
	if (argc != 6)
	{
		fprintf(stderr, "Incorrect number of arguments\n");
		return 1;
	}

	options build_options = options::build(std::span(argv, argc));

	if (build_options.project_file.empty()) {
		fprintf(stderr, "No project file provided");
		exit(1);
	}
	if (build_options.configuration.empty()) {
		fprintf(stderr, "No configuration provided");
		exit(1);
	}
	if (build_options.platform.empty()) {
		fprintf(stderr, "No platform provided");
		exit(1);
	}
	if (build_options.command.empty()) {
		fprintf(stderr, "No command provided");
		exit(1);
	}
	if (build_options.out_file.empty()) {
		fprintf(stderr, "No output file provided");
		exit(1);
	}

	string intermediate_location = build_options.out_file;

	string OutfileStr = build_options.out_file;

	const string base_dir = buildcarbide::fileutils::get_base_path(build_options.project_file);

	_chdir(base_dir.c_str());

	if (OutfileStr.length() < 3 || (OutfileStr[2] != '/' && OutfileStr[2] != '\\'))
	{
		OutfileStr = buildcarbide::fileutils::build_path(base_dir, OutfileStr);
	}

	buildcarbide::configuration config;
	config.target = build_options.platform;
	config.config = build_options.configuration;
	config.base_path = buildcarbide::fileutils::fixup(base_dir);
	config.intermediate_dir = buildcarbide::fileutils::build_path(base_dir, intermediate_location);

	auto *proj = buildcarbide::get_project(build_options.project_file);
	buildcarbide::build builder(config, *proj);
	delete proj;

	const auto IsExcluded = [&](tinyxml2::XMLElement * __restrict element) -> bool
	{
		if (!element)
		{
			return false;
		}

		auto true_false = element->FirstChild();
		if (!true_false)
		{
			return false;
		}

		auto true_false_text = true_false->ToText();
		if (!true_false_text)
		{
			return false;
		}

		if (strcmp(true_false_text->Value(), "false") == 0)
		{
			return false;
		}

		const char *condition_text = element->Attribute("Condition");
		if (!condition_text)
		{
			return false;
		}

		// parse the condition format, which looks like this:
		// $(Configuration)|$(Platform)'=='Release|Win32'
		string format;
		string values;

		// this is not a stable solution at all.
		if (*condition_text == '\'' || *condition_text == '\"')
		{
			++condition_text;
		}

		while (*condition_text != '\'' && *condition_text != '\"')
		{
			format += *condition_text++;
		}

		++condition_text;

		while (*condition_text == '=')
		{
			++condition_text;
		}

		if (*condition_text == '\'' || *condition_text == '\"')
		{
			++condition_text;
		}

		while (*condition_text != '\'' && *condition_text != '\"')
		{
			values += *condition_text++;
		}

		string test = format;
		replace(test, "$(Configuration)", build_options.configuration);
		replace(test, "$(Platform)", build_options.platform);

		return values == test;
	};

	try
	{
		std::vector<string> ClCompile;
		std::vector<string> ClInclude;

		tinyxml2::XMLDocument doc;
		doc.LoadFile(build_options.project_file.c_str());
		if (doc.Error())
		{
			throw 0;
		}

		const auto project = doc.FirstChildElement("Project");
		if (!project)
		{
			throw 0;
		}

		auto item_group = project->FirstChildElement("ItemGroup");
		if (!item_group)
		{
			throw 0;
		}
		while (item_group)
		{
			// Iterate over all item groups.
			// source files
			{
				auto compile_element = item_group->FirstChildElement("ClCompile");
				for (; compile_element; compile_element = compile_element->NextSiblingElement("ClCompile"))
				{
					auto exclusion_element = compile_element->FirstChildElement("ExcludedFromBuild");
					if (IsExcluded(exclusion_element))
					{
						continue;
					}

					const char *file_name = compile_element->Attribute("Include");
					if (!file_name)
					{
						continue;
					}
					ClCompile.push_back(buildcarbide::fileutils::build_path(base_dir, file_name));
				}
			}

			// header files
			{
				auto compile_element = item_group->FirstChildElement("ClInclude");
				for (; compile_element; compile_element = compile_element->NextSiblingElement("ClInclude"))
				{
					auto exclusion_element = compile_element->FirstChildElement("ExcludedFromBuild");
					if (IsExcluded(exclusion_element))
					{
						continue;
					}

					const char *file_name = compile_element->Attribute("Include");
					if (!file_name)
					{
						continue;
					}
					ClInclude.push_back(buildcarbide::fileutils::build_path(base_dir, file_name));
				}
			}

			item_group = item_group->NextSiblingElement("ItemGroup");
		}

		struct BuildFile
		{
			string filename;
			uint64_t modtime;
		};

		std::vector<BuildFile> FilesToBuild;

		bool DoClean = false;

		const uint64_t OutFileBuildTime = get_file_time(OutfileStr, 0);

		// If we are rebuilding, they are all built.
		// In this situation, we also clean.
		if (build_options.command == "rebuild")
		{
			for (const string &file : ClCompile)
			{
				FilesToBuild.push_back({file, uint64_t(-1)});
			}
			DoClean = true;
		}
		else if (build_options.command == "clean")
		{
			DoClean = true;
		}
		else if (build_options.command == "build")
		{
			// TODO we need to do dependency checks. However, for now, we will just check if the headers have changed and flag everything as dirty.
			bool RebuildAll = false;
			for (const string &file : ClInclude)
			{
				if (get_file_time(file) > OutFileBuildTime)
				{
					RebuildAll = true;
					break;
				}
			}

			for (const string &file : ClCompile)
			{
				FilesToBuild.push_back({ file, RebuildAll ? uint64_t(-1) : get_file_time(file) });
			}
		}

		if (DoClean)
		{
			SHFILEOPSTRUCT fileStruct;
			memset(&fileStruct, 0, sizeof(fileStruct));
			fileStruct.wFunc = FO_DELETE;
			std::vector<char> InterLoc(intermediate_location.length() + 2, '\0');
			memcpy(InterLoc.data(), intermediate_location.data(), intermediate_location.length());
			fileStruct.pFrom = InterLoc.data();
			fileStruct.fFlags = FOF_NO_UI;

			SHFileOperation(&fileStruct);

			std::vector<char> OutLoc(OutfileStr.length() + 2, '\0');
			memcpy(OutLoc.data(), OutfileStr.data(), OutfileStr.length());
			fileStruct.pFrom = OutLoc.data();

			SHFileOperation(&fileStruct);
		}

		if (FilesToBuild.empty())
		{
			// just return
			return 0;
		}

		std::vector<string> object_files;

		// Do the build.
		for (const auto &build_file : FilesToBuild)
		{
			if (build_file.modtime <= OutFileBuildTime)
			{
				continue;
			}

			string object_file;
			const string compile_args = get_compile_args(build_options, build_file.filename, build_options.configuration, intermediate_location, object_file);
			object_files.push_back(object_file);

			SHCreateDirectoryEx( nullptr, buildcarbide::fileutils::get_base_path(buildcarbide::fileutils::build_path(base_dir, object_file)).c_str(), nullptr );

			// Do the build.
			printf("%s\n", compile_args.c_str());
			int res = system(compile_args.c_str());
			if (res != 0)
			{
				fprintf(stderr, "Error compiling \'%s\'\n", build_file.filename.c_str());
				return 1;
			}
		}

		if (object_files.empty())
		{
			return 0;
		}

		SHCreateDirectoryEx( nullptr, buildcarbide::fileutils::get_base_path(OutfileStr).c_str(), nullptr );

		const string link_args = get_link_args(build_options, object_files, OutfileStr, build_options.configuration);
		printf("%s\n", link_args.c_str());
		int res = system(link_args.c_str());
		if (res != 0)
		{
			fprintf(stderr, "Error linking\n");
			return 1;
		}

	}
	catch (...)
	{
		fprintf(stderr, "Failed to parser project file \'%s\'\n", build_options.project_file.c_str());
		return 1;
	}

	return 0;
}
