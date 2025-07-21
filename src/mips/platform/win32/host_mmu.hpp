#pragma once

#include <common.hpp>

namespace mips::platform {
	class host_mmu final {
		void *m_pointer = nullptr;
		void *m_shadow_pointer = nullptr;
	public:
		host_mmu(bool with_shadow, uint32 total_memory, uint32 stack_memory);
		~host_mmu();

		void * get_pointer() const __restrict {
			return m_pointer;
		}

		void * get_shadow_pointer() const __restrict {
			return m_shadow_pointer;
		}
	};
}
