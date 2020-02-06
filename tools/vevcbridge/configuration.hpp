#pragma once

#include <string>

namespace buildcarbide
{
	struct configuration
	{
		std::string target;
		std::string config;
		std::string base_path;
		std::string intermediate_dir;
	};
}
