#pragma once

#include "mips/config.hpp"
#include <common.hpp>

#include <memory>
#include <new>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "mips/llsc.hpp"
#include "mips/mips.hpp"
#include "mips/mmu.hpp"
#include "mips/statistics.hpp"
#include "processor/jit/jit.hpp"

namespace mips {
	struct CPU_Exception;
	class debugger;
	class memory_source;
	class processor;

	namespace elf
	{
		class binary;
	}

	namespace platform
	{
		class host_mmu;
	}

	class system {
		static constexpr usize memory_alignment = std::hardware_destructive_interference_size;

		struct memory_deleter final
		{
			_nothrow void operator()(char* ptr) const noexcept
			{
				::operator delete[](ptr, std::align_val_t{memory_alignment});
			}
		};

	public:
		struct options final {
			enum class policy : uint8
			{
				// Is unsupported but the behavior of it is undefined. Fastest mode.
				undefined = 0,
				// Is disabled and this is enforced.
				disabled,
				// Is enabled.
				enabled,
				// Is enabled, and with extra runtime strictness
				strict
			};

			// TODO : some systems allow more memory to be physically addressed than their bit-size.
			usize_guest total_memory = 0;
			usize_guest stack_memory = 0; // 0 = no stack reservation, unified memory space
			JitType jit_type = JitType::None;
			llsc llsc_type = llsc::fine;
			mmu mmu_type = mmu::emulated;
			uint16 debug_port = 0;
			bool read_only_exec : 1 = false;
			bool collect_statistics : 1 = false;
			bool disable_cti : 1 = false;
			bool ticked : 1 = false;
			bool debug : 1 = false;
			bool debug_owned : 1 = false;
			policy self_modifying_code : 2 = policy::enabled;

			void validate() const;
		};

		struct capabilities final
		{
			bool can_handle_syscalls_inline : 1 = false;
		};

	protected:
		friend class processor;


		processor* __restrict processor_ = nullptr;
		memory_source* __restrict memory_source_ = nullptr;
		std::unique_ptr<char[], memory_deleter> memory_; // includes shadow memory, if present
		platform::host_mmu *host_mmu_ = nullptr;
		debugger* debugger_ = nullptr;
		usize_guest shadow_memory_offset_ = 0;
		uptr_guest system_break_base_ = 0;
		uptr_guest system_break_ = 0;
		int32 execution_result_code_ = 0;

		const options options_;
		const capabilities capabilities_;

		// Termination
		enum class execution_state : uint8
		{
			running = 0b00,
			complete_success = 0b01,
			complete_fail = 0b10
		};
		execution_state execution_state_ : 2 = execution_state::running;
			
		void initialize(const elf::binary& __restrict binary);

		system(capabilities&& capabilities, const options& __restrict init_options, const elf::binary& __restrict binary);

	public:
		system(const options& __restrict init_options, const elf::binary& __restrict binary);
		virtual ~system();

		virtual void clock(uint64 clocks) __restrict;

		[[nodiscard]]
		_nothrow _pure bool requires_instruction_cache_directory() const __restrict noexcept;

		[[nodiscard]]
		_nothrow _pure bool requires_shadow_memory() const __restrict noexcept;

		[[nodiscard]]
		_nothrow _forceinline _pure const capabilities& get_capabilities() const __restrict noexcept
		{
			return capabilities_;
		}

		[[nodiscard]]
		_nothrow _pure uint64 get_instruction_count() const __restrict noexcept;

		[[nodiscard]]
		_nothrow _pure const statistics* get_statistics() const __restrict noexcept;

		[[nodiscard]]
		_nothrow _pure std::pair<const char*, usize> get_jit_max_instruction_size() const __restrict noexcept;

		virtual uint32 handle_exception(const CPU_Exception & __restrict ex) __restrict = 0;

		[[nodiscard]]
		_nothrow _forceinline _pure bool is_execution_complete() const __restrict noexcept {
			return execution_state_ != execution_state::running;
		}

		[[nodiscard]]
		_nothrow _forceinline _pure bool is_execution_success() const __restrict noexcept {
			return execution_state_ == execution_state::complete_success;
		}
		
		[[nodiscard]]
		_nothrow _forceinline _pure int32 get_exit_code() const __restrict noexcept {
			return execution_result_code_;
		}

		[[nodiscard]]
		_nothrow _forceinline _pure bool is_debugger_owned() const __restrict noexcept {
			return options_.debug_owned;
		}

		[[nodiscard]]
		_nothrow _forceinline _pure options::policy get_self_modifying_code_policy() const __restrict noexcept {
			return options_.self_modifying_code;
		}

		[[nodiscard]]
		_nothrow _forceinline _pure processor* get_processor() const __restrict noexcept {
			return processor_;
		}

		[[nodiscard]]
		_nothrow _forceinline _pure debugger* get_debugger() const __restrict noexcept {
			return debugger_;
		}

		[[nodiscard]]
		_nothrow _forceinline _pure std::span<char> get_memory() const __restrict noexcept {
			return { memory_.get(), options_.total_memory };
		}

		[[nodiscard]]
		_nothrow _forceinline _pure std::span<char> get_shadow_memory() const __restrict noexcept {
			return requires_shadow_memory() ?
				std::span<char>{ memory_.get() + shadow_memory_offset_, options_.total_memory } :
				std::span<char>{};
		}

		// TODO : when I figure out how to make it work well with the coprocessor being virtual,
		// return a bitfielded struct instead.
		[[nodiscard]]
		_pure // technically not pure because it returns a `std::vector` which allocates (side-effects)
		_nothrow std::vector<std::string> compare(const system& __restrict other) const noexcept;
	};
}
