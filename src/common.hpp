#pragma once

#include <array>
#include <cstdint>

#if defined(_MSC_VER)
#	define __unpredictable(expr) (expr)
#	define __likely(expr) (expr)
#	define __unlikely(expr) (expr)
#	define __expect(expr, value) (expr)
#elif defined(__clang__)
#	define __unpredictable(expr) (__builtin_unpredictable(expr))
#	define __likely(expr) (__builtin_expect(!!(expr), 1))
#	define __unlikely(expr) (__builtin_expect((expr), 0))
#	define __expect(expr, value) (__builtin_expect((expr), (value)))
#	define __nothrow noexcept
#	define __assume(expr) __builtin_assume(expr)
#	define __forceinline __attribute__((alwaysinline))
#else
#	error unknown toolchain
#endif

#define NOMINMAX 1

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uintptr = uint64;
using intptr = int64;
using usize = uint64;

template <uint32 _bit> constexpr uint64 bit = (uint64(1) << _bit);
