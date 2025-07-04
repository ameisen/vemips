#pragma once

#ifndef FMT_HEADER_ONLY
#	define FMT_HEADER_ONLY 1
#	define FMT_HEADER_ONLY_DEFINED 1
#endif

#include <string_view>
#include <cstdio>

#undef check

#include <fmt/core.h>
#include <fmt/xchar.h>

#include "platform/platform.hpp"

namespace buildcarbide {
	template <typename... Args>
	static no_throw void errorf(std::wstring_view format, const Args&... args) noexcept {
		//fmt::vprint(stderr, L"{}\n", fmt::vformat(format, fmt::make_wformat_args(forward<Args>(args)...)));
		fmt::vprint(format, fmt::make_wformat_args(std::forward<const Args>(args)...));
		std::fputs("", stderr);  // NOLINT(cert-err33-c)
	}

	template <size_t Nf, typename... Args>
	static no_throw void errorf(const wchar_t (& __restrict format)[Nf], const Args&... args) noexcept {
		errorf({format, Nf - 1}, std::forward<const Args>(args)...);
	}

	template <typename... Args>
	static no_throw void errorf(std::string_view format, const Args&... args) noexcept {
		//fmt::println(stderr, format, forward<Args>(args)...);
		fmt::vprint(format, fmt::make_format_args(std::forward<const Args>(args)...));
		std::fputs("", stderr);  // NOLINT(cert-err33-c)
	}

	template <size_t Nf, typename... Args>
	static no_throw void errorf(const char (& __restrict format)[Nf], const Args&... args) noexcept {
		errorf({format, Nf - 1}, std::forward<const Args>(args)...);
	}

	template <int ExitCode, typename... Args>
	[[noreturn]]
	static no_throw void fail(const std::wstring& format, const Args&... args) noexcept {
		static_assert(ExitCode != 0, "fail ExitCode should not be 0");
		errorf(format, std::forward<const Args>(args)...);
		__debugbreak();
		std::exit(ExitCode);  // NOLINT(concurrency-mt-unsafe)
	}

	template <int ExitCode, size_t Nf, typename... Args>
	[[noreturn]]
	static no_throw void fail(const wchar_t (& __restrict format)[Nf], const Args&... args) noexcept {
		fail<ExitCode>({format, Nf - 1}, std::forward<const Args>(args)...);
	}

	template <int ExitCode, typename... Args>
	[[noreturn]]
	static no_throw void fail(const std::string& format, const Args&... args) noexcept {
		static_assert(ExitCode != 0, "fail ExitCode should not be 0");
		errorf(format, std::forward<const Args>(args)...);
		__debugbreak();
		std::exit(ExitCode);  // NOLINT(concurrency-mt-unsafe)
	}

	template <int ExitCode, size_t Nf, typename... Args>
	[[noreturn]]
	static no_throw void fail(const char (& __restrict format)[Nf], const Args&... args) noexcept {
		fail<ExitCode>({format, Nf - 1}, std::forward<const Args>(args)...);
	}
}

#undef check
// ReSharper disable once CppInconsistentNaming
#define check(condition) carbide_check(condition)

#ifdef FMT_HEADER_ONLY_DEFINED
#	undef FMT_HEADER_ONLY
#	undef FMT_HEADER_ONLY_DEFINED
#endif
