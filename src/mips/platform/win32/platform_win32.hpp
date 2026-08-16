#pragma once

#include "config.hpp"
#include <common.hpp>

#include <type_traits>


// TODO : specific to x86
namespace mips::platform
{
	struct host_features final
	{
		bool bmi1 : 1 = false;
		bool bmi2 : 1 = false;
		bool lzcnt : 1 = false;

		bool fast_xchg_rr8_16 : 1 = false;
		bool fast_xchg_rr32_64 : 1 = false;
		bool fast_xchg_rm : 1 = false;
	};
	using host_features_return_type = std::conditional_t<
		(sizeof(host_features) > 8),
		const host_features&,
		host_features
	>;

	[[nodiscard]]
	_func_const
	_nothrow host_features_return_type get_host_features() noexcept;

#if VEMIPS_TABLEGEN
	[[nodiscard]]
	_func_const
	inline _nothrow host_features_return_type get_host_features() noexcept { return {}; }
#endif
}
