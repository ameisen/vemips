#include "pch.hpp"
#include "instructions_support.hpp"


namespace mips
{
	memory_hazards instructions::parse_sync_type(const uint8 type)
	{
		xassert(type <= 0b11111);

		switch (type)
		{
			case 0x0: // SYNC
				// Awaits Loads, Stores
				// Orders Loads, Stores
				// Global Barrier for Loads, Stores
				return
					memory_hazards::await_load | memory_hazards::await_store |
					memory_hazards::order_load | memory_hazards::order_store |
					memory_hazards::global_barrier_load | memory_hazards::global_barrier_store;
			case 0x4: // SYNC_WMB
				// Awaits Stores
				// Orders Stores
				return
					memory_hazards::await_store |
					memory_hazards::order_store;
			case 0x10: // SYNC_MB
				// Awaits Loads, Stores
				// Orders Loads, Stores
				return
					memory_hazards::await_load | memory_hazards::await_store |
					memory_hazards::order_load | memory_hazards::order_store;
			case 0x11: // SYNC_ACQUIRE
				// Awaits Loads
				// Orders Loads, Stores
				return
					memory_hazards::await_load |
					memory_hazards::order_load | memory_hazards::order_store;
			case 0x12: // SYNC_RELEASE
				// Awaits Loads, Stores
				// Orders Stores
				return
					memory_hazards::await_load | memory_hazards::await_store |
					memory_hazards::order_store;
			case 0x13: // SYNC_RMB
				// Awaits Loads
				// Orders Loads
				return
					memory_hazards::await_load |
					memory_hazards::order_load;
			case 0x14: // SYNC_GINV
				// Awaits Loads, Stores
				// Orders Loads, Stores
				// Global Barrier for GINVI, GINVT, and SYNCI (TODO : important)
				return
					memory_hazards::await_load | memory_hazards::await_store |
					memory_hazards::order_load | memory_hazards::order_store |
					memory_hazards::global_barrier_GINVI | memory_hazards::global_barrier_GINVT | memory_hazards::global_barrier_SYNCI;
			default:
				// nothing, reserved or implementation-defined
				return memory_hazards::none;
		}
	}
}
