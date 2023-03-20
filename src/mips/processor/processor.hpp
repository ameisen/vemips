#pragma once

#include "mips/config.hpp"
#include <common.hpp>
#include <mips/mips_common.hpp>
#include <mips/memory_source.hpp>

#include <unordered_map>
#include <memory>

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
			none = 0u,
			branch_delay = 1u << 0,
			no_cti = 1u << 1,
			trapped_exception = 1u << 2,
			pc_changed = 1u << 3,
			jit_mem_flush = 1u << 4,
		};

	private:
		static constexpr const size_t num_registers = 32;

		// These are at the highest point to be easier on the JIT. < 127 B addresses are grand.
		std::array<register_type, num_registers>	registers_{ 0 };
		flag													flags_ = flag::none;

		uint32												branch_target_ = 0;
		uint32												program_counter_ = 0;
		uint64												instruction_count_ = 0;
		uint64												target_instructions_ = std::numeric_limits<uint64>::max();
		const uintptr									memory_ptr_ = 0;
		const uint32									memory_size_ = 0;
		const uint32									stack_size_ = 0;

	public:
		uint32												user_value_ = 0; // TODO REPLACE WITH COP0

	private:

		const std::array<coprocessor* __restrict, 4> coprocessors_ = { nullptr };
		memory_source  * const __restrict memory_source_ = nullptr;

	public: // TODO : clean up later, though not using std::variant most likely (exception overhead)
		union {
			// TODO : wrap these in a lightweight variant
			jit1 * __restrict								jit1_;
			jit2 * __restrict								jit2_;
		};
	private:
		const JitType											  jit_type_;

		CPU_Exception									  trapped_exception_ = {};

		const std::unique_ptr<std::unordered_map<const char *, size_t>>  instruction_stats_;

		const mips::mmu											mmu_type_;
		system												* const guest_system_ = nullptr;

		struct {
			const bool readonly_exec_ : 1;
			const bool ticked_ : 1;
			const bool collect_stats_ : 1;
			const bool disable_cti_ : 1;
			const bool debugging_ : 1 = false;
			bool from_exception_ : 1 = false;
		};

		void ExecuteInstruction(bool branch_delay);
		void ExecuteInstructionExplicit(const instructions::InstructionInfo* instruction_info, instruction_t instruction, bool branch_delay);

	public:
		void compare(const processor& __restrict other, uint32 /*previous_pc*/) const;

		void add_stat(const char *name) {
			++(*instruction_stats_)[name];
		}

		[[nodiscard]]
		const std::unordered_map<const char *, size_t> & get_stats_map() const {
			return (*instruction_stats_);
		}

		[[nodiscard]]
		size_t get_jit_max_instruction_size() const __restrict;

		[[nodiscard]]
		uint64 get_instruction_count() const __restrict {
			return instruction_count_;
		}

		[[nodiscard]]
		JitType get_jit_type() const __restrict {
			return jit_type_;
		}

		void set_trapped_exception(const CPU_Exception & __restrict ex) {
			set_flags(flag::trapped_exception);
			trapped_exception_ = ex;
		}

		struct options final {
			memory_source* __restrict mem_src = nullptr;
			char* mem_ptr = nullptr;
			system* guest_system = nullptr;
			JitType jit_type = JitType::Jit;
			mips::mmu mmu_type = mips::mmu::emulated;
			uint32 stack = 0;
			uint32 mem_size = 0;
			bool rox : 1 = false;
			bool collect_stats : 1 = false;
			bool disable_cti : 1 = false;
			bool ticked : 1 = false;
			bool icache : 1 = false;
			bool debugging : 1 = false;
		};

		processor(const options & __restrict options);
		~processor();

		[[nodiscard]]
		coprocessor * get_coprocessor(const uint32 idx) const {
			return coprocessors_[idx];
		}

		// Sets the program counter. This is meant to be used as an external function to set up the CPU
		void set_program_counter(uint32 address) __restrict;

		// Gets the program counter
		[[nodiscard]]
		uint32 get_program_counter() const __restrict;

		// Execute Clock
		void execute(uint64 clocks = 1);
		template <bool ticked, bool debugging>
		_forceinline void execute_internal(uint64 clocks = 1);
		bool execute_explicit(const instructions::InstructionInfo* instruction_info, instruction_t instruction);

		// Get the register as a specific type
		template <typename T>
		[[nodiscard]]
		T get_register(const uint32 idx) const {
			if constexpr (_constant_p(idx) && idx == 0) {
				return {};
			}

			// Strict-aliasing rules apply
			if constexpr (std::is_same_v<T, register_type>) {
				return registers_[idx];
			}
			else {
				static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
				T result;
				memcpy(&result, &registers_[idx], sizeof(result));
				return result;
			}
		}

		// Set the register from a given type
		template <typename T>
		void set_register(const uint32 idx, T value) {
			if constexpr (_constant_p(idx) && idx == 0) {
				return;
			}

			// Strict-aliasing rules apply
			if constexpr (std::is_same_v<T, register_type>) {
				registers_[idx] = value;
			}
			else {
				static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
				union {
					register_type	dst;
					T							src;
				} caster{
					.src = value
				};
				registers_[idx] = caster.dst;
			}
		}

		// Set up a delay branch for the processor to the given pointer
		void delay_branch(uint32 pointer) __restrict;

		// Set up a compact branch for the processor to the given pointer
		void compact_branch(uint32 pointer) __restrict;

		// Sets the next instruction as being in the forbidden slot
		void set_no_cti() __restrict;

		[[nodiscard]]
		bool get_no_cti() const __restrict {
			return get_flags(flag::no_cti);
		}

		// Sets the link register
		void set_link(uint32 pointer);

		// Gets the instruction at the current program counter
		[[nodiscard]]
		instruction_t get_instruction() const __restrict;

		void memory_touched(uint32 pointer, uint32 size) const __restrict;
		void memory_touched_jit(uint32 pointer, uint32 size) const __restrict;

		template <typename T>
		void mem_poke(uint32 address) const __restrict {
			if (mmu_type_ == mmu::host) {
				memory_source_->at(address, sizeof(T)); // TODO this doesn't actually work
			}
			else {
				if _unlikely(address == 0) [[unlikely]] {
					throw CPU_Exception{ CPU_Exception::Type::AdES, program_counter_, address };
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					if _unlikely(!memory_source_->at(address, sizeof(T))) [[unlikely]] {
						throw CPU_Exception{ CPU_Exception::Type::AdEL, program_counter_, address };
					}
				}
				else if _unlikely(address >= memory_size_) [[unlikely]] {
					throw CPU_Exception{ CPU_Exception::Type::AdEL, program_counter_, address };
				}
			}
		}

		template <typename T>
		[[nodiscard]]
		T mem_fetch(uint32 address) const __restrict {
			if (mmu_type_ == mmu::host) {
				return *(const T * __restrict)(uintptr_t(memory_ptr_) + address);
			}
			else {
				if _unlikely(address == 0) [[unlikely]] {
					throw CPU_Exception{ CPU_Exception::Type::AdES, program_counter_, address };
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					const T* __restrict val_ptr = (const T * __restrict)memory_source_->at(address, sizeof(T));
					if _likely(val_ptr) [[likely]] {
						return *val_ptr;
					}
				}
				else if _likely(address < memory_size_) [[likely]] {
					return *(const T * __restrict)(uintptr_t(memory_ptr_) + address);
				}
			}
			throw CPU_Exception{ CPU_Exception::Type::AdEL, program_counter_, address };
		}

		template <typename T>
		const T * mem_fetch_debugger(uint32 address) const __restrict {
			if (mmu_type_ == mmu::host)
			{
				return (const T * __restrict)(uintptr_t(memory_ptr_) + address);
			}
			else {
				if _unlikely(address == 0) [[unlikely]] {
					return nullptr;
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					const T* __restrict val_ptr = (const T * __restrict)memory_source_->at(address, sizeof(T));
					if _likely(val_ptr) [[likely]] {
						return val_ptr;
					}
				}
				else if _likely(address < memory_size_) [[likely]] {
					return (const T * __restrict)(uintptr_t(memory_ptr_) + address);
				}
			}
			return nullptr;
		}

		template <typename T>
		T * __restrict safe_mem_fetch_exec(uint32 address) const __restrict {
			if (mmu_type_ == mmu::host) {
				return (const T * __restrict)(uintptr_t(memory_ptr_) + address);
			}
			else {
				if _unlikely(address == 0) [[unlikely]] {
					return nullptr;
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					const T* __restrict val_ptr = (const T * __restrict)memory_source_->at_exec(address, sizeof(T));
					if _likely(val_ptr) [[likely]] {
						return val_ptr;
					}
				}
				else {
					if _likely(address < memory_size_) [[likely]] {
						return (const T * __restrict)(uintptr_t(memory_ptr_) + address);
					}
					else {
						return nullptr;
					}
				}
			}
		}

		template <typename T>
		void mem_write(uint32 address, T value) __restrict {
			if (mmu_type_ == mmu::host) {
				*(T * __restrict)(uintptr_t(memory_ptr_) + address) = value;
				memory_touched(address, sizeof(T));
				return;
			}
			else {
				if _unlikely(address == 0) [[unlikely]] {
					throw CPU_Exception{ CPU_Exception::Type::AdES, program_counter_, address };
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					T* __restrict val_ptr = (T * __restrict)memory_source_->write_at(address, sizeof(T));
					if _likely(val_ptr) [[likely]] {
						*val_ptr = value;
						memory_touched(address, sizeof(T));
						return;
					}
				}
				else if _likely(address < memory_size_) [[likely]] {
					*(T * __restrict)(uintptr_t(memory_ptr_) + address) = value;
					memory_touched(address, sizeof(T));
					return;
				}
			}
			throw CPU_Exception{ CPU_Exception::Type::AdES, program_counter_, address };
		}

		uintptr_t get_mem_write_jit(uint32 address, uint32 size) __restrict {
			if (mmu_type_ != mmu::host) {
				address += stack_size_;
			}
			const uintptr_t val_ptr = (uintptr_t)memory_source_->at(address, size);
			if _likely(val_ptr) [[likely]] {
				memory_touched(address, size);
			}
			return val_ptr;
		}

		[[nodiscard]]
		uintptr_t get_mem_read_jit(uint32 address, uint32 size) const __restrict {
			if (mmu_type_ != mmu::host) {
				address += stack_size_;
			}
			const uintptr_t val_ptr = (uintptr_t)memory_source_->at(address, size);
			return val_ptr;
		}

		[[nodiscard]]
		bool get_flags(const flag bits) const __restrict;

		void set_flags(const flag bits) __restrict;

		void clear_flags(const flag bits) __restrict;

		[[nodiscard]]
		system* get_guest_system() const __restrict {
			return guest_system_;
		}
	};

	static constexpr processor::flag operator & (processor::flag a, processor::flag b) {
		using underlying_t = std::underlying_type_t<decltype(a)>;
		return decltype(a)(underlying_t(a) & underlying_t(b));
	}

	static constexpr processor::flag operator | (processor::flag a, processor::flag b) {
		using underlying_t = std::underlying_type_t<decltype(a)>;
		return decltype(a)(underlying_t(a) | underlying_t(b));
	}

	static constexpr processor::flag operator &= (processor::flag& __restrict a, processor::flag b) {
		using underlying_t = std::underlying_type_t<decltype(b)>;
		return a = decltype(b)(underlying_t(a) & underlying_t(b));
	}

	static constexpr processor::flag operator |= (processor::flag& __restrict a, processor::flag b) {
		using underlying_t = std::underlying_type_t<decltype(b)>;
		return a = decltype(b)(underlying_t(a) | underlying_t(b));
	}

	static constexpr processor::flag operator ~ (processor::flag v) {
		using underlying_t = std::underlying_type_t<decltype(v)>;
		return decltype(v)(~underlying_t(v));
	}

	static constexpr bool operator ! (const processor::flag v) {
		return v == processor::flag::none;
	}

	inline bool processor::get_flags(const flag bits) const __restrict {
		return !!(flags_ & bits);
	}

	inline void processor::set_flags(const flag bits) __restrict {
		flags_ = flags_ | bits;
	}

	inline void processor::clear_flags(const flag bits) __restrict {
		flags_ = flags_ & ~bits;
	}

	inline void processor::compare(const processor& __restrict other, uint32 /*previous_pc*/) const {
		for (size_t i = 1; i < num_registers; ++i) { // don't check $zero
			if _unlikely(registers_[i] != other.registers_[i]) [[unlikely]] {
				__debugbreak();
			}
		}
		if _unlikely(!from_exception_ && (program_counter_ != other.program_counter_)) [[unlikely]] {
			__debugbreak();
		}
		if _unlikely(instruction_count_ != other.instruction_count_) [[unlikely]] {
			__debugbreak();
		}
		if _unlikely((flags_ & ~flag::jit_mem_flush) != (other.flags_ & ~flag::jit_mem_flush)) [[unlikely]] {
			__debugbreak();
		}
		if (get_flags(flag::branch_delay)) {
			if _unlikely(branch_target_ != other.branch_target_) [[unlikely]] {
				__debugbreak();
			}
		}
	}
}
