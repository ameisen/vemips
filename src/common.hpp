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

#include "uni_algo.h"

#ifdef _MSC_VER
#	pragma warning(push, 0)
#endif
#define FMT_HEADER_ONLY 1
#include <fmt/format.h>
#ifdef UNICODE
#	include <fmt/xchar.h>
#endif
#ifdef _MSC_VER
#	pragma warning(pop)
#endif

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

#elif defined(__GNUC__)
# error GCC unimplemented

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

#ifdef UNICODE
using tchar = wchar_t;
using tstring_view = std::wstring_view;
using tstring = std::wstring;

#define TCHAR_FORMAT "%ls"
#define TSTR(expr) L ## expr
#else
using tchar = char;
using tstring_view = std::string_view;
using tstring = std::string;

#define TCHAR_FORMAT "%s"
#define TSTR(expr) expr
#endif

template <uint32 Bit> constexpr uint64 bit = (uint64(1) << Bit);

// ReSharper disable once IdentifierTypo CppInconsistentNaming
#if _DEBUG
#	define xassert(expr) do { const bool assert_result = bool(expr); assert(assert_result && #expr); if (!assert_result) { __debugbreak(); } _assume(assert_result); } while (false)
#else
#	define xassert(expr) do { const bool assert_result = bool(expr); assert(assert_result && #expr); _assume(assert_result); } while (false)
#endif

#define xwarn(expr, msg) xwarn_impl((expr), #expr, msg)

namespace mips {
	template <size_t En, size_t Mn>
	static inline void xwarn_impl(const bool expr, const char (&__restrict expr_string)[En], const char (& __restrict message)[Mn]) {
#if _DEBUG
		if _unlikely(!expr) {
			fmt::println(stderr, "xwarn [{}]: {}", expr_string, message);
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

	template <typename T, size_t N>
	static constexpr _nothrow _forceinline size_t count_of(T (& __restrict)[N]) {
		return N;
	}

	template<class T> struct is_restrict          : std::false_type {};
	template<class T> struct is_restrict<T* __restrict> : std::true_type {};
	template<class T> struct is_restrict<T& __restrict> : std::true_type {};

	template <typename Type>
	static constexpr const bool is_restrict_v = false;

	template <typename TFrom, typename TTo>
	using copy_const = std::conditional_t<
		std::is_const_v<TFrom>,
		const TTo,
		TTo
	>;

	template <typename TFrom, typename TTo>
	using copy_volatile = std::conditional_t<
		std::is_volatile_v<TFrom>,
		volatile TTo,
		TTo
	>;

	template <typename TFrom, typename TTo>
	using copy_restrict = std::conditional_t<
		std::is_volatile_v<TFrom>,
		volatile TTo,
		TTo
	>;

	template <typename TFrom, typename TTo>
	using copy_qualifiers_cv =
		copy_const<
			TFrom,
			copy_volatile<
				TFrom,
				TTo
			>
		>;

	// TODO : missing most qualifiers
	template <typename TFrom, typename TTo>
	using copy_qualifiers_ptr =
		copy_restrict<
			TFrom,
			copy_const<
				TFrom,
				copy_volatile<
					TFrom,
					TTo
				>
			>*
		>;

		// TODO : missing most qualifiers
	template <typename TFrom, typename TTo>
	using copy_qualifiers_ref =
		copy_restrict<
			TFrom,
			copy_const<
				TFrom,
				copy_volatile<
					TFrom,
					TTo
				>
			>&
		>;

	
	// TODO : missing most qualifiers
	template <typename TFrom, typename TTo>
	using copy_qualifiers =
		std::conditional_t<
			std::is_pointer_v<TTo>,
			copy_qualifiers_ptr<TFrom, std::remove_pointer_t<TTo>>,
			std::conditional_t<
				std::is_rvalue_reference_v<TTo>,
				std::remove_reference_t<copy_qualifiers_ptr<TFrom, std::remove_reference_t<TTo>>>&&,
				std::conditional_t<
					std::is_lvalue_reference_v<TTo>,
					std::remove_reference_t<copy_qualifiers_ptr<TFrom, std::remove_reference_t<TTo>>>&,
					copy_qualifiers_cv<TFrom, TTo>
				>
			>
		>;
}

#define _make_qual(type) copy_qualifiers<decltype(self), type>

