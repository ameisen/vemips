#include "pch.hpp"

#include "platform_win32.hpp"


namespace mips
{
	platform::host_features_return_type platform::get_host_features()
	{
		static const host_features features = []
		{
			host_features result;

			int cpuid_registers[4] = {};
			__cpuidex(cpuid_registers, 0x07, 0x0);
			result.bmi1 = cpuid_registers[1] & (1 << 3);
			result.bmi2 = cpuid_registers[1] & (1 << 8);

			return result;
		}();

		return features;
	}
}
