#include "options.hpp"

#include <Windows.h>

#include <algorithm>
#include <cctype>
#include <string_view>
#include <unordered_map>
#include <locale>

#include "buildcarbide.hpp"
#include "fileutils/fileutils.hpp"

using std::string_view;

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

	const auto pop_argument = [&]() -> string_view {
		if (index >= args.size()) {
			return {};
		}
		return args[index++];
	};

	project_file = pop_argument();

	const auto pop_argument_value = [&](string_view &arg) -> string_view {
		size_t split_index = string::npos;
		for (const char split_char : { '=', ':' }) {
			if (
				const size_t test_index = arg.find(split_char);
				test_index != string::npos
			) {
				if (split_index == string::npos || test_index < split_index) {
					split_index = test_index;
				}
			}
		}

		if (split_index != string::npos) {
			const auto sub_arg = arg.substr(split_index + 1);
			arg = arg.substr(0, split_index);
			return sub_arg;
		}
		else {
			return pop_argument();
		}
	};

	while (index < args.size()) {
		const auto to_lower = [](const string_view str) {
			auto lower_str = string(str);
			const auto& __restrict locale = std::locale::classic();
			for (char& c : lower_str) {
				c = std::tolower(c, locale);
			}
			return lower_str;
		};

		auto arg = pop_argument();
		auto lower_arg = to_lower(arg);

		auto sub_arg = pop_argument_value(arg);
		auto lower_sub_arg = to_lower(sub_arg);

		auto config_iterator = options_map.find(lower_arg);
		if (config_iterator == options_map.end()) {
			fail<1>("Unknown Argument: '%s'\n", lower_arg.data());
		}

		auto& config = config_iterator->second;
		config = lower_sub_arg;
	}

	// Get the SDK environment variable
	if (sdk_path.empty()) {
		char buffer[32'768]; // max env-variable size
		buffer[sizeof(buffer) - 1] = '\0';
		if (GetEnvironmentVariable("VEMIPS_SDK", buffer, sizeof buffer) == 0) {
			fail<1>("Failed to get value of environment variable VEMIPS_SDK. Please confirm that it is configured.\n");
		}
		sdk_path = fileutils::fix_up(string_view(buffer));
	}

	fileutils::fix_up_in_place(project_file);
	fileutils::fix_up_in_place(out_file);
	fileutils::fix_up_in_place(sdk_path);

	return {
		std::move(project_file),
		std::move(configuration),
		std::move(platform),
		std::move(command),
		std::move(out_file),
		std::move(sdk_path)
	};
}
