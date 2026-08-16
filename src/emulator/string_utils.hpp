#pragma once

#include <common.hpp>

#include <optional>
#if _MSC_VER
#	include <tchar.h>
#endif // _MSC_VER

namespace vemips::string_utils
{
#if _MSC_VER
	using tchar = TCHAR;
#else
	using tchar = char;
#endif

	[[nodiscard]]
	extern _pure _nothrow long long string_to_ll(const tchar* __restrict str, int base = 10) noexcept;

	[[nodiscard]]
	extern _pure _nothrow unsigned long long string_to_ull(const tchar* __restrict str, int base = 10) noexcept;

	[[nodiscard]]
	extern _func_const _nothrow tchar to_lower_char(tchar c) noexcept;

	[[nodiscard]]
	extern _pure _nothrow bool insensitive_equals(tstring_view a, tstring_view b) noexcept;

	[[nodiscard]]
	extern _pure _nothrow std::optional<bool> parse_value_bool(const std::optional<tstring_view>& value) noexcept;
}
