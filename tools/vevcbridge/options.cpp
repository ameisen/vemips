#include "options.hpp"

#include <Windows.h>

#include <algorithm>
#include <cctype>
#include <unordered_map>

buildcarbide::options buildcarbide::options::build(std::span<const char*> args) {
	string project_file;
	string configuration;
	string platform;
	string command;
	string out_file;
	string sdk_path;

	std::unordered_map<string_view, string&> options_map = {
		{"configuration", configuration},
		{"platform", platform},
		{"command", command},
		{"out", out_file},
		{"sdk", sdk_path},
	};

	size_t index = 0;

	auto pop_arg = [&]() -> string_view {
		if (index >= args.size()) {
			return {};
		}
		return args[index++];
	};

	project_file = pop_arg();

	auto pop_subarg = [&](string_view &arg) -> string_view {
		size_t split_index = string::npos;
		for (char split_char : { '=', ':' }) {
			size_t index = arg.find(split_char);
			if (index != string::npos) {
				if (split_index == string::npos || index < split_index) {
					split_index = index;
				}
			}
		}

		if (split_index != string::npos) {
			auto sub_arg = arg.substr(split_index + 1);
			arg = arg.substr(0, split_index);
			return sub_arg;
		}
		else {
			return pop_arg();
		}
	};

	while (index < args.size()) {
		auto arg = pop_arg();

		auto sub_arg = pop_subarg(arg);
		string lower_arg = string(arg);

		// convert to lower case
		for (size_t i = 0; i < lower_arg.size(); ++i) {
			lower_arg[i] = std::tolower(lower_arg[i]);
		}
		auto configIter = options_map.find(lower_arg);
		if (configIter == options_map.end()) {
			fprintf(stderr, "Unknown Argument: '%s'", lower_arg.data());
			exit(1);
		}

		auto& config = configIter->second;
		config = lower_arg;
	}

	// Get the SDK environment variable
	if (sdk_path.empty())
	{
		char buffer[32'768]; // max env-variable size
		buffer[sizeof(buffer) - 1] = '\0';
		if (GetEnvironmentVariable("VEMIPS_SDK", buffer, sizeof buffer) == 0)
		{
			fprintf(stderr, "Failed to get value of environment variable VEMIPS_SDK. Please confirm that it is configured.\n");
			exit(1);
		}
		sdk_path = buildcarbide::fileutils::fix_up(string_view(buffer));
	}

	fileutils::fix_up_in_place(project_file);
	fileutils::fix_up_in_place(out_file);
	fileutils::fix_up_in_place(sdk_path);

	return options(std::move(project_file), std::move(configuration), std::move(platform), std::move(command), std::move(out_file), std::move(sdk_path));
}
