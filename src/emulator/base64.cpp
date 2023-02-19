/* 
   base64.cpp and base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

#include "pch.hpp"
#include "base64.hpp"

#include <iostream>

namespace {
  static constexpr const std::string_view base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

  static inline bool is_alphanumeric(char c) {
    return
      (c >= '0' && c <= '9') ||
      (c >= 'A' && c <= 'Z') ||
      (c >= 'a' && c <= 'z');
  }

  static inline bool is_base64(char c) {
    return (is_alphanumeric(c) || (c == '+') || (c == '/'));
  }
}

namespace base64 {
  std::vector<char> decode(const std::vector<char>& encoded_string) {
    size_t in_len = encoded_string.size();
    std::array<uint8_t, 4> char_array_4;
    std::array<uint8_t, 3> char_array_3;
    std::vector<char> ret;

    const char* __restrict encoded_data = encoded_string.data();

    size_t i = 0;
    size_t ii = 0;
    while (in_len-- && (encoded_data[ii] != '=') && is_base64(encoded_data[ii])) {
      char_array_4[i++] = encoded_data[ii++];
      if (i != 4) {
        continue;
      }
      i = 0;

      for (auto& c : char_array_4) {
        c = (unsigned char)base64_chars.find(c);
      }

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (auto c : char_array_3) {
        ret.push_back(c);
      }
    }

    if (i) {
      for (auto& c : char_array_4) {
        c = (unsigned char)base64_chars.find(c);
      }

      ret.push_back((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
      if (i >= 2) {
        ret.push_back(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
      }
      if (i == 3) {
        ret.push_back(((char_array_4[2] & 0x3) << 6) + char_array_4[3]);
      }
    }

    return ret;
  }
}
