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
#if defined(__clang__)
#	define _unpredictable(expr) (__builtin_unpredictable(expr))
#	define _likely(expr) (__builtin_expect(!!(expr), 1))
#	define _unlikely(expr) (__builtin_expect((expr), 0))
#	define _expect(expr, value) (__builtin_expect((expr), (value)))
#	define _nothrow __declspec(nothrow)
#	define _assume(expr) __builtin_assume(expr)
#	define _forceinline __attribute__((__always_inline__))
#	define _noinline __attribute__((__noinline__))
#	define _empty_bases __declspec(empty_bases)
#	define _msabi __attribute__((ms_abi))
#	define _vectorcall __vectorcall
#	define _trivial __attribute__((trivial_abi))
#	define _no_vtable __declspec(novtable)
#	define _flag_enum __attribute__((flag_enum))
#	define _define_segment(name, ...) __pragma(section(#name, __VA_ARGS__))
#	define _segment(name) __declspec(allocate(#name))
#	define _allocator __attribute__((__malloc__))
#	define _used __attribute__((__used__))
#	if __has_cpp_attribute(no_unique_address)
#		define _no_unique [[no_unique_address, msvc::no_unique_address]]
#	else
#		define _no_unique
#	endif
#	define _clear_cache(start, end) __builtin___clear_cache((char*)(start), (char*)(end))
#	define _constant_p(expression) __builtin_constant_p(expression)
#	define _pragma_small_code _Pragma("optimize(\"s\", on)") __attribute__((__minsize__))
#	define _pragma_default_code _Pragma("optimize(\"\", on)")
#	define _hot __attribute__((__hot__))
#	define _cold __attribute__((__cold__))
#	define _flatten __attribute__((__flatten__))
#	define _result_noalias __declspec(restrict)

#elif defined(_MSC_VER)
#	define _unpredictable(expr) (expr)
#	define _likely(expr) (expr)
#	define _unlikely(expr) (expr)
#	define _expect(expr, value) (expr)
#	define _nothrow __declspec(nothrow)
#	define _assume(expr) __assume(expr)
#	define _forceinline __forceinline
#	define _noinline __declspec(noinline)
#	define _empty_bases __declspec(empty_bases)
#	define _msabi
#	define _vectorcall __vectorcall
#	define _trivial
#	define _no_vtable __declspec(novtable)
#	define _flag_enum
#	define _define_segment(name, ...) __pragma(section(#name, __VA_ARGS__))
#	define _segment(name) __declspec(allocate(#name))
#	define _allocator __declspec(allocator)
#	define _used
#	define _no_unique [[no_unique_address, msvc::no_unique_address]]
#	define _clear_cache(start, end) FlushInstructionCache(GetCurrentProcess(), (start), static_cast<size_t>((end) - (start)))
#	define _constant_p(expression) (false)
#	define _pragma_small_code _Pragma("optimize(\"s\", on)")
#	define _pragma_default_code _Pragma("optimize(\"\", on)")
#	define _hot
#	define _cold
#	define _flatten
#	define _result_noalias __declspec(restrict)

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
template <uint32 Bit> constexpr uint64 bitmask = bit<Bit> - 1;

// ReSharper disable once IdentifierTypo CppInconsistentNaming
#if _DEBUG
#	define xassert(expr) do { const bool assert_result = bool(expr); assert(assert_result && #expr); if (!assert_result) { __debugbreak(); } _assume(assert_result); } while (false)
#	define xunreachable(message) do { assert(false && message); _assume(0); std::unreachable(); } while(false)
#elif _DEVELOPMENT
#	define xassert(expr) do { const bool assert_result = bool(expr); assert(assert_result && #expr); _assume(assert_result); } while (false)
#	define xunreachable(message) do { assert(false && message); _assume(0); std::unreachable(); } while(false)
#elif _RELEASE
#	define xassert(expr) do { const bool assert_result = bool(expr); _assume(assert_result); if (!(expr)) [[unlikely]] { std::unreachable(); } } while (false)
#	define xunreachable(message) do { _assume(0); std::unreachable(); } while(false)
#else
#	error Unknown Configuration
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

	template <typename T>
	static constexpr T make_bit(const uint32 bit)
	{
		return static_cast<T>(1) << bit;
	}

	template <typename T>
	static constexpr T make_bitmask(const uint32 bit)
	{
		if (bit == sizeof(T) * 8)
		{
			return std::numeric_limits<T>::max();
		}

		return static_cast<T>((static_cast<uint64>(1) << bit) - 1);
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
				std::remove_pointer_t<TFrom>,
				copy_volatile<
					std::remove_pointer_t<TFrom>,
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
				std::remove_reference_t<TFrom>,
				copy_volatile<
					std::remove_reference_t<TFrom>,
					TTo
				>
			>&
		>;

	
	// TODO : missing most qualifiers
	template <typename TFrom, typename TTo>
	using copy_qualifiers =
		std::conditional_t<
			std::is_pointer_v<TTo>,
			copy_qualifiers_ptr<TFrom, std::remove_reference_t<std::remove_pointer_t<TTo>>>,
			std::conditional_t<
				std::is_rvalue_reference_v<TTo>,
				std::remove_reference_t<copy_qualifiers_ref<TFrom, std::remove_reference_t<TTo>>>&&,
				std::conditional_t<
					std::is_lvalue_reference_v<TTo>,
					std::remove_reference_t<copy_qualifiers_ref<TFrom, std::remove_reference_t<TTo>>>&,
					copy_qualifiers_cv<TFrom, TTo>
				>
			>
		>;

	template <typename T>
	requires std::is_integral_v<T>
	static constexpr unsigned log2_floor(const T value)
	{
	    return value == 1 ? 0 : 1+log2_floor(value >> 1);
	}

	template <typename T>
	requires std::is_integral_v<T>
	static constexpr unsigned log2_ceil(const T value)
	{
	    return value == 1 ? 0 : log2_floor(value - 1) + 1;
	}

	enum class ptr_qual
	{
		none = 0,
		restrict
	};

	static constexpr ptr_qual operator&(const ptr_qual a, const ptr_qual b)
	{
		return static_cast<ptr_qual>(std::to_underlying(a) & std::to_underlying(b));
	}

	static constexpr ptr_qual operator|(const ptr_qual a, const ptr_qual b)
	{
		return static_cast<ptr_qual>(std::to_underlying(a) | std::to_underlying(b));
	}

	template <typename T, ptr_qual Qualifiers>
	class held_ptr
	{
	private:
		template <ptr_qual Q>
		static constexpr bool has_qualifiers = (Qualifiers & Q) == Q;

	public:
		using value_type = T;
		using pointer_type = std::conditional_t<
			has_qualifiers<ptr_qual::restrict>,
			T* __restrict,
			T*
		>;
		using reference_type = std::conditional_t<
			has_qualifiers<ptr_qual::restrict>,
			T& __restrict,
			T&
		>;

	private:
		pointer_type value_ = nullptr;

	public:
		_nothrow held_ptr() noexcept = default;
		_nothrow held_ptr(const held_ptr&) noexcept = default;
		_nothrow held_ptr(held_ptr&& other) noexcept
			: held_ptr(other.value_)
		{
			other.value_ = nullptr;
		}
		_nothrow explicit held_ptr(T* const value) noexcept
			: value_(value) {}
		_nothrow explicit held_ptr(nullptr_t) noexcept
			: held_ptr(nullptr) {}
		template <typename U, ptr_qual UQualifiers>
		requires (std::is_assignable_v<T*, U*>)
		_nothrow explicit held_ptr(const held_ptr<U, UQualifiers>& other) noexcept
			: held_ptr(other.value_) {}
		template <typename U, ptr_qual UQualifiers>
		requires (std::is_assignable_v<T*, U*>)
		_nothrow explicit held_ptr(held_ptr<U, UQualifiers>&& other) noexcept //NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
			: held_ptr(other.value_)
		{
			other.value_ = nullptr;
		}

		_nothrow ~held_ptr() noexcept
#if !defined(_DEBUG)
			= default;
#else
		{
			value_ = std::bit_cast<pointer_type>(uintptr(0xFEEFFEEFFEEFFEEFULL));
		}
#endif


		_nothrow held_ptr& operator=(const held_ptr&) noexcept = default;
		_nothrow held_ptr& operator=(held_ptr&& other) noexcept
		{
			value_ = other.value_;
			other.value_ = nullptr;
			return *this;
		}
		_nothrow held_ptr& operator=(T* const value) noexcept
		{
			value_ = value;
			return *this;
		}
		_nothrow held_ptr& operator=(nullptr_t) noexcept
		{
			value_ = nullptr;
			return *this;
		}
		template <typename U, ptr_qual UQualifiers>
		requires (std::is_assignable_v<T*, U*>)
		_nothrow held_ptr& operator=(const held_ptr<U, UQualifiers>& other) noexcept
		{
			value_ = other.value_;
			return *this;
		}
		template <typename U, ptr_qual UQualifiers>
		requires (std::is_assignable_v<T*, U*>)
		_nothrow held_ptr& operator=(held_ptr<U, UQualifiers>&& other) noexcept //NOLINT(cppcoreguidelines-rvalue-reference-param-not-moved)
		{
			value_ = other.value_;
			other.value_ = nullptr;
			return *this;
		}

		_nothrow bool operator==(nullptr_t) const noexcept
		{
			return get() == nullptr;
		}

		_nothrow bool operator==(const T* other) const noexcept
		{
			return get() == other;
		}

		template <typename U>
		requires (std::is_assignable_v<T*, U*>)
		_nothrow bool operator==(const U* other) const noexcept
		{
			return get() == static_cast<T*>(other);
		}

		_nothrow bool operator==(const held_ptr& other) const noexcept
		{
			return get() == other.get();
		}

		template <typename U, ptr_qual UQualifiers>
		requires (std::is_assignable_v<T*, U*>)
		_nothrow bool operator==(const held_ptr<U, UQualifiers>& other) const noexcept
		{
			return get() == static_cast<T*>(other);
		}

		_nothrow bool operator!=(nullptr_t) const noexcept
		{
			return get() != nullptr;
		}

		_nothrow bool operator!=(const T* other) const noexcept
		{
			return get() != other;
		}

		template <typename U>
		requires (std::is_assignable_v<T*, U*>)
		_nothrow bool operator!=(const U* other) const noexcept
		{
			return get() != static_cast<T*>(other);
		}

		_nothrow bool operator!=(const held_ptr& other) const noexcept
		{
			return get() != other.get();
		}

		template <typename U, ptr_qual UQualifiers>
		requires (std::is_assignable_v<T*, U*>)
		_nothrow bool operator!=(const held_ptr<U, UQualifiers>& other) const noexcept
		{
			return get() != static_cast<T*>(other);
		}

		_nothrow explicit operator bool() const noexcept
		{
			return value_ != nullptr;
		}

		_nothrow bool is_valid() const noexcept
		{
			return static_cast<bool>(*this);
		}

		template <typename Self>
		_nothrow copy_qualifiers<Self, pointer_type> operator->(this Self&& self) noexcept //NOLINT(cppcoreguidelines-missing-std-forward)
		{
			return *std::forward<Self>(self);
		}

		template <typename Self>
		_nothrow copy_qualifiers<Self, pointer_type> get(this Self&& self) noexcept //NOLINT(cppcoreguidelines-missing-std-forward)
		{
			T* const result = std::forward<Self>(self).value_;
			return result;
		}

		template <typename Self>
		_nothrow copy_qualifiers<Self, pointer_type> operator*(this Self&& self) noexcept //NOLINT(cppcoreguidelines-missing-std-forward)
		{
			T* const result = std::forward<Self>(self).value_;
			xassert(result != nullptr);
			return result;
		}

		_nothrow void swap(held_ptr& other) noexcept
		{
			xassert(this != &other);

			std::swap(value_, other.value_);
		}

		template <typename U, ptr_qual UQualifiers>
		requires (std::is_assignable_v<T*, U*> && std::is_assignable_v<U*, T*>)
		_nothrow void swap(held_ptr<U, UQualifiers>& other) noexcept
		{
			xassert(this != &other);

			U* const temp = static_cast<U*>(value_);
			value_ = static_cast<T*>(other.value_);
			other.value_ = temp;
		}

		_nothrow void reset() noexcept
		{
			reset(nullptr);
		}

		_nothrow void reset(nullptr_t) noexcept
		{
			value_ = nullptr;
		}

		_nothrow void reset(T* const value) noexcept
		{
			value_ = value;
		}

		template <typename U>
		requires (std::is_assignable_v<T*, U*>)
		_nothrow void reset(U* const value) noexcept
		{
			value_ = static_cast<T*>(value);
		}
	};

	template <typename T, typename... Tt>
	static T* make_unique_inline(std::unique_ptr<T>& ptr, Tt&&... args)
	{
		ptr = std::make_unique<T>(std::forward<Tt>(args)...);
		return ptr.get();
	}

	// .second == true if created 
	template <typename T, typename... Tt>
	static std::tuple<T*, bool> get_or_make_unique_inline(std::unique_ptr<T>& ptr, Tt&&... args)
	{

		if (T* const result = ptr.get())
		{
			return { result, false };
		}
		
		return {
			make_unique_inline(ptr, std::forward<Tt>(args)...),
			true
		};
	}
}

#define _make_qual(type) copy_qualifiers<decltype(self), type>

