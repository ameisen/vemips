#pragma once

#include <unordered_map>
#include <string>

#include "configuration.hpp"
#include "project/project.hpp"
#include "file_utils/file_utils.hpp"

namespace buildcarbide {
	[[nodiscard]] extern std::unordered_map<std::wstring, file_utils::modtime_t> get_c_includes(
		const configuration & __restrict configuration,
		const project & __restrict project,
		const std::wstring & __restrict filename
	);
}
