#pragma once

#include "mips/config.hpp"
#include <common.hpp>
#include <mips/mips_common.hpp>
#include <mips/memory_source.hpp>

#include <unordered_map>

#include <map>
#include "mips/mmu.hpp"

namespace mips::instructions {
	struct InstructionInfo;
}

class CodeGen;
namespace mips {
	enum class JitType : uint32 {
		None,
		Jit,
		FunctionTable
	};

	class system;
	class jit1;
	class coprocessor;
	class processor final {
		// remove when we have unified instructions
		friend class CPUI;
		friend class jit1;
		friend class jit2;
		friend class system;
		friend class Jit1_CodeGen;

	public:
		using register_type = uint32;
		enum class flag : uint32 {
			branch_delay = 0,
			no_cti = 1,
			trapped_exception = 2,
			pc_changed = 3,
			jit_mem_flush = 4,
		};
		static constexpr uint32 flag_bit(flag _bit) { return 1u << uint32(_bit); }

	private:
		static constexpr const size_t NumRegisters = 32;

		// These are at the highest point to be easier on the JIT. < 127 B addresses are grand.
		std::array<register_type, NumRegisters>	m_registers{ 0 };
		uint32												m_flags = 0;

		uint32												m_branch_target = 0;
		uint32												m_program_counter = 0;
		uint64												m_instruction_count = 0;
		uint64												m_target_instructions = std::numeric_limits<uint64>::max();;
		uint64												m_memory_ptr = 0;
		uint32												m_memory_size = 0;
		uint32												m_stack_size = 0;

	public:
		uint32												m_user_value = 0; // TODO REPLACE WITH COP0

	private:

		coprocessor	 * __restrict m_coprocessors[4]{ nullptr };
		memory_source  * __restrict m_memory_source = nullptr;

	public: // clean up later
		union {
			jit1 * __restrict							 m_jit1;
			jit2 * __restrict							 m_jit2;
		};
	private:
		JitType											  m_jit_type;

		CPU_Exception									  m_trapped_exception;
		
		const bool												  m_readonly_exec;
		const bool										m_ticked;
		const bool												  m_collect_stats;
		const bool																			m_disable_cti;
		std::unordered_map<const char *, size_t>  m_instruction_stats;

		bool												  m_from_exception = false;
		mips::mmu											m_mmu_type;
		system												*m_guest_system = nullptr;
		const bool												  m_debugging = false;

		void ExecuteInstruction(bool branch_delay);

	public:
		void compare(const processor & __restrict other, uint32 /*previous_pc*/) {
			for (size_t i = 1; i < NumRegisters; ++i) { // don't check $zero
				if (m_registers[i] != other.m_registers[i]) {
					__debugbreak();
				}
			}
			if (!m_from_exception && (m_program_counter != other.m_program_counter)) {
				__debugbreak();
			}
			if (m_instruction_count != other.m_instruction_count) {
				__debugbreak();
			}
			if ((m_flags & ~flag_bit(flag::jit_mem_flush)) != (other.m_flags & ~flag_bit(flag::jit_mem_flush))) {
				__debugbreak();
			}
			if (get_flags(flag_bit(flag::branch_delay))) {
				if (m_branch_target != other.m_branch_target) {
					__debugbreak();
				}
			}
		}

		void add_stat(const char *name) {
			++m_instruction_stats[name];
		}

		const std::unordered_map<const char *, size_t> & get_stats_map() const {
			return m_instruction_stats;
		}

		size_t get_jit_max_instruction_size() const __restrict;

		uint64 get_instruction_count() const __restrict {
			return m_instruction_count;
		}

		JitType get_jit_type() const {
			return m_jit_type;
		}

		void set_trapped_exception(const CPU_Exception & __restrict ex) {
			set_flags(flag_bit(flag::trapped_exception));
			m_trapped_exception = ex;
		}

		struct options final {
			memory_source* __restrict mem_src = nullptr;
			char* mem_ptr = nullptr;
			system* guest_system = nullptr;
			JitType jit_type = JitType::Jit;
			mips::mmu mmu_type = mips::mmu::emulated;
			uint32 stack = 0;
			uint32 mem_size = 0;
			bool rox = false;
			bool collect_stats = false;
			bool disable_cti = false;
			bool ticked = false;
			bool icache = false;
			bool debugging = false;
		};

		processor(const options & __restrict _options);
		~processor();

		coprocessor * get_coprocessor(uint32 idx) __restrict {
			return m_coprocessors[idx];
		}

		// Sets the program counter. This is meant to be used as an external function to set up the CPU
		void set_program_counter(uint32 address) __restrict;

		// Gets the program counter
		uint32 get_program_counter() __restrict;

		// Execute Clock
		void execute(uint64 clocks = 1);
		template <bool ticked, bool debugging>
		_forceinline void execute_internal(uint64 clocks = 1);

		// Get the register as a specific type
		template <typename T>
		T get_register(uint32 idx) const {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
			union {
				register_type  src;
				T				  dst;
			} caster;
			caster.src = m_registers[idx];
			return caster.dst;
		}

		// Set the register from a given type
		template <typename T>
		void set_register(uint32 idx, T value) {
			// Strict-aliasing rules apply
			static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
			union {
				register_type  dst;
				T				  src;
			} caster;
			caster.src = value;
			m_registers[idx] = caster.dst;
		}

		// Set up a delay branch for the processor to the given pointer
		void delay_branch(uint32 pointer) __restrict;

		// Set up a compact branch for the processor to the given pointer
		void compact_branch(uint32 pointer) __restrict;

		// Sets the next instruction as being in the forbidden slot
		void set_no_cti() __restrict;

		bool get_no_cti() const __restrict {
			return get_flags(flag_bit(flag::no_cti));
		}

		// Sets the link register
		void set_link(uint32 pointer);

		// Gets the instruction at the current program counter
		instruction_t get_instruction() const __restrict;

		void memory_touched(uint32 pointer, uint32 size) __restrict;
		void memory_touched_jit(uint32 pointer, uint32 size) __restrict;

		template <typename T>
		void mem_poke(uint32 address) __restrict {
			if (m_mmu_type == mmu::host) {
				m_memory_source->at(address, sizeof(T)); // TODO this doesn't actually work
			}
			else {
				if (address == 0) {
					throw CPU_Exception{ CPU_Exception::Type::AdES, m_program_counter, address };
				}

				address += m_stack_size;

				if (m_mmu_type == mmu::emulated) {
					if (!m_memory_source->at(address, sizeof(T))) {
						throw CPU_Exception{ CPU_Exception::Type::AdEL, m_program_counter, address };
					}
				}
				else if (address >= m_memory_size) {
					throw CPU_Exception{ CPU_Exception::Type::AdEL, m_program_counter, address };
				}
			}
		}

		template <typename T>
		T mem_fetch(uint32 address) const __restrict {
			if (m_mmu_type == mmu::host) {
				return *(const T * __restrict)(uintptr_t(m_memory_ptr) + address);
			}
			else {
				if (address == 0) {
					throw CPU_Exception{ CPU_Exception::Type::AdES, m_program_counter, address };
				}

				address += m_stack_size;

				if (m_mmu_type == mmu::emulated) {
					const T* __restrict val_ptr = (const T * __restrict)m_memory_source->at(address, sizeof(T));
					if (val_ptr) {
						return *val_ptr;
					}
				}
				else if (address < m_memory_size) {
					return *(const T * __restrict)(uintptr_t(m_memory_ptr) + address);
				}
			}
			throw CPU_Exception{ CPU_Exception::Type::AdEL, m_program_counter, address };
		}

		template <typename T>
		const T * mem_fetch_debugger(uint32 address) const __restrict {
			if (m_mmu_type == mmu::host)
			{
				return (const T * __restrict)(uintptr_t(m_memory_ptr) + address);
			}
			else {
				if (address == 0) {
					return nullptr;
				}

				address += m_stack_size;

				if (m_mmu_type == mmu::emulated) {
					const T* __restrict val_ptr = (const T * __restrict)m_memory_source->at(address, sizeof(T));
					return val_ptr;
				}
				else if (address < m_memory_size) {
					return (const T * __restrict)(uintptr_t(m_memory_ptr) + address);
				}
			}
			return nullptr;
		}

		template <typename T>
		T * __restrict safe_mem_fetch_exec(uint32 address) const __restrict {
			if (m_mmu_type == mmu::host) {
				return (const T * __restrict)(uintptr_t(m_memory_ptr) + address);
			}
			else {
				if (address == 0) {
					return nullptr;
				}

				address += m_stack_size;

				if (m_mmu_type == mmu::emulated) {
					const T* __restrict val_ptr = (const T * __restrict)m_memory_source->at_exec(address, sizeof(T));
					return val_ptr;
				}
				else {
					if (address < m_memory_size) {
						return (const T * __restrict)(uintptr_t(m_memory_ptr) + address);
					}
					else {
						return nullptr;
					}
				}
			}
		}

		template <typename T>
		void mem_write(uint32 address, T value) __restrict {
			if (m_mmu_type == mmu::host) {
				*(T * __restrict)(uintptr_t(m_memory_ptr) + address) = value;
				memory_touched(address, sizeof(T));
				return;
			}
			else {
				if (address == 0) {
					throw CPU_Exception{ CPU_Exception::Type::AdES, m_program_counter, address };
				}

				address += m_stack_size;

				if (m_mmu_type == mmu::emulated) {
					T* __restrict val_ptr = (T * __restrict)m_memory_source->write_at(address, sizeof(T));
					if (val_ptr) {
						*val_ptr = value;
						memory_touched(address, sizeof(T));
						return;
					}
				}
				else if (address < m_memory_size) {
					*(T * __restrict)(uintptr_t(m_memory_ptr) + address) = value;
					memory_touched(address, sizeof(T));
					return;
				}
			}
			throw CPU_Exception{ CPU_Exception::Type::AdES, m_program_counter, address };
		}

		uintptr_t get_mem_write_jit(uint32 address, uint32 size) __restrict {
			if (m_mmu_type != mmu::host) {
				address += m_stack_size;
			}
			const uintptr_t val_ptr = (uintptr_t)m_memory_source->at(address, size);
			if (val_ptr) {
				memory_touched(address, size);
			}
			return val_ptr;
		}

		uintptr_t get_mem_read_jit(uint32 address, uint32 size) __restrict {
			if (m_mmu_type != mmu::host) {
				address += m_stack_size;
			}
			const uintptr_t val_ptr = (uintptr_t)m_memory_source->at(address, size);
			return val_ptr;
		}

		bool get_flags(uint32 bits) const __restrict {
			return !!(m_flags & bits);
		}

		void set_flags(uint32 bits) __restrict {
			m_flags |= bits;
		}

		void clear_flags(uint32 bits) __restrict {
			m_flags &= ~bits;
		}

		system * get_guest_system() __restrict {
			return m_guest_system;
		}
	};
}
