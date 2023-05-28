#pragma once

#include <string>
#include <optional>

namespace buildcarbide::file_utils {
	using modtime_t = uint64;

	enum class filetime {
		modified = 0,
		created = 1,
		accessed = 2,
	};

	extern no_throw std::optional<modtime_t> get_file_time(filetime time_type, const std::wstring & __restrict filename) noexcept;
	extern no_throw std::optional<modtime_t> get_file_time(filetime time_type, FILE * __restrict file) noexcept;
}
