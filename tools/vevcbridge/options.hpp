#pragma once

#include <string>
#include <span>

using std::string;

namespace buildcarbide {
	struct options final {
		const string project_file;
		const string configuration;
		const string platform;
		const string command;
		const string out_file;
		const string sdk_path;

		options(string &&project_file, string &&configuration, string &&platform, string &&command, string &&out_file, string &&sdk_path) : 
			project_file(std::move(project_file)),
			configuration(std::move(configuration)),
			platform(std::move(platform)),
			command(std::move(command)),
			out_file(std::move(out_file)),
			sdk_path(std::move(sdk_path))
		{
		}

		static options build(std::span<const char*> args);
	};
}
