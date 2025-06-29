#include "pch.hpp"

#include "host_mmu.hpp"

#define WIN32_LEAN_AND_MEAN  // NOLINT(clang-diagnostic-unused-macros)
#include <Windows.h>

using namespace mips::platform;

host_mmu::host_mmu(uint32 total_memory, uint32 stack_memory) {
	const uint32 low_memory = total_memory - stack_memory;
	const uint32 high_memory = stack_memory;

	m_pointer = VirtualAlloc(nullptr, 1ull << 32, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!m_pointer) {
		throw std::exception("Failed to allocate virtual address space for VM");
	}

	// Do _not_ commit the first page, since we want nullptr to fail.
	void *res = VirtualAlloc((char *)m_pointer + 0x1000, low_memory, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!res) {
		VirtualFree(m_pointer, 0, MEM_RELEASE);
		m_pointer = nullptr;
		throw std::exception("Failed to allocate virtual address space for VM");
	}

	if (high_memory) {
		const uint32 high_start = uint32(0x100000000ull - high_memory);
		res = VirtualAlloc(((char *)m_pointer) + high_start, high_memory, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (!res) {
			VirtualFree(m_pointer, 0, MEM_RELEASE);
			m_pointer = nullptr;
			throw std::exception("Failed to allocate virtual address space for VM");
		}
	}
}

host_mmu::~host_mmu() {
	if (m_pointer) {
		VirtualFree(m_pointer, 0, MEM_RELEASE);
	}
}
