#include "pch.hpp"

#include "string_utils.hpp"
#include <common.hpp>

#include <cstdlib>
#if _MSC_VER
#	include <cwctype>
#endif
#include <ranges>
#include <type_traits>


namespace vemips::string_utils
{
	static constexpr bool wide_tchar = std::is_same_v<tchar, wchar_t>;

	[[nodiscard]]
	extern _pure _nothrow long long string_to_ll(const tchar* __restrict str, const int base) noexcept
	{
#if UNICODE
		return std::wcstoll(str, nullptr, base);
#else
		return std::strtoll(str, nullptr, base);
#endif
	}

	[[nodiscard]]
	extern _pure _nothrow unsigned long long string_to_ull(const tchar* __restrict str, const int base) noexcept
	{
#if UNICODE
		return std::wcstoull(str, nullptr, base);
#else
		return std::strtoull(str, nullptr, base);
#endif
	}

	[[nodiscard]]
	extern _func_const _nothrow tchar to_lower_char(const tchar c) noexcept
	{
#if UNICODE
		return static_cast<tchar>(std::towlower(c));
#else
		return static_cast<tchar>(std::tolower(c));
#endif
	}

	[[nodiscard]]
	extern _pure _nothrow bool insensitive_equals(const tstring_view a, const tstring_view b) noexcept
	{
		return std::ranges::equal(
			a,
			b,
			[] _forceinline_lambda((const auto& ac, const auto& bc) noexcept)
			{
				return to_lower_char(ac) == to_lower_char(bc);
			}
		);
	}

	[[nodiscard]]
	extern _pure _nothrow std::optional<bool> parse_value_bool(const std::optional<tstring_view>& value) noexcept
	{
		if (!value)
		{
			return {};
		}

		if (
			insensitive_equals(*value, TSTR("off")) ||
			insensitive_equals(*value, TSTR("no")) ||
			insensitive_equals(*value, TSTR("false")) ||
			insensitive_equals(*value, TSTR("disable")) ||
			insensitive_equals(*value, TSTR("disabled")) ||
			insensitive_equals(*value, TSTR("-")) ||
			insensitive_equals(*value, TSTR("0"))
		)
		{
			return false;
		}
		else if (
			insensitive_equals(*value, TSTR("on")) ||
			insensitive_equals(*value, TSTR("yes")) ||
			insensitive_equals(*value, TSTR("true")) ||
			insensitive_equals(*value, TSTR("enable")) ||
			insensitive_equals(*value, TSTR("enabled")) ||
			insensitive_equals(*value, TSTR("+")) ||
			insensitive_equals(*value, TSTR("1"))
		)
		{
			return true;
		}

		return {};
	}
}
