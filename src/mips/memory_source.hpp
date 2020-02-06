#pragma once

#include "common.hpp"

namespace elf {
	class binary;
}

namespace mips {
	class processor;
	class memory_source {
	public:
		memory_source() = default;
		virtual ~memory_source() {}

		virtual const void * __restrict at(uint32 offset, uint32 size) const __restrict = 0;
		virtual const void * __restrict at_exec(uint32 offset, uint32 size) const __restrict = 0;
		virtual void * __restrict write_at(uint32 offset, uint32 size) __restrict = 0;

		virtual void register_processor (processor * __restrict cpu) __restrict = 0;
		virtual void unregister_processor (processor * __restrict cpu) __restrict = 0;

		virtual void * get_ptr() __restrict = 0;
		virtual uint32 get_size() __restrict = 0;

		virtual void set_executable_memory(const elf::binary & __restrict binary) __restrict = 0;
	};
}
