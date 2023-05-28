#pragma once

#if defined(_WIN32)
#  include "win32/file_utils_win32.hpp"
#else
#  error "Unknown Platform"
#endif

#include <string_view>

using std::wstring;
using std::wstring_view;

using namespace std::literals::string_view_literals;

namespace buildcarbide::file_utils {
	static constexpr no_throw size_t string_length(const wstring_view str) noexcept {
		return str.size();
	}

	static constexpr no_throw size_t string_length(const std::pmr::wstring& __restrict str) noexcept {
		return str.size();
	}

	template <size_t N>
	static constexpr no_throw size_t string_length(const wchar_t(& __restrict str)[N]) noexcept {
		for (size_t i = 0; i < N; ++i) {
			if constexpr (str[i] == L'\0') {
				return i;
			}
		}
		return N;
	}

	static no_throw size_t string_length(const wchar_t* __restrict str) noexcept {
		return wcslen(str);
	}

	static constexpr no_throw size_t string_length(const wchar_t c) noexcept {
		return c == L'\0' ? 0 : 1;
	}

	static constexpr no_throw size_t string_length(const std::string_view str) noexcept {
		return str.size();
	}

	static constexpr no_throw size_t string_length(const std::pmr::string& __restrict str) noexcept {
		return str.size();
	}

	template <size_t N>
	static constexpr no_throw size_t string_length(const char(& __restrict str)[N]) noexcept {
		for (size_t i = 0; i < N; ++i) {
			if constexpr (str[i] == '\0') {
				return i;
			}
		}
		return N;
	}

	static no_throw size_t string_length(const char* __restrict str) noexcept {
		return strlen(str);
	}

	static constexpr no_throw size_t string_length(const char c) noexcept {
		return c == '\0' ? 0 : 1;
	}

	static no_throw void fix_up_in_place(std::wstring& __restrict path) noexcept {
		size_t double_slashes = 0;
		bool last_was_slash = false;
		for (auto &i : path) {
			if (i == L'\\') {
				i = L'/';
			}

			const bool is_slash = (i == L'/');
			if (is_slash & last_was_slash) [[unlikely]] {
				++double_slashes;
			}
			last_was_slash = is_slash;
		}

		if (double_slashes != size_t{}) {
			std::wstring sanitized_path;
			sanitized_path.reserve(path.size() - double_slashes);

			wchar_t last_char = L'\0';
			for (const wchar_t c : path) {
				if (last_char == L'/' && c == L'/') {
					continue;
				}

				sanitized_path += c;

				last_char = c;
			}

			path = std::move(sanitized_path);
		}

		if (!path.empty() && path.back() == L'/') {
			path.pop_back();
		}
	}

	static no_throw std::wstring fix_up(const std::wstring& __restrict path) noexcept {
		auto result = path;
		fix_up_in_place(result);
		return result;
	}

	static no_throw std::wstring fix_up(const wstring_view path) noexcept {
		return fix_up(wstring(path));
	}

	static no_throw std::wstring fix_up(const wchar_t* const __restrict path) noexcept {
		return fix_up(wstring_view(path));
	}

	template <typename T>
	static no_throw void combiner(wstring & __restrict combined, const T & __restrict arg) noexcept {
		if (!combined.empty()) {
			combined += L'/';
		}
		combined += arg;
	}

	template <>
	static no_throw void combiner<std::string>(wstring & __restrict combined, const std::string & __restrict arg) noexcept {
		if (!combined.empty()) {
			combined += L'/';
		}
		combined += to_wstring(arg);
	}

	template <>
	static no_throw void combiner<std::string_view>(wstring & __restrict combined, const std::string_view & __restrict arg) noexcept {
		if (!combined.empty()) {
			combined += L'/';
		}
		combined += to_wstring(arg);
	}

	template <>
	static no_throw void combiner<const char *>(wstring & __restrict combined, const char * const & __restrict arg) noexcept {
		if (!combined.empty()) {
			combined += L'/';
		}
		combined += to_wstring(arg);
	}

	template <typename... Ts>
	static no_throw std::wstring build_path(Ts&&... args) noexcept
	{
		size_t total_length = 0;
		(
			[&] {
				total_length += string_length(args);
			}(),
			...
		);

		wstring combined;
		combined.reserve(total_length);

		(
			combiner(combined, std::forward<Ts>(args)),
			...
		);

		fix_up(combined);

		return combined;
	}

	static no_throw std::wstring get_base_path(const std::wstring & __restrict path) {
		const auto is_slash = [](const wchar_t c) {
			return c == L'/';
		};

		size_t current_index = path.size() - 1;
		for (; current_index > 0 && !is_slash(path[current_index]); --current_index) {}
		for (; current_index > 0 && is_slash(path[current_index]); --current_index) {}

		return path.substr(0, current_index + 1);
	}

	static no_throw std::wstring strip_extension(const std::wstring & __restrict filename) {
		// Is there a filename?
		if (filename.empty()) {
			return {};
		}

		const size_t dot_index = filename.rfind(L'.');
		if (dot_index == wstring::npos) {
			return filename;
		}

		if (
			const size_t slash_index = filename.rfind(L'/');
			dot_index < slash_index
		) {
			return filename;
		}

		return filename.substr(0, dot_index);
	}

	static no_throw wstring get_file_name(const wstring& __restrict path) noexcept {
		const size_t last_index = path.rfind(L'/');
		if (last_index == wstring::npos) {
			return path;
		}

		return path.substr(last_index + 1);
	}
}
