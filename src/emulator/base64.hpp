#pragma once

#include <vector>

namespace base64 {
	std::vector<char> decode(const std::vector<char>& encoded_string);
}
