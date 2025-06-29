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
		virtual ~memory_source() = default;

		virtual bool is_readable(uint32 offset) const __restrict = 0;
		virtual bool is_writable(uint32 offset) const __restrict = 0;
		virtual const void * at(uint32 offset, uint32 size) const __restrict = 0;
		virtual const void * at_exec(uint32 offset, uint32 size) const __restrict = 0;
		virtual void * write_at(uint32 offset, uint32 size) __restrict = 0;

		virtual void register_processor (processor * __restrict cpu) __restrict = 0;
		virtual void unregister_processor (processor * __restrict cpu) __restrict = 0;

		virtual void * get_ptr() = 0;
		virtual const void * get_ptr() const = 0;
		virtual uint32 get_size() const = 0;

		virtual void set_executable_memory(const elf::binary & __restrict binary) __restrict = 0;

		virtual std::optional<uint32> get_first_unreadable(uint32 offset, uint32 size) const __restrict;
		virtual std::optional<uint32> get_first_unwritable(uint32 offset, uint32 size) const __restrict;
	};
}
