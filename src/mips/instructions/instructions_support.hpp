#pragma once

#include <limits>

#include <common.hpp>


namespace mips
{
	enum class memory_hazards : uint32
	{
		none = 0,
		await_load = (1U << 0),
		await_store = (1U << 1),
		order_load = (1U << 2),
		order_store = (1U << 3),
		global_barrier_load = (1U << 4),
		global_barrier_store = (1U << 5),
		global_barrier_GINVI = (1U << 6),
		global_barrier_GINVT = (1U << 7),
		global_barrier_SYNCI = (1U << 8),

		all_data = await_load | await_store | order_load | order_store | global_barrier_load | global_barrier_store,
		all = std::numeric_limits<uint32>::max()
	};

	MAKE_BITFLAG_ENUM(mips::memory_hazards)

	namespace instructions
	{
		memory_hazards parse_sync_type(uint8 type);
	}
}
