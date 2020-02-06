#pragma once

#include <unordered_set>
#include <unordered_map>
#include <string>

#include "../configuration.hpp"
#include "../project/project.hpp"
#include "../fileutils/fileutils.hpp"

namespace buildcarbide
{
	extern std::unordered_map<std::string, fileutils::modtime_t> get_c_includes(const configuration & __restrict configuration, const project & __restrict project, const std::string & __restrict filename);
}
