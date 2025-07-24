#pragma once


// TODO : specific to x86
namespace mips::platform
{
	struct host_features final
	{
		bool bmi1 : 1 = false;
		bool bmi2 : 1 = false;
	};
	using host_features_return_type = std::conditional_t<
		(sizeof(host_features) > 8),
		const host_features&,
		host_features
	>;

	host_features_return_type get_host_features();

#if VEMIPS_TABLEGEN
	inline host_features_return_type get_host_features() { return {}; }
#endif
}
