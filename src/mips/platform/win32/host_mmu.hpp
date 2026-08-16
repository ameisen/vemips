#pragma once

#include <common.hpp>


namespace mips::platform {
	class host_mmu final {
		void* m_pointer = nullptr;
		void* m_shadow_pointer = nullptr;
	public:
		host_mmu(bool with_shadow, usize_guest total_memory, usize_guest stack_memory);
		~host_mmu();

		[[nodiscard]]
		_pure
		_nothrow void* get_pointer() const __restrict noexcept {
			return m_pointer;
		}

		[[nodiscard]]
		_pure
		_nothrow void* get_shadow_pointer() const __restrict noexcept {
			return m_shadow_pointer;
		}
	};
}
