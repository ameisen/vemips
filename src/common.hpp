#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

//#include "uni_algo.h"

#ifdef _MSC_VER
#	pragma warning(push, 0)
#	include <sal.h>
#endif
#include <fmt/format.h>
#ifdef UNICODE
#	include <fmt/xchar.h>
#endif
#undef FMT_HEADER_ONLY
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
#	define _forceinline_lambda(...) [[clang::always_inline]] __VA_ARGS__
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
#	define _noreturn __attribute__((__noreturn__))
#	define _noreturn_pre
#	define _noreturn_post _noreturn
#	if __has_cpp_attribute(no_unique_address) && __has_cpp_attribute(msvc::no_unique_address)
#		define _no_unique [[no_unique_address, msvc::no_unique_address]]
#	elif __has_cpp_attribute(msvc::no_unique_address)
#		define _no_unique [[msvc::no_unique_address]]
#	elif __has_cpp_attribute(no_unique_address)
#		define _no_unique [[no_unique_address]]
#	else
#		define _no_unique
#	endif
#	define _clear_cache(start, end) __builtin___clear_cache((char*)(start), (char*)(end))
#	define _constant_p(expression) __builtin_constant_p(expression)
#	define _pragma_small_code
#	define _pragma_default_code
#	define _hot __attribute__((__hot__))
#	define _cold __attribute__((__minsize__, __cold__))
#	define _flatten __attribute__((__flatten__))
#	define _result_noalias __declspec(restrict)
#	define _pure __attribute__((__pure__)) __declspec(noalias)
#	define _func_const __attribute__((__const__)) __declspec(noalias)
#	if __has_cpp_attribute(msvc::forceinline_calls)
#		define _block_forceinline [[msvc::forceinline_calls]]
#	else
#		define _block_forceinline
#	endif
#	if __has_cpp_attribute(msvc::intrinsic)
#		define _intrinsic [[msvc::intrinsic]]
#	else
#		define _intrinsic
#	endif

#	define _clang_pragma(...) _Pragma(#__VA_ARGS__)
#	if _MSC_VER
#		define _msvc_pragma(...) _Pragma(#__VA_ARGS__)
#	else
#		define _msvc_pragma(...)
#	endif

#	define VEMIPS_JIT_ABI [[gnu::ms_abi]]
#	define VEMIPS_JIT_ABI_INFIX
#	define VEMIPS_JIT_ABI_PREFIX [[gnu::ms_abi]]
#elif defined(_MSC_VER)
#	define _unpredictable(expr) (expr)
#	define _likely(expr) (expr)
#	define _unlikely(expr) (expr)
#	define _expect(expr, value) (expr)
#	define _nothrow __declspec(nothrow)
#	define _assume(expr) __assume(expr)
#	define _forceinline __forceinline
#	define _noinline __declspec(noinline)
#	define _forceinline_lambda(...) __VA_ARGS__ [[msvc::forceinline]]
#	define _empty_bases __declspec(empty_bases)
#	define _msabi __cdecl
#	define _vectorcall __vectorcall
#	define _trivial
#	define _no_vtable __declspec(novtable)
#	define _flag_enum
#	define _define_segment(name, ...) __pragma(section(#name, __VA_ARGS__))
#	define _segment(name) __declspec(allocate(#name))
#	define _allocator __declspec(allocator)
#	define _used
#	define _noreturn __declspec(noreturn)
#	define _noreturn_pre _noreturn
#	define _noreturn_post // TODO : fixme
#	define _no_unique [[no_unique_address, msvc::no_unique_address]]
#	define _clear_cache(start, end) \
	FlushInstructionCache(GetCurrentProcess(), (start), static_cast<size_t>(reinterpret_cast<uintptr>(end) - reinterpret_cast<uintptr>(start)))
#	define _constant_p(expression) (false)
#	define _pragma_small_code _Pragma("optimize(\"s\", on)")
#	define _pragma_default_code _Pragma("optimize(\"\", on)")
#	define _hot
#	define _cold
#	define _flatten [[msvc::flatten]]
#	define _result_noalias __declspec(restrict)
#	define _pure __declspec(noalias)
#	define _func_const _pure
#	define _block_forceinline [[msvc::flatten, msvc::forceinline_calls]]
#	if __has_cpp_attribute(msvc::intrinsic)
#		define _intrinsic [[msvc::intrinsic]]
#	else
#		define _intrinsic
#	endif
#	define __has_builtin(...) 0

#	define _clang_pragma(...)
#	define _msvc_pragma(...) _Pragma(#__VA_ARGS__)

#	define VEMIPS_JIT_ABI
#	define VEMIPS_JIT_ABI_INFIX __cdecl
#	define VEMIPS_JIT_ABI_PREFIX __cdecl

#elif defined(__GNUC__)
# error GCC unimplemented

#else
#	error unknown toolchain
#endif

#define VEMIPS_JIT_ABI_DECL _msabi

// ReSharper restore IdentifierTypo CppInconsistentNaming

using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

// ReSharper disable IdentifierTypo
using uintptr = std::uintptr_t;
using intptr = std::intptr_t;
using uptr = uintptr;
using sptr = intptr;
using usize = std::size_t;
using ssize = std::ptrdiff_t;
// ReSharper restore IdentifierTypo

template <typename T>
concept char_type = std::same_as<T, char> || std::same_as<T, wchar_t>;

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

template <char_type CharT>
// ReSharper disable once IdentifierTypo
class [[jetbrains::pass_by_value]] zstring_view : public std::basic_string_view<CharT>
{
	using base = std::basic_string_view<CharT>;

public:
	using const_pointer = base::const_pointer;
	using size_type = base::size_type;
	using base::npos;

	zstring_view() noexcept = default;
	zstring_view(const zstring_view& other) noexcept = default;
	zstring_view(const CharT* __restrict str) noexcept : base(str) {}
	zstring_view(base) noexcept = delete;

	zstring_view& operator=(const zstring_view& other) noexcept = default;
	zstring_view& operator=(const CharT* __restrict str) noexcept
	{
		static_cast<base*>(this)->operator=(str);
		return *this;
	}
	zstring_view& operator=(base) noexcept = delete;

	[[nodiscard]]
	constexpr const_pointer c_str() const noexcept
	{
		return static_cast<const base*>(this)->data();
	}

	[[nodiscard]]
	constexpr zstring_view substr(const size_type offset, size_type count = npos) const
	{
		return std::bit_cast<zstring_view>(static_cast<const base*>(this)->substr(offset, count));
	}
};

// ReSharper disable once IdentifierTypo
using tzstring_view = zstring_view<tchar>;

// ReSharper disable once IdentifierTypo
using uptr_guest = uint32;
// ReSharper disable once IdentifierTypo
using sptr_guest = int32;

using usize_guest = uint32;
// ReSharper disable once IdentifierTypo
using ssize_guest = int32;

static constexpr uptr_guest operator ""_uptr_guest(const unsigned long long int value)
{
	if (std::numeric_limits<uptr_guest>::max() < value)
	{
		throw 0;
	}

	return static_cast<uptr_guest>(value);
}

static constexpr sptr_guest operator ""_sptr_guest(const unsigned long long int value)
{
	if (std::numeric_limits<sptr_guest>::max() < value || value < 0)
	{
		throw 0;
	}

	return static_cast<sptr_guest>(value);
}

static constexpr usize_guest operator ""_z_guest(const unsigned long long int value)
{
	if (std::numeric_limits<usize_guest>::max() < value)
	{
		throw 0;
	}

	return static_cast<usize_guest>(value);
}

static constexpr ssize_guest operator ""_sz_guest(const unsigned long long int value)
{
	if (std::numeric_limits<ssize_guest>::max() < value || value < 0)
	{
		throw 0;
	}

	return static_cast<ssize_guest>(value);
}

struct guest_span final
{
	uptr_guest offset = 0;
	usize_guest size = 0;

	_pure
	_nothrow _forceinline bool is_empty() const __restrict noexcept {
		return size == 0;
	}
};
static_assert(sizeof(guest_span) == sizeof(uint32) * 2);
static_assert(alignof(guest_span) == alignof(uint32));

template <std::integral T, uint32 Bit> inline constexpr T bit = (T(1) << Bit);
template <std::integral T, uint32 Bit> inline constexpr T bitmask = bit<T, Bit> - 1;

// ReSharper disable once IdentifierTypo CppInconsistentNaming
#define _xassert_assume(...) \
	do { \
		_Pragma("clang diagnostic push")\
		_Pragma("clang diagnostic ignored \"-Wconstant-evaluated\"")\
		if constexpr (!std::is_constant_evaluated()) { _assume(__VA_ARGS__); } \
		_Pragma("clang diagnostic pop")\
	} while (false)

#if _DEBUG
#	define xassert(expr) do { const bool assert_result = bool(expr); assert(assert_result && #expr); if (!assert_result) { __debugbreak(); } _xassert_assume(assert_result); } while (false)
#	define xunreachable(message) do { assert(false && message); _assume(0); std::unreachable(); } while(false)
#elif _DEVELOPMENT
#	define xassert(expr) do { const bool assert_result = bool(expr); assert(assert_result && #expr); _xassert_assume(assert_result); } while (false)
#	define xunreachable(message) do { assert(false && message); _assume(0); std::unreachable(); } while(false)
#elif _RELEASE
#	define xassert(expr) do { const bool assert_result = bool(expr); _xassert_assume(assert_result); if (!(expr)) [[unlikely]] { std::unreachable(); } } while (false)
#	define xunreachable(message) do { _assume(0); std::unreachable(); } while(false)
#else
#	error Unknown Configuration
#endif

#define xwarn(expr, msg) detail::xwarn_impl((expr), #expr, msg)

#define MAKE_BITFLAG_ENUM(enum_type) \
	[[nodiscard]] inline constexpr _func_const _forceinline _nothrow enum_type operator &(const enum_type a, const enum_type b) noexcept { \
		using underlying_t = ::std::underlying_type_t<decltype(a)>; \
		return decltype(a)(underlying_t(a) & underlying_t(b)); \
	} \
 \
	[[nodiscard]] inline constexpr _func_const _forceinline _nothrow enum_type operator |(const enum_type a, const enum_type b) noexcept { \
		using underlying_t = ::std::underlying_type_t<decltype(a)>; \
		return decltype(a)(underlying_t(a) | underlying_t(b)); \
	} \
 \
	inline constexpr _forceinline _nothrow enum_type operator &=(enum_type& __restrict a, const enum_type b) noexcept { \
		using underlying_t = ::std::underlying_type_t<decltype(b)>; \
		return a = decltype(b)(underlying_t(a) & underlying_t(b)); \
	} \
 \
	inline constexpr _forceinline _nothrow enum_type operator |=(enum_type& __restrict a, const enum_type b) noexcept { \
		using underlying_t = ::std::underlying_type_t<decltype(b)>; \
		return a = decltype(b)(underlying_t(a) | underlying_t(b)); \
	} \
 \
	[[nodiscard]] inline constexpr _func_const _forceinline _nothrow enum_type operator ~(const enum_type v) noexcept { \
		using underlying_t = ::std::underlying_type_t<decltype(v)>; \
		return decltype(v)(~underlying_t(v)); \
	} \
 \
	[[nodiscard]] inline constexpr _func_const _forceinline _nothrow bool operator !(const enum_type v) noexcept { \
		return v == enum_type{0}; \
	}

namespace mips {
	namespace detail {

#if _DEBUG
		template <size_t En, size_t Mn>
		_noinline _nothrow void xwarn_inner_impl(
			const char (&__restrict expr_string)[En],
			const char (& __restrict message)[Mn]
		) noexcept {
			// TODO : add try/catch just in case?
			fmt::println(stderr, "xwarn [{}]: {}", expr_string, message);
		}
#endif

		template <size_t En, size_t Mn>
		_forceinline _nothrow void xwarn_impl(
#if !_DEBUG
			[[maybe_unused]]
#endif
			const bool expr,
#if !_DEBUG
			[[maybe_unused]]
#endif
			const char (& __restrict expr_string)[En],
#if !_DEBUG
			[[maybe_unused]]
#endif
			const char (& __restrict message)[Mn]
		) noexcept {
#if _DEBUG
			// TODO : add try/catch just in case?
			if _unlikely(!expr) {
				xwarn_inner_impl(expr_string, message);
			}
#endif
		}

	}

	namespace enumeration
	{
		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		[[nodiscard]]
		_func_const
		_forceinline _nothrow static constexpr bool has_all(const EnumT value, const Enums... enums) noexcept
		{
			static_assert((... && std::is_same_v<EnumT, std::remove_cvref_t<decltype(enums)>>));

			const EnumT reference = (... | enums);
			
			return (value & reference) == reference;
		}

		/// <summary>
		/// <paramref name="value" /> must not alias any other arguments.
		/// </summary>
		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		[[nodiscard]]
		_forceinline _nothrow static constexpr bool has_all_clear(EnumT& __restrict value, const Enums... enums) noexcept
		{
			static_assert((... && std::is_same_v<EnumT, std::remove_cvref_t<decltype(enums)>>));

			const EnumT reference = (... | enums);
			
			EnumT local_value = value;

			if ((local_value & reference) == reference)
			{
				local_value &= ~reference;
				value = local_value;;
				return true;	
			}

			return false;
		}

		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		[[nodiscard]]
		_func_const
		_forceinline _nothrow static constexpr bool has_all_masked(const EnumT value, const EnumT mask, const Enums... enums) noexcept
		{
			static_assert((... && std::is_same_v<EnumT, std::remove_cvref_t<decltype(enums)>>));

			const EnumT reference = (... | enums);
			
			return (value & mask) == reference;
		}

		/// <summary>
		/// <paramref name="value" /> must not alias any other arguments.
		/// </summary>
		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		[[nodiscard]]
		_forceinline _nothrow static constexpr bool has_all_masked_clear(EnumT& __restrict value, const EnumT mask, const Enums... enums) noexcept
		{
			static_assert((... && std::is_same_v<EnumT, std::remove_cvref_t<decltype(enums)>>));

			const EnumT reference = (... | enums);
			
			EnumT local_value = value;

			if ((local_value & mask) == reference)
			{
				local_value &= ~reference;
				value = local_value;;
				return true;	
			}

			return false;
		}

		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		[[nodiscard]]
		_func_const
		_forceinline _nothrow static constexpr bool has_any(const EnumT value, const Enums... enums) noexcept
		{
			static_assert((... && std::is_same_v<EnumT, std::remove_cvref_t<decltype(enums)>>));

			const EnumT reference = (... | enums);
			
			return (value & reference) != EnumT{0};
		}

		/// <summary>
		/// <paramref name="value" /> must not alias any other arguments.
		/// </summary>
		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		[[nodiscard]]
		_forceinline _nothrow static constexpr bool has_any_clear(EnumT& __restrict value, const Enums... enums) noexcept
		{
			static_assert((... && std::is_same_v<EnumT, std::remove_cvref_t<decltype(enums)>>));

			const EnumT reference = (... | enums);
			
			EnumT local_value = value;

			if ((local_value & reference) != EnumT{0})
			{
				local_value &= ~reference;
				value = local_value;;
				return true;	
			}

			return false;
		}

		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		[[nodiscard]]
		_func_const
		_forceinline _nothrow static constexpr EnumT set(const EnumT value, const Enums... enums) noexcept
		{
			static_assert((... && std::is_same_v<EnumT, std::remove_cvref_t<decltype(enums)>>));

			const EnumT reference = (... | enums);
			
			return value | reference;
		}

		/// <summary>
		/// <paramref name="value" /> must not alias any other arguments.
		/// </summary>
		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		_forceinline _nothrow static constexpr void set_inline(EnumT& __restrict value, const Enums... enums) noexcept
		{
			value = set(value, enums...);
		}

		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		[[nodiscard]]
		_func_const
		_forceinline _nothrow static constexpr EnumT clear(const EnumT value, const Enums... enums) noexcept
		{
			static_assert((... && std::is_same_v<EnumT, std::remove_cvref_t<decltype(enums)>>));

			const EnumT reference = (... | enums);
			
			return value & ~reference;
		}

		/// <summary>
		/// <paramref name="value" /> must not alias any other arguments.
		/// </summary>
		template <typename EnumT, typename... Enums> requires (std::is_enum_v<EnumT> && sizeof...(Enums) != 0)
		_forceinline _nothrow static constexpr void clear_inline(EnumT& __restrict value, const Enums... enums) noexcept
		{
			value = clear(value, enums...);
		}
	}

	// TODO : move me somewhere
	template <typename T>
	concept IntegralC = std::is_integral_v<T>;

	template <typename T>
	concept NumericC = std::is_integral_v<T> || std::is_floating_point_v<T>;

	template <typename T>
	concept EnumC = std::is_enum_v<T>;

	template <NumericC T, NumericC U>
	[[nodiscard]]
	inline constexpr _nothrow T value_assert(U value) noexcept {
		static constexpr const T min_value = std::numeric_limits<T>::lowest();
		static constexpr const T max_value = std::numeric_limits<T>::max();

		using signed_t = std::conditional_t<std::is_signed_v<T>, std::make_signed_t<U>, U>;
		const signed_t signed_value = value;
		xassert(signed_value >= min_value);
		xassert(signed_value <= max_value);
		return T(signed_value);
	}

	template <IntegralC T>
	inline constexpr const int32 num_bits = std::numeric_limits<T>::digits + (std::is_signed_v<T> ? 1 : 0);

	template <IntegralC T>
	[[nodiscard]]
	inline constexpr _func_const _forceinline _nothrow T make_bit(const uint32 bit) noexcept
	{
		xassert(bit < num_bits<T>);

		return static_cast<T>(1) << bit;
	}

	template <IntegralC T>
	[[nodiscard]]
	inline constexpr _func_const _forceinline _nothrow T make_bitmask(const uint32 bit) noexcept
	{
		xassert(bit <= num_bits<T>);

		if (bit == sizeof(T) * 8)
		{
			return std::numeric_limits<T>::max();
		}

		return static_cast<T>((static_cast<uint64>(1) << bit) - 1);
	}

	template <IntegralC T, uint32 Bit> requires (Bit < num_bits<T>)
	inline constexpr const T bit = make_bit<T>(Bit);

	template <IntegralC T, uint32 Bit> requires (Bit <= num_bits<T>)
	inline constexpr const T bitmask = make_bitmask<T>(Bit);

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

	template <typename T> using remove_restrict_t = remove_restrict<T>::type;

	template <IntegralC ValueT, IntegralC RangeT>
	[[nodiscard]]
	inline constexpr _func_const _nothrow _forceinline bool within(const ValueT value, const RangeT min, const RangeT max) noexcept {
		return value >= min && value <= max;
	}

	template <IntegralC ValueT, typename LimitsT>
	[[nodiscard]]
	inline constexpr _pure _nothrow _forceinline bool within(const ValueT value, const LimitsT& limits) noexcept {
		return value >= limits.min() && value <= limits.max();
	}

	template <IntegralC ValueT>
	[[nodiscard]]
	inline constexpr _func_const _nothrow _forceinline bool is_max_value(const ValueT value) noexcept {
		return value == std::numeric_limits<ValueT>::max();
	}

	template <typename T, size_t N>
	[[nodiscard]]
	inline constexpr _func_const _nothrow _forceinline size_t count_of(T (& __restrict)[N]) noexcept {
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

	enum class ptr_qual
	{
		none = 0,
		restrict
	};

	MAKE_BITFLAG_ENUM(ptr_qual);

	template <typename T, ptr_qual Qualifiers>
	class held_ptr
	{
	private:
		template <ptr_qual Q>
		static constexpr const bool has_qualifiers = (Qualifiers & Q) == Q;

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

		[[nodiscard]]
		_pure
		_nothrow bool operator==(nullptr_t) const noexcept
		{
			return get() == nullptr;
		}

		[[nodiscard]]
		_pure
		_nothrow bool operator==(const T* other) const noexcept
		{
			return get() == other;
		}

		template <typename U>
		requires (std::is_assignable_v<T*, U*>)
		[[nodiscard]]
		_pure
		_nothrow bool operator==(const U* other) const noexcept
		{
			return get() == static_cast<T*>(other);
		}

		[[nodiscard]]
		_pure
		_nothrow bool operator==(const held_ptr& other) const noexcept
		{
			return get() == other.get();
		}

		template <typename U, ptr_qual UQualifiers>
		requires (std::is_assignable_v<T*, U*>)
		[[nodiscard]]
		_pure
		_nothrow bool operator==(const held_ptr<U, UQualifiers>& other) const noexcept
		{
			return get() == static_cast<T*>(other);
		}

		[[nodiscard]]
		_pure
		_nothrow bool operator!=(nullptr_t) const noexcept
		{
			return get() != nullptr;
		}

		[[nodiscard]]
		_pure
		_nothrow bool operator!=(const T* other) const noexcept
		{
			return get() != other;
		}

		template <typename U>
		requires (std::is_assignable_v<T*, U*>)
		[[nodiscard]]
		_pure
		_nothrow bool operator!=(const U* other) const noexcept
		{
			return get() != static_cast<T*>(other);
		}

		[[nodiscard]]
		_pure
		_nothrow bool operator!=(const held_ptr& other) const noexcept
		{
			return get() != other.get();
		}

		template <typename U, ptr_qual UQualifiers>
		requires (std::is_assignable_v<T*, U*>)
		[[nodiscard]]
		_pure
		_nothrow bool operator!=(const held_ptr<U, UQualifiers>& other) const noexcept
		{
			return get() != static_cast<T*>(other);
		}

		[[nodiscard]]
		_pure
		_nothrow explicit operator bool() const noexcept
		{
			return value_ != nullptr;
		}

		[[nodiscard]]
		_pure
		_nothrow bool is_valid() const noexcept
		{
			return static_cast<bool>(*this);
		}

		template <typename Self>
		[[nodiscard]]
		_pure
		_nothrow copy_qualifiers<Self, pointer_type> operator->(this Self&& self) noexcept //NOLINT(cppcoreguidelines-missing-std-forward)
		{
			return *std::forward<Self>(self);
		}

		template <typename Self>
		[[nodiscard]]
		_pure
		_nothrow copy_qualifiers<Self, pointer_type> get(this Self&& self) noexcept //NOLINT(cppcoreguidelines-missing-std-forward)
		{
			T* const result = std::forward<Self>(self).value_;
			return result;
		}

		template <typename Self>
		[[nodiscard]]
		_pure
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
	inline T* make_unique_inline(std::unique_ptr<T>& ptr, Tt&&... args) noexcept(std::is_nothrow_constructible_v<T, Tt...>)
	{
		ptr = std::make_unique<T>(std::forward<Tt>(args)...);
		return ptr.get();
	}

	// .second == true if created 
	template <typename T, typename... Tt>
	[[nodiscard]]
	inline std::tuple<T*, bool> get_or_make_unique_inline(std::unique_ptr<T>& ptr, Tt&&... args) noexcept(std::is_nothrow_constructible_v<T, Tt...>)
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

	template <typename T>
	requires (std::is_nothrow_convertible_v<T, bool>)
	[[nodiscard]]
	inline _nothrow T& value_or(T& value, T& else_value) noexcept
	{
		return static_cast<bool>(value) ?
			value :
			else_value;
	}

	template <typename T, typename... Tt>
	requires (
		std::is_nothrow_convertible_v<T, bool> &&
		std::is_nothrow_convertible_v<Tt..., T>
	)
	[[nodiscard]]
	_pure
	inline _nothrow T& value_or(T& value, T& else_value, Tt&&... else_values) noexcept
	{
		return static_cast<bool>(value) ?
			value :
			value_or(else_value, std::forward<Tt>(else_values)...);
	}

	template <typename T, typename TConverter>
	requires (
		std::is_nothrow_convertible_v<T, bool> &&
		std::is_nothrow_invocable_v<TConverter>
	)
	[[nodiscard]]
	inline _nothrow T& value_or(T& value, const TConverter& else_getter) noexcept
	{
		return static_cast<bool>(value) ?
			value :
			else_getter();
	}

	template<typename... Ts>
	struct overloads : Ts... { using Ts::operator()...; };

	#if 0
	template<typename TReturn, typename... Ts>
	struct overloads_no_monostate : Ts... {
		using Ts::operator()...;
		_forceinline
		_nothrow TReturn operator ()(std::monostate) noexcept { return {}; }
	};

	template<typename TReturn, typename... TFunc> overloads_no_monostate(TFunc...) -> overloads_no_monostate<TReturn, TFunc...>;
	#endif

	template <IntegralC TCompareType, IntegralC TInType>
	[[nodiscard]]
	inline constexpr _pure _forceinline _nothrow bool in_range(const TInType& value) noexcept
	{
		if constexpr (std::is_signed_v<TInType> && std::is_unsigned_v<TCompareType>)
		{
			return value >= 0 && TCompareType(value) <= std::numeric_limits<TCompareType>::max();
		}
		else if constexpr (std::is_unsigned_v<TInType> && std::is_signed_v<TCompareType>)
		{
			return value <= uint64(std::numeric_limits<TCompareType>::max());
		}

		return
			value >= std::numeric_limits<TCompareType>::lowest() &&
			value <= std::numeric_limits<TCompareType>::max();
	}

	template <NumericC TType>
	[[nodiscard]]
	inline constexpr _pure _forceinline _nothrow bool in_range(const TType& value, const TType min, const TType max) noexcept
	{
		return value >= min && value >= max;
	}

	template <typename TPointer>
	requires ((std::is_pointer_v<TPointer> || std::is_member_pointer_v<TPointer>) && sizeof(TPointer) <= sizeof(void*))
	[[nodiscard]]
	inline constexpr _func_const _forceinline _nothrow copy_qualifiers_ptr<TPointer, void> ptr_cast(const TPointer ptr) noexcept
	{
		return std::bit_cast<copy_qualifiers_ptr<TPointer, void>>(ptr);
	}

	template <typename TPointer>
	requires ((std::is_pointer_v<TPointer> || std::is_member_pointer_v<TPointer>) && sizeof(TPointer) <= sizeof(void*))
	[[nodiscard]]
	inline constexpr _func_const _forceinline _nothrow copy_qualifiers<TPointer, uintptr> uptr_cast(const TPointer ptr) noexcept
	{
		return std::bit_cast<copy_qualifiers<TPointer, uintptr>>(ptr);
	}

	template <size_t Value>
	using uint_fitted = std::conditional_t<
		in_range<uint8>(Value),
		uint8,
		std::conditional_t<
			in_range<uint16>(Value),
			uint16,
			std::conditional_t<
				in_range<uint32>(Value),
				uint32,
				std::conditional_t<
					in_range<uint64>(Value),
					uint64,
					void
				>
			>
		>
	>;

	template <ptrdiff_t Value>
	using int_fitted = std::conditional_t<
		in_range<int8>(Value),
		int8,
		std::conditional_t<
			in_range<int16>(Value),
			int16,
			std::conditional_t<
				in_range<int32>(Value),
				int32,
				std::conditional_t<
					in_range<int64>(Value),
					int64,
					void
				>
			>
		>
	>;

	template <std::integral TOut, std::integral TIn>
	[[nodiscard]]
	static constexpr _func_const _nothrow _forceinline TOut checked_cast(const TIn value) noexcept
	{
		using in_decay_t = std::remove_cvref_t<TIn>;
		using out_decay_t = std::remove_cvref_t<TOut>;

		if constexpr (std::is_same_v<in_decay_t, out_decay_t>)
		{
			return value;
		}
		else if constexpr(
			std::numeric_limits<in_decay_t>::lowest() >= std::numeric_limits<out_decay_t>::lowest() &&
			std::numeric_limits<in_decay_t>::max() >= std::numeric_limits<out_decay_t>::max()
		)
		{
			return static_cast<TOut>(value);
		}
		else
		{
			xassert(
				value >= std::numeric_limits<out_decay_t>::lowest() &&
				value <= std::numeric_limits<out_decay_t>::max()
			);
			return static_cast<TOut>(value);
		}
	}

	template <std::integral TIn> requires std::is_integral_v<TIn>
	[[nodiscard]]
	static constexpr _func_const _nothrow _forceinline std::make_unsigned_t<TIn> unsigned_cast(const TIn value) noexcept
	{
		if constexpr (std::is_signed_v<TIn>)
		{
			xassert(value >= 0);
		}
		return static_cast<std::make_unsigned_t<TIn>>(value);
	}

	template <std::integral TIn> requires std::is_integral_v<TIn>
	[[nodiscard]]
	static constexpr _func_const _nothrow _forceinline std::make_signed_t<TIn> signed_cast(const TIn value) noexcept
	{
		if constexpr (std::is_unsigned_v<TIn>)
		{
			xassert(value <= std::numeric_limits<std::make_signed_t<TIn>>::max());
		}
		return static_cast<std::make_signed_t<TIn>>(value);
	}

	template <std::integral T, std::integral AlignT> requires std::is_unsigned_v<AlignT>
	[[nodiscard]]
	static constexpr _func_const _nothrow T align_up_pow2(T value, AlignT align) noexcept {
		xassert(std::has_single_bit(align));
		
		const AlignT align_mask = align - 1;

		return (value + align_mask) & ~align_mask;
	}

	template <std::integral T, std::integral AlignT> requires std::is_unsigned_v<AlignT>
	[[nodiscard]]
	static constexpr _func_const _nothrow T align_pow2(T value, AlignT align) noexcept {
		xassert(std::has_single_bit(align));

		const AlignT align_mask = align - 1;

		if constexpr (std::is_signed_v<T>)
		{
			if (value >= 0) {
				return (value + align_mask) & ~align_mask;
			}
			else {
				return (value - align_mask) & ~align_mask;
			}
		}
		else
		{
			return (value + align_mask) & ~align_mask;
		}
	}

	template <size_t Align, std::integral ValueT>
	[[nodiscard]]
	static constexpr _func_const _nothrow auto align_up(const ValueT value) noexcept {
		if constexpr (std::has_single_bit(Align))
		{
			const ValueT align_mask = Align - 1;

			return (value + align_mask) & ~align_mask;
		}
		else
		{
			return value + (Align - (value % Align));
		}
	}

	template <size_t Align, std::integral ValueT>
	[[nodiscard]]
	static constexpr _func_const _nothrow auto align(const ValueT value) noexcept {
		if constexpr (std::has_single_bit(Align))
		{
			const ValueT align_mask = Align - 1;

			if constexpr (std::is_signed_v<ValueT>)
			{
				if (value >= 0) {
					return (value + align_mask) & ~align_mask;
				}
				else {
					return (value - align_mask) & ~align_mask;
				}
			}
			else
			{
				return (value + align_mask) & ~align_mask;
			}
		}
		else
		{
			if constexpr (std::is_signed_v<ValueT>)
			{
				if (value >= 0)
				{
					return value + (Align - (value % Align));
				}
				else
				{
					return value - (Align + (value % -Align));
				}
			}
			else
			{
				return value + (Align - (value % Align));
			}
		}
	}

	template <size_t Align, std::integral ValueT>
	[[nodiscard]]
	static constexpr _func_const _nothrow auto align_mask(const ValueT value) noexcept
	{
		const ValueT align_mask = Align - 1;
		return value & ~align_mask;
	}
}

#define _make_qual(type) copy_qualifiers<decltype(self), type>

#if _WIN32
#	define SAL_Ret_notnull_ _Ret_notnull_
#else
#	define SAL_Ret_notnull_
#endif
