#pragma once

#include "platform/common.hpp"

#include <algorithm>
#include <array>
#include <string>

#include <cassert>
#include <cstdint>
#include <cstddef>

// ReSharper disable IdentifierTypo CppInconsistentNaming
#if defined(_MSC_VER)
#	define _unpredictable(expr) (expr)
#	define _likely(expr) (expr)
#	define _unlikely(expr) (expr)
#	define _expect(expr, value) (expr)
#	define _nothrow noexcept
#	define _assume(expr) __assume(expr)
#	define _forceinline __forceinline
#elif defined(__clang__)
#	define _unpredictable(expr) (__builtin_unpredictable(expr))
#	define _likely(expr) (__builtin_expect(!!(expr), 1))
#	define _unlikely(expr) (__builtin_expect((expr), 0))
#	define _expect(expr, value) (__builtin_expect((expr), (value)))
#	define _nothrow noexcept
#	define _assume(expr) __builtin_assume(expr)
#	define _forceinline __attribute__((alwaysinline))
#else
#	error unknown toolchain
#endif
// ReSharper restore IdentifierTypo CppInconsistentNaming

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

// ReSharper disable IdentifierTypo
using uintptr = uint64;
using intptr = int64;
using usize = uint64;
// ReSharper restore IdentifierTypo

template <uint32 Bit> constexpr uint64 bit = (uint64(1) << Bit);

// ReSharper disable once IdentifierTypo CppInconsistentNaming
#define xassert(expr) do { assert(expr); _assume(expr); } while (false)

namespace mips {
	template <typename T, typename U>
	static constexpr inline T value_assert(U value) {
		static constexpr const T min_value = std::numeric_limits<T>::min();
		static constexpr const T max_value = std::numeric_limits<T>::max();

		using signed_t = std::conditional_t<std::is_signed_v<T>, std::make_signed_t<U>, U>;
		const signed_t signed_value = value;
		xassert(signed_value >= min_value && signed_value <= max_value);
		return T(signed_value);
	}

	// ReSharper disable CppCStyleCast
	template <typename T> static constexpr const T* null_ptr = (T*)nullptr;
	// ReSharper restore CppCStyleCast

	template <typename T> struct remove_restrict {
		using type = std::conditional_t<
			std::is_pointer_v<T>,
			std::conditional_t<
				std::is_const_v<T> && std::is_volatile_v<T>,
				std::add_volatile_t<std::add_const_t<std::add_pointer_t<std::remove_pointer_t<T>>>>,
				std::conditional_t<
					std::is_const_v<T>,
					std::add_const_t<std::add_pointer_t<std::remove_pointer_t<T>>>,
					std::conditional_t<
						std::is_volatile_v<T>,
						std::add_volatile_t<std::add_pointer_t<std::remove_pointer_t<T>>>,
						std::add_pointer_t<std::remove_pointer_t<T>>
					>
				>
			>,
			std::conditional_t<
				std::is_lvalue_reference_v<T>,
				std::conditional_t<
					std::is_const_v<T>&& std::is_volatile_v<T>,
					std::add_volatile_t<std::add_const_t<std::add_lvalue_reference_t<std::remove_reference_t<T>>>>,
					std::conditional_t<
						std::is_const_v<T>,
						std::add_const_t<std::add_lvalue_reference_t<std::remove_reference_t<T>>>,
						std::conditional_t<
							std::is_volatile_v<T>,
							std::add_volatile_t<std::add_lvalue_reference_t<std::remove_reference_t<T>>>,
							std::add_lvalue_reference_t<std::remove_reference_t<T>>
						>
					>
				>,
				std::conditional_t<
					std::is_rvalue_reference_v<T>,
					std::conditional_t<
						std::is_const_v<T>&& std::is_volatile_v<T>,
						std::add_volatile_t<std::add_const_t<std::add_rvalue_reference_t<std::remove_reference_t<T>>>>,
						std::conditional_t<
							std::is_const_v<T>,
							std::add_const_t<std::add_rvalue_reference_t<std::remove_reference_t<T>>>,
							std::conditional_t<
								std::is_volatile_v<T>,
								std::add_volatile_t<std::add_rvalue_reference_t<std::remove_reference_t<T>>>,
								std::add_rvalue_reference_t<std::remove_reference_t<T>>
							>
						>
					>,
					T
				>
			>
		>;
	};

	template <typename T> using remove_restrict_t = typename remove_restrict<T>::type;
}
