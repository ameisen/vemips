#pragma once

#include <cstdio>
#include <cstdlib>
#include <utility>

namespace buildcarbide {
	template <typename... Args>
	static int error_printf(const char* __restrict format, Args&&... args) {
		return fprintf(stderr, format, std::forward<Args>(args)...);  // NOLINT(clang-diagnostic-format-nonliteral, clang-diagnostic-format-security)
	}

	template <int ExitCode, typename... Args>
	[[noreturn]] static void fail(const char* __restrict format, Args&&... args) {
		static_assert(ExitCode != 0, "fail ExitCode should not be 0");
		error_printf(format, std::forward<Args>(args)...);
		std::exit(ExitCode);  // NOLINT(concurrency-mt-unsafe)
	}
}
