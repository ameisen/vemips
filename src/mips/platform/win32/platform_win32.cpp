#include "pch.hpp"

#include "platform_win32.hpp"

#include <common.hpp>

#include <intrin.h>


namespace mips
{
	namespace
	{
		struct alignas(uint32) processor_info final
		{
			uint32 stepping : 4;
			uint32 model : 4;
			uint32 family : 4;
			uint32 type : 2;
			uint32 : 2;
			uint32 extended_model : 4;
			uint32 extended_family : 8;
			uint32 : 4;

			[[nodiscard]]
			constexpr uint8 get_model() const
			{
				if (family == 0x6 || family == 0xF)
				{
					return model + uint8(extended_model << 4);
				}

				return model;
			}

			[[nodiscard]]
			constexpr uint16 get_family() const
			{
				if (family == 0xF)
				{
					return 0xF + extended_family;
				}

				return family;
			}
		};
		static_assert(sizeof(processor_info) == sizeof(uint32));

		struct amd_family final
		{
			amd_family() = delete;
			enum value : uint16  // NOLINT(performance-enum-size)
			{
				zen1_2 =       0x17,
				hygon_dhyana = 0x18,
				zen3_4 =       0x19,
				zen5_6 =       0x1A,
			};
		};

		static constexpr int32 fourcc(const char(&str)[5])
		{
			return
				str[0] |
				str[1] << 8  |
				str[2] << 16 |
				str[3] << 24;
		}

		static constexpr int32 fourcc(const std::string_view str)
		{
			return
				str[0] |
				str[1] << 8  |
				str[2] << 16 |
				str[3] << 24;
		}

		static constexpr std::array<int32, 3> fourcc(const char(&str)[(4 * 3) + 1])
		{
			return {
				fourcc(std::string_view{str + 0, 4}),
				fourcc(std::string_view{str + 4, 4}),
				fourcc(std::string_view{str + 8, 4})
			};
		}
	}

	_nothrow platform::host_features_return_type platform::get_host_features() noexcept
	{
		static const host_features features = []
		{
			host_features result;

			int32 cpuid_registers[4] = {};
			__cpuidex(cpuid_registers, 0x07, 0x0);
			result.bmi1 = cpuid_registers[1] & (1 << 3);
			result.bmi2 = cpuid_registers[1] & (1 << 8);

			__cpuidex(cpuid_registers, 0x8000'0001, 0x0);
			result.lzcnt = cpuid_registers[2] & (1 << 5);

			// xchg tests
			{
				/*
				// XCHG

				// RR 8/16
				// ARCH     = OPS : LATENCY : RTHROUGHPUT
				// INTEL
				// Ice/Raptor =   3 :       3 : 1/2
				// AMD
				// K8         =   3 :       2 : 1
				// ...        =   2 :       1 : 1
				// Excv       =   2 :       1 : 1
				// Zen1       =   2 :       1 : 1
				// Zen2       =   2 :       1 : 1
				// Zen3       =   2 :       1 : 1
				// Zen4       =   2 :       1 : 1
				// Zen5       =   2 :       1 : 1

				// RR 32/64
				// ARCH     = OPS : LATENCY : RTHROUGHPUT
				// INTEL
				// Ice/Raptor =   3 :       3 : 1/2
				// AMD
				// K8         =   3 :       2 : 1
				// ...        =   2 :       1 : 1
				// Excv       =   2 :       1 :  0.5
				// Zen1       =   2 :       0 : 0.33
				// Zen2       =   2 :       0 : 0.33
				// Zen3       =   2 :       0 : 0.33
				// Zen4       =   2 :       0 : 0.33
				// Zen5       =   2 :       0 : 0.25
			
				// RM
				// ARCH OPS : LATENCY : RTHROUGHPUT
				// INTEL
				// Ice/Raptor =   8 :       8 :  19
				// AMD
				// Excv       =   2 :      50 :  50
				// Zen1       =   2 :      30 :  30
				// Zen2       =   2 :      18 :  18
				// Zen3       =   2 :     7/8 : 7/8
				// Zen4       =   2 :     7/8 : 7/8
				// Zen5       =   2 :      20 :  20

				// MOV

				// RR 8/16
				// ARCH     = OPS : LATENCY : RTHROUGHPUT
				// INTEL
				// Ice/Raptor =   1 :       2 : 0.25 // 8| 3 : 6 : 0.083
				// AMD
				// K8         =   1 :       1 : 0.3_     | 3 : 3 : 0.111
				// ...
				// Excv       =   1 :       1 : 0.5		 | 3 : 3 : 0.166
				// Zen1       =   1 :       1 : 0.25-0.3 | 3 : 3 : 0.083 - 0.111
				// Zen2       =   1 :       1 : 0.3		 | 3 : 3 : 0.111
				// Zen3       =   1 :       1 : 0.25-0.5 | 3 : 3 : 0.083 - 0.166
				// Zen4       =   1 :       1 : 0.25	 | 3 : 3 : 0.083
				// Zen5       =   1 :       1 : 0.2/0.5	 | 3 : 3 : 0.2   - 0.166

				// RR 32/64
				// ARCH     = OPS : LATENCY : RTHROUGHPUT
				// INTEL
				// Ice/Raptor =   1 :       1 : 0.25     | 3 : 3 : 0.083
				// AMD
				// K8         =   1 :       1 : 0.3_     | 3 : 3 : 0.111
				// ...
				// Excv       =   1 :       1 : 0.25	 | 3 : 3 : 0.083
				// Zen1       =   1 :       0 : 0.2-0.3	 | 3 : 0 : 0.2   - 0.111
				// Zen2       =   1 :       0 : 0.25	 | 3 : 0 : 0.083
				// Zen3       =   1 :       0 : 0.17	 | 3 : 0 : 0.057
				// Zen4       =   1 :       0 : 0.17	 | 3 : 0 : 0.057
				// Zen5       =   1 :       0 : 0.17	 | 3 : 0 : 0.057
			
				// RM
				// ARCH OPS : LATENCY : RTHROUGHPUT
				// INTEL
				// Ice/Raptor =   1-2 :     3 : 0.5		 | 3-5 : 7   : 0.125
				// AMD
				// K8         =   1 :     3-4 : 0.5		 | 3   : 7-9 : 0.143
				// ...
				// Excv       =   1 :     3-4 : 0.5 -  1 | 3   : 7-9 : 0.167 - 0.125
				// Zen1       =   1 :     3-4 : 0.5 -  1 | 3   : 6-8 : 0.111 - 0.188
				// Zen2       =   1 :     0-4 : 0.5 -  1 | 3   : 0-8 : 0.125 - 0.167
				// Zen3       =   1 :     0-3 : 0.33-0.5 | 3   : 0-6 : 0.084 - 0.101
				// Zen4       =   1 :     0-4 : 0.33-0.5 | 3   : 0-8 : 0.084 - 0.101
				// Zen5       =   1 :     0-4 : 0.25-0.5 | 3   : 0-8 : 0.072 - 0.101
				
				These cannot be interpreted directly, though.
				In an exchange, two of the MOVs could be executed in parallel.

				xchg should probably only be used on AMD Zen1 and up chips.
				*/

				__cpuid(cpuid_registers, 0x00);

				if (
					const std::array registers {
						cpuid_registers[1],
						cpuid_registers[3],
						cpuid_registers[2]
					};
					fourcc("AuthenticAMD") == registers ||
					fourcc("HygonGenuine") == registers
				)
				{
					__cpuid(cpuid_registers, 0x01);

					// We can filter further by using model, but that's not necessary right now.

					const auto info = std::bit_cast<processor_info>(cpuid_registers[0]);

					const auto family = info.get_family();
					if (info.get_family() >= amd_family::zen1_2)
					{
						result.fast_xchg_rr8_16 = true;
						result.fast_xchg_rr32_64 = true;
						result.fast_xchg_rm = true;
					}
				}
			}

			return result;
		}();

		return features;
	}
}
