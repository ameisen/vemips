#pragma once

#include <cstdint>

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

#include <array>

#if !defined(_MSC_VER)
#  define __assume(x)
#  define __forceinline inline
#endif
#if defined(__clang__)
#  define __restrict
#endif

template <uint32 _bit> constexpr uint32 bit = (1u << _bit);

#define NOMINMAX 1
