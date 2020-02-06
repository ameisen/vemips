#pragma once

#include <common.hpp>

namespace mips::platform {
	class host_mmu {
		void *m_pointer = nullptr;
	public:
		host_mmu(uint32 total_memory, uint32 stack_memory);
		~host_mmu();

		void * get_pointer() const __restrict {
			return m_pointer;
		}
	};
}
