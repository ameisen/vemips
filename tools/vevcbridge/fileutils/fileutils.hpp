#pragma once

#if defined(_WIN32)
#  include "win32/fileutils_win32.hpp"
#else
#  error "Unknown Platform"
#endif

#include <string_view>

using std::string;
using std::string_view;

using namespace std::literals::string_view_literals;

namespace buildcarbide::fileutils
{
	static constexpr size_t string_length(string_view str) {
		return str.size();
	}

	static constexpr size_t string_length(const string& __restrict str) {
		return str.size();
	}

	template <size_t N>
	static constexpr size_t string_length(const char(&str)[N]) {
		for (size_t i = 0; i < N; ++i) {
			if (str[i] == '\0') {
				return i;
			}
		}
		return N;
	}

	static size_t string_length(const char* __restrict str) {
		return strlen(str);
	}

	static constexpr size_t string_length(char c) {
		return 1;
	}

	static void fix_up_in_place(std::string& __restrict path) {
		for (size_t i = 0; i < path.size(); ++i) {
			if (path[i] == '\\') {
				path[i] = '/';
			}
		}

		if (path.contains("//"sv)) {
			string sanitized_path;
			sanitized_path.reserve(path.size() - 1);

			char last_char = '\0';
			for (char c : path) {
				if (last_char == '/' && c == '/') {
					continue;
				}

				sanitized_path += c;

				last_char = c;
			}

			path = sanitized_path;
		}

		if (!path.empty() && path.back() == '/') {
			path.pop_back();
		}
	}

	static std::string fix_up(const std::string& __restrict path)
	{
		auto result = path;
		fix_up_in_place(result);
		return result;
	}

	static std::string fix_up(string_view path) {
		return fix_up(string(path));
	}

	static std::string fix_up(const char* __restrict path) {
		return fix_up(string_view(path));
	}

	template <typename... Ts>
	static std::string build_path(Ts... args)
	{
		size_t total_length = 0;
		([&] {
			total_length += string_length(args);
			}(), ...);

		string combined;
		combined.reserve(total_length);

		const auto combiner = [&] (const auto &arg) {
			if (!combined.empty()) {
				combined += '/';
			}
			combined += arg;
		};

		(
			combiner(args),
			...
		);

		fix_up(combined);

		return combined;
	}

	static std::string get_base_path(const std::string & __restrict path)
	{
		const auto is_slash = [](char c) {
			return c == '/';
		};

		size_t current_index = path.size() - 1;
		for (; current_index > 0 && !is_slash(path[current_index]); --current_index) {}
		for (; current_index > 0 && is_slash(path[current_index]); --current_index) {}

		if (current_index < 0) {
			return {};
		}

		return path.substr(0, current_index + 1);
	}

	static std::string strip_extension(const std::string & __restrict filename)
	{
		// Is there a filename?
		if (filename.empty())
		{
			return {};
		}

		const size_t dot_index = filename.rfind('.');
		const size_t slash_index = filename.rfind('/');

		if (dot_index == string::npos || dot_index < slash_index) {
			return filename;
		}

		return filename.substr(0, dot_index);
	}
}
