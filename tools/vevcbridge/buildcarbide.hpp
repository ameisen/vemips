#pragma once

#include <array>
#include <string>
#include <string_view>

#include <cassert>
#include <codecvt>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#if _DEBUG
	// ReSharper disable once CppInconsistentNaming
#	define check(condition) ([&]() noexcept { const bool result_condition = bool(condition); assert(result_condition && #condition); return result_condition; }())
#else
	// ReSharper disable once CppInconsistentNaming
#	define check(condition) ([&]() noexcept { const bool result_condition = bool(condition); __assume(result_condition); return result_condition; }()) // NOLINT(clang-diagnostic-assume)
#endif

#include "platform/platform.hpp"
#include "libfmt.hpp"

namespace buildcarbide {
	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;
	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;

	using std::size_t;
	using std::wstring_view;
	using std::wstring;
	using std::forward;

	static inline std::string to_string(const std::wstring & __restrict str) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(str);
	}

	static inline std::string to_string(const wchar_t * __restrict str) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(str);
	}

	static inline std::string to_string(const std::wstring_view str) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(std::addressof(*str.begin()), std::addressof(*str.end()));
	}

	static inline std::wstring to_wstring(const std::string & __restrict str) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(str);
	}

	static inline std::wstring to_wstring(const char * __restrict str) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(str);
	}

	static inline std::wstring to_wstring(const std::string_view str) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(std::addressof(*str.begin()), std::addressof(*str.end()));
	}

	template<class T>
	concept integral = std::is_integral_v<T>;

	namespace detail {
		struct static_tag {
			static_tag() = delete;
		};

		template <integral Th, Th OffsetBasis, Th Prime>
		struct fnv_base : static_tag {
		protected:
			template <integral To> requires (sizeof(To) == 1)
			using octet_t = std::conditional_t<
				std::is_signed_v<Th>,
				std::make_signed_t<To>,
				std::make_unsigned_t<To>
			>;

			using type = Th;

		public:
			static constexpr const type offset_basis = OffsetBasis;
			static constexpr const type prime = Prime;
		};

		template <integral Th, Th OffsetBasis, Th Prime>
		struct fnv1_a_base : public fnv_base<Th, OffsetBasis, Prime> {
			using fnv_base<Th, OffsetBasis, Prime>::octet_t;

			template <integral To> requires (sizeof(To) == 1)
			static __forceinline constexpr no_throw Th iteration(const Th hash, const To octet) noexcept {
				return (hash ^ octet_t<To>(octet)) * Prime;
			}

			template <integral TElement> requires(std::is_trivially_copyable_v<TElement> && sizeof(TElement) != 1)
			static __forceinline constexpr no_throw Th iteration(Th hash, TElement element) noexcept {
				for (
					const auto element_octets = std::bit_cast<std::array<uint8, sizeof(TElement)>>(element);
					const auto octet : element_octets
				) {
					hash = iteration(hash, octet);
				}
				return hash;
			}
		};

		template <integral T> struct fnv1_a;
		template <> struct fnv1_a<uint32> final : fnv1_a_base<uint32, 0x811C9DC5u, 0x01000193u> {};
		template <> struct fnv1_a<uint64> final : fnv1_a_base<uint64, 0xCBF29CE484222325ull, 0x00000100000001B3ull> {};

	}

	template <integral T = uint64>
	static inline constexpr no_throw T hash(const char * __restrict value) noexcept {
		using fnv = detail::fnv1_a<T>;

		T hash = fnv::offset_basis;

		char c;
		while ((c = *value++) != '\0') {
			hash = fnv::iteration(hash, c);
		}

		return hash;
	}

	template <integral T = uint64>
	static inline constexpr no_throw T hash(const char * __restrict value, const size_t length) noexcept {
		using fnv = detail::fnv1_a<T>;

		T hash = fnv::offset_basis;

		for (size_t i = 0; i < length; ++i) {
			hash = fnv::iteration(hash, *value++);
		}

		return hash;
	}

	template <integral T = uint64>
	static inline constexpr no_throw T hash(const std::string_view value) noexcept {
		return hash(value.data(), value.length());
	}

	template <integral T = uint64>
	static inline constexpr no_throw T hash(const wchar_t * __restrict value) noexcept {
		using fnv = detail::fnv1_a<T>;

		T hash = fnv::offset_basis;

		wchar_t c;
		while ((c = *value++) != L'\0') {
			hash = fnv::iteration(hash, c);
		}

		return hash;
	}

	template <integral T = uint64>
	static inline constexpr no_throw T hash(const wchar_t * __restrict value, const size_t length) noexcept {
		using fnv = detail::fnv1_a<T>;

		T hash = fnv::offset_basis;

		for (size_t i = 0; i < length; ++i) {
			hash = fnv::iteration(hash, *value++);
		}

		return hash;
	}

	template <integral T = uint64>
	static inline constexpr no_throw T hash(const wstring_view value) noexcept {
		return hash(value.data(), value.length());
	}

	template <typename T, size_t N>
	static constexpr no_throw size_t extent_of(const T (& __restrict)[N]) noexcept {
		return N;
	}
}

