#include "buildcarbide.hpp"

#include "options.hpp"

#include "file_utils/file_utils.hpp"

#include <cctype>
#include <algorithm>
#include <locale>

#include "platform/platform_headers.hpp"

using std::wstring_view;

namespace buildcarbide {
	static constexpr const size_t max_environment_variable_length = 32'768;

	options options::build(const std::span<const wchar_t*> args) {
		wstring project_file;
		wstring configuration;
		wstring platform;
		wstring command;
		wstring out_file;
		wstring sdk_path;

		const auto map_option = [&](const wstring_view token) {
			switch (hash(token)) {
				case hash(L"configuration"):
					return &configuration;
				case hash(L"platform"):
					return &platform;
				case hash(L"command"):
					return &command;
				case hash(L"out"):
					return &out_file;
				case hash(L"sdk"):
					return &sdk_path;
				default: [[unlikely]]
					return static_cast<wstring*>(nullptr);
			}
		};

		size_t index = 0;

		const auto pop_argument = [&]() -> wstring_view {
			if (index >= args.size()) [[unlikely]] {
				return {};
			}
			return args[index++];
		};

		const auto pop_argument_value = [&](wstring_view &arg) -> wstring_view {
			size_t split_index = wstring::npos;
			for (const wchar_t split_char : { L'=', L':' }) {
				if (
					const size_t test_index = arg.find(split_char);
					test_index != wstring::npos
				) {
					if (split_index == wstring::npos || test_index < split_index) {
						split_index = test_index;
					}
				}
			}

			if (split_index != wstring::npos) {
				const auto sub_arg = arg.substr(split_index + 1);
				arg = arg.substr(0, split_index);
				return sub_arg;
			}
			else {
				return pop_argument();
			}
		};

		while (index < args.size()) {
			const auto to_lower = [](const wstring_view str) {
				wstring lower_str;
				lower_str.reserve(str.length());

				const auto& __restrict locale = std::locale::classic();
				for (const wchar_t c : str) {
					// TODO : technically performs additional checks, but I'm unsure how to avoid it
					lower_str.push_back(std::tolower(c, locale));
				}

				return lower_str;
			};

			auto arg = pop_argument();

			if (arg.length() > 1 && (arg[0] == L'/' || (arg[0] == L'-' && arg[1] == L'-'))) {
				if (arg[0] == L'/') {
					arg = arg.substr(1);
				}
				else {
					arg = arg.substr(2);
				}

				auto sub_arg = pop_argument_value(arg);
				auto lower_arg = to_lower(arg);
				//auto lower_sub_arg = to_lower(sub_arg);

				if (auto mapping = map_option(lower_arg); mapping != nullptr) [[likely]] {
					*mapping = sub_arg;
				}
				else {
					fail<1>(L"Unknown Argument: '{}'", lower_arg);
				}
			}
			else {
				if (project_file.length() == 0) {
					project_file = arg;
				}
				else if (configuration.length() == 0) {
					configuration = arg;
				}
				else if (platform.length() == 0) {
					platform = arg;
				}
				else if (command.length() == 0) {
					command = arg;
				}
				else if (out_file.length() == 0) {
					out_file = arg;
				}
				else {
					fail<1>(L"Unknown Argument: '{}'", arg);
				}
			}
		}

		// Get the SDK environment variable
		if (sdk_path.empty()) {
			wchar_t buffer[max_environment_variable_length] = { L'\0' };
			if (GetEnvironmentVariable(L"VEMIPS_SDK", buffer, DWORD(extent_of(buffer))) == 0) [[unlikely]] {
				fail<1>(L"Failed to get value of environment variable VEMIPS_SDK. Please confirm that it is configured.");
			}
			sdk_path = file_utils::fix_up(buffer);
		}

		file_utils::fix_up_in_place(project_file);
		file_utils::fix_up_in_place(out_file);
		file_utils::fix_up_in_place(sdk_path);

		return {
			std::move(project_file),
			std::move(configuration),
			std::move(platform),
			std::move(command),
			std::move(out_file),
			std::move(sdk_path)
		};
	}
}
