#pragma once

#ifndef FMT_HEADER_ONLY
#	define FMT_HEADER_ONLY 1
#	define FMT_HEADER_ONLY_DEFINED 1
#endif

#include <string_view>
#include <cstdio>

#include <fmt/core.h>
#include <fmt/xchar.h>

#include "platform/platform.hpp"

namespace buildcarbide {
	template <typename... Args>
	static no_throw void errorf(std::wstring_view format, Args&&... args) noexcept {
		//fmt::vprint(stderr, L"{}\n", fmt::vformat(format, fmt::make_wformat_args(forward<Args>(args)...)));
		fmt::vprint(format, fmt::make_wformat_args(std::forward<Args>(args)...));
		std::fputs("", stderr);  // NOLINT(cert-err33-c)
	}

	template <size_t Nf, typename... Args>
	static no_throw void errorf(const wchar_t (& __restrict format)[Nf], Args&&... args) noexcept {
		errorf({format, Nf - 1}, forward<Args>(args)...);
	}

	template <typename... Args>
	static no_throw void errorf(std::string_view format, Args&&... args) noexcept {
		//fmt::println(stderr, format, forward<Args>(args)...);
		fmt::vprint(format, fmt::make_format_args(std::forward<Args>(args)...));
		std::fputs("", stderr);  // NOLINT(cert-err33-c)
	}

	template <size_t Nf, typename... Args>
	static no_throw void errorf(const char (& __restrict format)[Nf], Args&&... args) noexcept {
		errorf({format, Nf - 1}, forward<Args>(args)...);
	}

	template <int ExitCode, typename... Args>
	[[noreturn]]
	static no_throw void fail(const std::wstring& format, Args&&... args) noexcept {
		static_assert(ExitCode != 0, "fail ExitCode should not be 0");
		errorf(format, std::forward<Args>(args)...);
		__debugbreak();
		std::exit(ExitCode);  // NOLINT(concurrency-mt-unsafe)
	}

	template <int ExitCode, size_t Nf, typename... Args>
	[[noreturn]]
	static no_throw void fail(const wchar_t (& __restrict format)[Nf], Args&&... args) noexcept {
		fail<ExitCode>({format, Nf - 1}, std::forward<Args>(args)...);
	}

	template <int ExitCode, typename... Args>
	[[noreturn]]
	static no_throw void fail(const std::string& format, Args&&... args) noexcept {
		static_assert(ExitCode != 0, "fail ExitCode should not be 0");
		errorf(format, std::forward<Args>(args)...);
		__debugbreak();
		std::exit(ExitCode);  // NOLINT(concurrency-mt-unsafe)
	}

	template <int ExitCode, size_t Nf, typename... Args>
	[[noreturn]]
	static no_throw void fail(const char (& __restrict format)[Nf], Args&&... args) noexcept {
		fail<ExitCode>({format, Nf - 1}, std::forward<Args>(args)...);
	}
}

#ifdef FMT_HEADER_ONLY_DEFINED
#	undef FMT_HEADER_ONLY
#	undef FMT_HEADER_ONLY_DEFINED
#endif
