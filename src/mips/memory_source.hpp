#pragma once

#include "common.hpp"

#include <optional>


namespace mips {
	class processor;

	namespace elf
	{
		class binary;
	}

	class memory_source {
	public:
		memory_source() = default;
		virtual ~memory_source() = default;

		[[nodiscard]]
		virtual _pure _nothrow bool is_readable(uptr_guest offset) const __restrict noexcept = 0;
		[[nodiscard]]
		virtual _pure _nothrow bool is_writable(uptr_guest offset) const __restrict noexcept = 0;
		[[nodiscard]]
		virtual _pure _nothrow const void* at(uptr_guest offset, usize_guest size) const __restrict noexcept = 0;
		[[nodiscard]]
		virtual _pure _nothrow const void* at_instruction(uptr_guest offset, usize_guest size) const __restrict noexcept = 0;
		[[nodiscard]]
		virtual _pure _nothrow const void* at_exec(uptr_guest offset, usize_guest size) const __restrict noexcept = 0;
		[[nodiscard]]
		virtual _pure _nothrow void* write_at(uptr_guest offset, usize_guest size) __restrict noexcept = 0;

		virtual _nothrow void register_processor (processor* __restrict cpu) __restrict noexcept = 0;
		virtual _nothrow void unregister_processor (processor* __restrict cpu) __restrict noexcept = 0;

		[[nodiscard]]
		virtual _pure _nothrow void* get_ptr() noexcept = 0;
		[[nodiscard]]
		virtual _pure _nothrow const void* get_ptr() const noexcept = 0;
		[[nodiscard]]
		virtual _pure _nothrow void* get_shadow_ptr() noexcept = 0;
		[[nodiscard]]
		virtual _pure _nothrow const void* get_shadow_ptr() const noexcept = 0;
		[[nodiscard]]
		virtual _pure _nothrow usize_guest get_size() const noexcept = 0;

		virtual _nothrow void set_executable_memory(const elf::binary& __restrict binary) __restrict noexcept = 0;

		[[nodiscard]]
		virtual _pure _nothrow std::optional<uptr_guest> get_first_unreadable(
			uptr_guest offset,
			usize_guest size
		) const __restrict noexcept;
		[[nodiscard]]
		virtual _pure _nothrow std::optional<uptr_guest> get_first_unwritable(
			uptr_guest offset,
			usize_guest size
		) const __restrict noexcept;
	};
}
