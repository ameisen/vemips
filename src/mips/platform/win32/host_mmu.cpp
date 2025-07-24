#include "pch.hpp"

#include "host_mmu.hpp"

#include "platform/platform_headers.hpp"


using namespace mips::platform;

host_mmu::host_mmu(bool with_shadow, uint32 total_memory, uint32 stack_memory) {
	const uint32 low_memory = total_memory - stack_memory;
	const uint32 high_memory = stack_memory;

	const auto get_memory = [&] {
		void* pointer = VirtualAlloc(nullptr, 1ull << 32, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!pointer) {
			throw std::exception("Failed to allocate virtual address space for VM");
		}

		// Do _not_ commit the first page, since we want nullptr to fail.
		void *res = VirtualAlloc((char *)pointer + 0x1000, low_memory, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!res) {
			VirtualFree(pointer, 0, MEM_RELEASE);
			pointer = nullptr;
			throw std::exception("Failed to allocate virtual address space for VM");
		}

		if (high_memory) {
			const uint32 high_start = uint32(0x100000000ull - high_memory);
			res = VirtualAlloc(((char *)pointer) + high_start, high_memory, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			if (!res) {
				VirtualFree(pointer, 0, MEM_RELEASE);
				pointer = nullptr;
				throw std::exception("Failed to allocate virtual address space for VM");
			}
		}

		return pointer;
	};

	m_pointer = get_memory();
	if (with_shadow)
	{
		m_shadow_pointer = get_memory();
	}
}

host_mmu::~host_mmu() {
	if (m_pointer) {
		VirtualFree(m_pointer, 0, MEM_RELEASE);
	}
	if (m_shadow_pointer)
	{
		VirtualFree(m_shadow_pointer, 0, MEM_RELEASE);
	}
}
