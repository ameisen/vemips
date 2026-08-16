#pragma once

#if EMSCRIPTEN

#include <vector>


namespace vemips::base64 {
	[[nodiscard]]
	std::vector<char> decode(const std::vector<char>& encoded_string);
}

#endif
