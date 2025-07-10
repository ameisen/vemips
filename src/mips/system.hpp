#pragma once

#include <common.hpp>
#include "mips/mips.hpp"
#include "elf/elf.hpp"
#include "mips/mmu.hpp"
#include "mips/platform/platform.hpp"
#include "mips/debugger.hpp"

namespace mips {
	class debugger;
	class processor;
	class system {
	protected:
		friend class processor;

		processor * __restrict processor_ = nullptr;
		memory_source * __restrict memory_source_ = nullptr;
		std::vector<char> memory_;
		platform::host_mmu *host_mmu_ = nullptr;
		debugger* debugger_ = nullptr;
		uint32 system_break_ = 0;

		// Termination
		bool execution_complete_ : 1 = false;
		bool execution_success_ : 1 = false;

	public:
		struct options final {
			uint32 total_memory = 0;
			uint32 stack_memory = 0; // 0 = no stack reservation, unified memory space
			mips::JitType jit_type = JitType::None;
			mmu mmu_type = mmu::emulated;
			uint16 debug_port = 0;
			bool read_only_exec : 1 = false;
			bool collect_statistics : 1 = false;
			bool disable_cti : 1 = false;
			bool ticked : 1 = false;
			bool instruction_cache : 1 = false;
			bool debug : 1 = false;
			bool debug_owned : 1 = false;

			void validate() const;
		};

		struct capabilities final
		{
			bool can_handle_syscalls_inline : 1 = false;
		};
	protected:
		const options options_;
		const capabilities capabilities_;
			
		void initialize(const elf::binary & __restrict binary);

		system(capabilities&& capabilities, const options & __restrict init_options, const elf::binary & __restrict binary);
	public:
		system(const options & __restrict init_options, const elf::binary & __restrict binary);
		virtual ~system();

		virtual void clock(uint64 clocks) __restrict;

		[[nodiscard]]
		_nothrow const capabilities& get_capabilities() const __restrict noexcept
		{
			return capabilities_;
		}

		[[nodiscard]]
		uint64 get_instruction_count() const __restrict;

		[[nodiscard]]
		const processor::statistics* get_statistics() const __restrict;

		[[nodiscard]]
		size_t get_jit_max_instruction_size() const __restrict;

		virtual uint32 handle_exception(const CPU_Exception & __restrict ex) __restrict = 0;

		[[nodiscard]]
		bool is_execution_complete() const __restrict {
			return execution_complete_;
		}

		[[nodiscard]]
		bool is_execution_success() const __restrict {
			return execution_success_;
		}

		[[nodiscard]]
		bool is_debugger_owned() const __restrict {
			return options_.debug_owned;
		}

		[[nodiscard]]
		processor * get_processor() const __restrict {
			return processor_;
		}

		[[nodiscard]]
		debugger * get_debugger() const __restrict {
			return debugger_;
		}
	};
}
