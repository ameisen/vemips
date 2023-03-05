#pragma once

#include <cstdint>
#include <string>
#include <array>

namespace buildcarbide::fileutils {
	using modtime_t = uint64_t;

	extern modtime_t get_file_modtime(const std::string & __restrict filename);
	extern modtime_t get_file_modtime(FILE * __restrict fp);
}
