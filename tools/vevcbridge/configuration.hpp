#pragma once

#include <string>

namespace buildcarbide {
	struct configuration final {
		std::string target;
		std::string config;
		std::string base_path;
		std::string intermediate_dir;
	};
}
