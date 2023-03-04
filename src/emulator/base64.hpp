#pragma once

#if EMSCRIPTEN

#include <vector>

namespace base64 {
	std::vector<char> decode(const std::vector<char>& encoded_string);
}

#endif
