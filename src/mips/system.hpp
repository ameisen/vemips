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

		processor * __restrict m_processor = nullptr;
		memory_source * __restrict m_memory_source = nullptr;
		std::vector<char> m_memory;
		platform::host_mmu *m_host_mmu = nullptr;
		debugger* m_debugger = nullptr;
		uint32 m_system_break = 0;

		// Termination
		bool m_execution_complete = false;
		bool m_execution_success = false;

	public:
		struct options final {
			uint32 total_memory = 0;
			uint32 stack_memory = 0; // 0 = no stack reservation, unified memory space
			mips::JitType jit_type = JitType::None;
			mmu mmu_type = mmu::emulated;
			uint16 debug_port = 0;
			bool read_only_exec : 1 = false;
			bool record_instruction_stats : 1 = false;
			bool disable_cti : 1 = false;
			bool ticked : 1 = false;
			bool instruction_cache : 1 = false;
			bool debug : 1 = false;
			bool debug_owned : 1 = false;

			void validate() const __restrict;
		};
	protected:
		const options m_options;

		void initialize(const elf::binary & __restrict binary);
	public:
		system(const options & __restrict init_options, const elf::binary & __restrict binary);
		virtual ~system();

		virtual void clock(uint64 clocks) __restrict;

		[[nodiscard]]
		uint64 get_instruction_count() const __restrict;

		[[nodiscard]]
		const std::unordered_map<const char *, size_t> & get_stats_map() const __restrict;

		[[nodiscard]]
		size_t get_jit_max_instruction_size() const __restrict;

		virtual uint32 handle_exception(const CPU_Exception & __restrict ex) __restrict = 0;

		[[nodiscard]]
		bool is_execution_complete() const __restrict {
			return m_execution_complete;
		}

		[[nodiscard]]
		bool is_execution_success() const __restrict {
			return m_execution_success;
		}

		[[nodiscard]]
		bool is_debugger_owned() const __restrict {
			return m_options.debug_owned;
		}

		[[nodiscard]]
		processor * get_processor() const __restrict {
			return m_processor;
		}

		[[nodiscard]]
		debugger * get_debugger() const __restrict {
			return m_debugger;
		}
	};
}
