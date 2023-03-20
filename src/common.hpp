#pragma once

#include "platform/common.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <string>
#include <type_traits>

#include <cassert>
#include <cstdint>
#include <cstddef>

// ReSharper disable IdentifierTypo CppInconsistentNaming
#if defined(_MSC_VER) && !defined(__clang__)
#	define _unpredictable(expr) (expr)
#	define _likely(expr) (expr)
#	define _unlikely(expr) (expr)
#	define _expect(expr, value) (expr)
#	define _nothrow __declspec(nothrow)
#	define _assume(expr) __assume(expr)
#	define _forceinline __forceinline
# define _empty_bases __declspec(empty_bases)
# define _msabi
#	define _vectorcall __vectorcall
# define _trivial
#	define _no_vtable __declspec(novtable)
#	define _flag_enum
#	define _define_segment(name, ...) __pragma(section(#name, __VA_ARGS__))
#	define _segment(name) __declspec(allocate(#name))
# define _allocator __declspec(allocator)
# define _used
# define _no_unique [[no_unique_address, msvc::no_unique_address]]
#	define _clear_cache(start, end)
#	define _constant_p(expression) (false)

namespace mips::intrinsics {
	inline _forceinline _nothrow uint16_t byteswap(const uint16_t value) {
		return _byteswap_ushort(value);
	}
	inline _forceinline _nothrow uint32_t byteswap(const uint32_t value) {
		return _byteswap_ulong(value);
	}
	inline _forceinline _nothrow uint64_t byteswap(const uint64_t value) {
		return _byteswap_uint64(value);
	}

	inline _forceinline _nothrow uint32_t rotate_left(const uint32_t value, const int shift) {
		return _lrotl(value, shift);
	}

	inline _forceinline _nothrow uint32_t rotate_right(const uint32_t value, const int shift) {
		return _lrotr(value, shift);
	}
}

#elif defined(__clang__)
#	define _unpredictable(expr) (__builtin_unpredictable(expr))
#	define _likely(expr) (__builtin_expect(!!(expr), 1))
#	define _unlikely(expr) (__builtin_expect((expr), 0))
#	define _expect(expr, value) (__builtin_expect((expr), (value)))
#	define _nothrow __declspec(nothrow)
#	define _assume(expr) __builtin_assume(expr)
#	define _forceinline __attribute__((always_inline))
# define _empty_bases __declspec(empty_bases)
#	define _msabi __attribute__((ms_abi))
#	define _vectorcall __vectorcall
#	define _trivial __attribute__((trivial_abi))
#	define _no_vtable __declspec(novtable)
#	define _flag_enum __attribute__((flag_enum))
#	define _define_segment(name, ...) __pragma(section(#name, __VA_ARGS__))
#	define _segment(name) __declspec(allocate(#name))
# define _allocator __attribute__((malloc))
# define _used __attribute__((__used__))
#	if __has_cpp_attribute(no_unique_address)
#		define _no_unique [[no_unique_address, msvc::no_unique_address]]
#	else
#		define _no_unique
#	endif
#	define _clear_cache(start, end) __builtin___clear_cache((char*)(start), (char*)(end))
#	define _constant_p(expression) __builtin_constant_p(expression)

namespace mips::intrinsics {
	inline _forceinline _nothrow uint16_t byteswap(const uint16_t value) {
		return _byteswap_ushort(value);
	}
	inline _forceinline _nothrow uint32_t byteswap(const uint32_t value) {
		return _byteswap_ulong(value);
	}
	inline _forceinline _nothrow uint64_t byteswap(const uint64_t value) {
		return _byteswap_uint64(value);
	}

	inline _forceinline _nothrow uint32_t rotate_left(const uint32_t value, const int shift) {
		return _lrotl(value, shift);
	}

	inline _forceinline _nothrow uint32_t rotate_right(const uint32_t value, const int shift) {
		return _lrotr(value, shift);
	}
}
#elif defined(__GNUC__)
# error GCC unimplemented

namespace mips::intrinsics {
	inline _forceinline _nothrow uint16_t byteswap(const uint16_t value) {
		return __builtin_bswap16(value);
	}
	inline _forceinline _nothrow uint32_t byteswap(const uint32_t value) {
		return __builtin_bswap32(value);
	}
	inline _forceinline _nothrow uint64_t byteswap(const uint64_t value) {
		return __builtin_bswap32(value);
	}

	inline _forceinline _nothrow uint32_t rotate_left(const uint32_t value, const int shift) {
		return (value << shift) | (value >> (32 - shift));
	}

	inline _forceinline _nothrow uint32_t rotate_right(const uint32_t value, const int shift) {
		return (value >> shift) | (value << (32 - shift));
	}
}

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
#if _DEBUG
#	define xassert(expr) do { assert(expr); if (!(expr)) { __debugbreak(); } _assume(expr); } while (false)
#else
#	define xassert(expr) do { assert(expr); _assume(expr); } while (false)
#endif

#define xwarn(expr, msg) xwarn_impl((expr), #expr, msg)

namespace mips {
	template <size_t En, size_t Mn>
	static inline void xwarn_impl(bool expr, const char (&__restrict expr_string)[En], const char (& __restrict message)[Mn]) {
#if _DEBUG
		if _unlikely(!expr) {
			fprintf(stderr, "xwarn [%s]: %s", expr_string, message);
		}
#endif
	}

	template <typename T, typename U>
	static constexpr inline T value_assert(U value) {
		static constexpr const T min_value = std::numeric_limits<T>::min();
		static constexpr const T max_value = std::numeric_limits<T>::max();

		using signed_t = std::conditional_t<std::is_signed_v<T>, std::make_signed_t<U>, U>;
		const signed_t signed_value = value;
		xassert(signed_value >= min_value && signed_value <= max_value);
		return T(signed_value);
	}

	template <typename T> using raw_ptr = T*;
	template <typename T> using raw_restrict_ptr = T* __restrict;

	// ReSharper disable CppCStyleCast
	template <typename T> static constexpr const T* null_ptr = raw_ptr<T>(nullptr);
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

	// TODO : move me somewhere
	template <typename T>
	concept IntegralC = std::is_integral_v<T>;

	template <IntegralC ValueT, IntegralC RangeT>
	static constexpr _nothrow _forceinline bool within(const ValueT value, const RangeT min, const RangeT max) {
		return value >= min && value <= max;
	}

	template <IntegralC ValueT, typename LimitsT>
	static constexpr _nothrow _forceinline bool within(const ValueT value, const LimitsT& limits) {
		return value >= limits.min() && value <= limits.max();
	}

	template <IntegralC ValueT>
	static constexpr _nothrow _forceinline bool is_max_value(const ValueT value) {
		return value == std::numeric_limits<ValueT>::max();
	}

	template <typename T>
	concept EnumC = std::is_enum_v<T>;
}
