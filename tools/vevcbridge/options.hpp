#pragma once

#include <string>
#include <span>
#include "fileutils/fileutils.hpp"

using std::string;

namespace buildcarbide {
	struct options final {
		const string project_file;
		const string configuration;
		const string platform;
		const string command;
		const string out_file;
		const string sdk_path;

		options(string &&_project_file, string &&_configuration, string &&_platform, string &&_command, string &&_out_file, string &&_sdk_path) : 
			project_file(std::move(_project_file)),
			configuration(std::move(_configuration)),
			platform(std::move(_platform)),
			command(std::move(_command)),
			out_file(std::move(_out_file)),
			sdk_path(std::move(_sdk_path))
		{
		}

		static options build(std::span<const char*> args);
	};
}
