#pragma once

#include <string>

namespace buildcarbide {
	struct configuration final {
		std::wstring target;
		std::wstring config;
		std::wstring base_path;
		std::wstring intermediate_dir;
	};
}
