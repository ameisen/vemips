#pragma once

#include "mips/config.hpp"
#include <common.hpp>

#include <mips/mips_common.hpp>
#include <mips/memory_source.hpp>

#include <expected>
#include <unordered_map>
#include <memory>

#include <map>

#include "coprocessor/coprocessor.hpp"
#include "mips/mmu.hpp"

// TODO : doesn't handle wraparound address properly.

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
	class coprocessor1;
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
		static constexpr const size_t num_coprocessors = coprocessor::max;

		// These are at the highest point to be easier on the JIT. < 127 B addresses are grand.
		alignas(64) std::array<register_type, num_registers /*- 1*/>	registers_{ 0 };
		flag													flags_ = flag::none;

		uint32												branch_target_ = 0;
		uint32												program_counter_ = 0;
	public:
		uint32												user_value_ = 0; // TODO REPLACE WITH COP0
	private:
		uint64												instruction_count_ = 0;
		uint64												target_instructions_ = std::numeric_limits<uint64>::max();
		const uintptr									memory_ptr_ = 0;
		const uint32									memory_size_ = 0;
		const uint32									stack_size_ = 0;

	private:

		const std::array<coprocessor* __restrict, num_coprocessors> coprocessors_ = { nullptr };
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

		const bool readonly_exec_ : 1;
		const bool ticked_ : 1;
		const bool collect_stats_ : 1;
		const bool disable_cti_ : 1;
		const bool debugging_ : 1 = false;
	public:
		bool in_jit : 1 = false;
	private:
		bool from_exception_ : 1 = false;

		void ExecuteInstruction(bool branch_delay);
		void ExecuteInstructionExplicit(const instructions::InstructionInfo* instruction_info, instruction_t instruction, bool branch_delay);

	public:
#pragma region dynamic recompiler support
		enum class offset_type
		{
			registers 
		};

		static constexpr intptr get_address_offset_static (const offset_type type)
		{
			switch (type)
			{
				case offset_type::registers:
					return offsetof(processor, registers_);

				default:
					xassert(false);
			}
		}

#pragma endregion
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
		std::unordered_map<const char *, size_t> & get_stats_map() {
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

		template <typename Self>
		[[nodiscard]]
		auto get_coprocessor(this Self&& self, const uint32 idx) ->
			copy_qualifiers_ptr<decltype(self), coprocessor>
		{
			xassert(idx < self.coprocessors_.size());
			return self.coprocessors_[idx];
		}

		template <typename Self>
		[[nodiscard]]
		auto get_coprocessor(this Self&& self, const coprocessor::types type) ->
			copy_qualifiers_ptr<decltype(self), coprocessor>
		{
			xassert(coprocessor::is_valid(type));
			return self.get_coprocessor(std::to_underlying(type));
		}
	private:
		template <std::underlying_type_t<coprocessor::types> Index>
		[[nodiscard]]
		auto get_coprocessor_impl(this auto&& self) -> std::conditional_t<
			Index == std::to_underlying(coprocessor::types::floating_point),
			_make_qual(coprocessor1 *),
			_make_qual(coprocessor *)
		>
		requires (
			Index < std::tuple_size_v<decltype(coprocessors_)>
		)
		{
			_make_qual(coprocessor*) coprocessor = self.coprocessors_[Index];

			if constexpr (Index == std::to_underlying(coprocessor::types::floating_point))
			{
				return reinterpret_cast<_make_qual(coprocessor1 *)>(coprocessor);
			}
			else
			{
				return coprocessor;
			}
		}

	public:
		template <std::underlying_type_t<coprocessor::types> Index>
		[[nodiscard]]
		auto get_coprocessor(this auto&& self) -> std::conditional_t<
			Index == std::to_underlying(coprocessor::types::floating_point),
			_make_qual(coprocessor1 *),
			_make_qual(coprocessor *)
		>
		requires (
			Index < std::tuple_size_v<decltype(coprocessors_)>
		)
		{
			return self.template get_coprocessor_impl<Index>();
		}

		template <coprocessor::types Type>
		[[nodiscard]]
		auto get_coprocessor(this auto&& self) requires(
			coprocessor::is_valid(Type)
		)
		{
			static constexpr const auto Index = std::to_underlying(Type);

			return self.template get_coprocessor_impl<Index>();
		}

		template <typename Self>
		[[nodiscard]]
		auto get_fpu_coprocessor(this Self&& self)
		{
			return self.template get_coprocessor<coprocessor::types::floating_point>();
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
			if (idx == 0) {
				return {};
			}

			// Strict-aliasing rules apply
			if constexpr (std::is_same_v<T, register_type>) {
				return registers_[idx /*- 1*/];
			}
			else {
				static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
				T result;
				memcpy(&result, &registers_[idx /*- 1*/], sizeof(result));
				return result;
			}
		}

		// Set the register from a given type
		template <typename T>
		void set_register(const uint32 idx, T value) {
			if (idx == 0) {
				return;
			}

			// Strict-aliasing rules apply
			if constexpr (std::is_same_v<T, register_type>) {
				registers_[idx /*- 1*/] = value;
			}
			else {
				static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
				union {
					register_type	dst;
					T							src;
				} caster{
					.src = value
				};
				registers_[idx /*- 1*/] = caster.dst;
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

	private:
		std::optional<uint32> mem_poke_host(uint32 address, uint32 size) const __restrict;
		std::optional<uint32> mem_fetch_host(void* dst, uint32 address, uint32 size) const __restrict;
		std::optional<uint32> mem_write_host(const void* src, uint32 address, uint32 size) const __restrict;

	public:
		template <typename T>
		void mem_poke(uint32 address) const __restrict {
			if (mmu_type_ == mmu::host) {
				// TODO : handle literal edge case - overflows 32-bit address space
				if (const auto result = mem_poke_host(address, sizeof(T)); result.has_value()) [[unlikely]]
				{
					throw CPU_Exception{ CPU_Exception::Type::AdEL, program_counter_, result.value() };
				}
				
				return;
			}
			else {
				if _unlikely(address == 0) [[unlikely]] {
					throw CPU_Exception{ CPU_Exception::Type::AdEL, program_counter_, address };
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					if _unlikely(memory_source_->at(address, sizeof(T))) [[unlikely]] {
						return;
					}
				}
				else if _unlikely(address < memory_size_) [[unlikely]] {
					return;
				}
			}

			// error path
			[&] __declspec(noinline) {
				if (mmu_type_ == mmu::emulated)
				{
					const std::optional<uint32> invalid_address = memory_source_->get_first_unreadable(address, sizeof(T));

					if (invalid_address.has_value())
					{
						throw CPU_Exception{ CPU_Exception::Type::AdEL, program_counter_, invalid_address.value() };
					}
				}
				else
				{
					for (uint32 i = 0; i < sizeof(T); ++i)
					{
						if (address >= memory_size_ || address == 0U)
						{
							throw CPU_Exception{ CPU_Exception::Type::AdEL, program_counter_, address };
						}
					}
				}

				throw CPU_Exception{ CPU_Exception::Type::AdEL, program_counter_, address };
			}();
		}

		template <typename T>
		[[nodiscard]]
		std::expected<T, uint32> try_mem_fetch_except(uint32 address) const __restrict {
			if (mmu_type_ == mmu::host) {
				// TODO : handle literal edge case - overflows 32-bit address space
				std::remove_const_t<T> result;
				if (
					const auto result_error = mem_fetch_host(&result, address, sizeof(T));
					result_error.has_value()
				) [[unlikely]]
				{
					return std::unexpected(result_error.value());
				}

				return result;
			}
			else {
				if _unlikely(address == 0) [[unlikely]] {
					return std::unexpected(0U);
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					const T* __restrict val_ptr = static_cast<const T * __restrict>(memory_source_->at(address, sizeof(T)));
					if _likely(val_ptr) [[likely]] {
						return *val_ptr;
					}
				}
				else if _likely(address < memory_size_) [[likely]] {
					return *reinterpret_cast<const T * __restrict>(uintptr_t(memory_ptr_) + address);
				}
			}

			// error path
			return [&] __declspec(noinline) {
				if (mmu_type_ == mmu::emulated)
				{
					const std::optional<uint32> invalid_address = memory_source_->get_first_unreadable(address, sizeof(T));

					if (invalid_address.has_value())
					{
						return std::unexpected(invalid_address.value());
					}
				}
				else
				{
					for (uint32 i = 0; i < sizeof(T); ++i)
					{
						if (address >= memory_size_ || address == 0U)
						{
							return std::unexpected(address);
						}
					}
				}

				return std::unexpected(address);
			}();
		}

		template <typename T>
		[[nodiscard]]
		std::optional<T> try_mem_fetch(const uint32 address) const __restrict {
			if (
				const auto result = try_mem_fetch_except<T>(address);
				result.has_value()
			)
			{
				return result.value();
			}
			else
			{
				return {};
			}
		}

		template <typename T>
		[[nodiscard]]
		T mem_fetch(uint32 address) const __restrict {
			if (
				const auto result = try_mem_fetch_except<T>(address);
				result.has_value()
			)
			{
				return result.value();
			}
			else
			{
				throw CPU_Exception{ CPU_Exception::Type::AdEL, program_counter_, result.error() };
			}
		}

		template <typename T>
		const T * mem_fetch_debugger(uint32 address) const __restrict {
			if (mmu_type_ == mmu::host)
			{
				// TODO : handle literal edge case - overflows 32-bit address space
				T result;
				if (
					const auto result_error = mem_fetch_host(&result, address, sizeof(T));
					result_error.has_value()
				) [[unlikely]]
				{
					return nullptr;
				}

				return reinterpret_cast<const T * __restrict>(uintptr_t(memory_ptr_) + address);
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
				// TODO : handle literal edge case - overflows 32-bit address space
				std::remove_const_t<T> result;
				if (
					const auto result_error = mem_fetch_host(&result, address, sizeof(T));
					result_error.has_value()
				) [[unlikely]]
				{
					return nullptr;
				}

				return reinterpret_cast<const T * __restrict>(uintptr_t(memory_ptr_) + address);
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
				else if _likely(address < memory_size_) [[likely]] {
					return (const T * __restrict)(uintptr_t(memory_ptr_) + address);
				}
			}

			return nullptr;
		}

		template <typename T>
		[[nodiscard]]
		std::optional<uint32> try_mem_write_except(uint32 address, T value) const __restrict {
			if (mmu_type_ == mmu::host) {
				// TODO : handle literal edge case - overflows 32-bit address space
				if (
					const auto result_error = mem_write_host(&value, address, sizeof(T));
					result_error.has_value()
				) [[unlikely]]
				{
					return result_error.value();
				}

				return {};
			}
			else {
				if _unlikely(address == 0) [[unlikely]] {
					return 0U;
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					*static_cast<T * __restrict>(memory_source_->write_at(address, sizeof(T))) = value;
					memory_touched(address, sizeof(T));
					return {};
				}
				else if _likely(address < memory_size_) [[likely]] {
					*reinterpret_cast<T * __restrict>(uintptr_t(memory_ptr_) + address) = value;
					memory_touched(address, sizeof(T));
					return {};
				}
			}

			// error path
			return [&] __declspec(noinline) {
				if (mmu_type_ == mmu::emulated)
				{
					const std::optional<uint32> invalid_address = memory_source_->get_first_unreadable(address, sizeof(T));

					if (invalid_address.has_value())
					{
						return invalid_address.value();
					}
				}
				else
				{
					for (uint32 i = 0; i < sizeof(T); ++i)
					{
						if (address >= memory_size_ || address == 0U)
						{
							return address;
						}
					}
				}

				return address;
			}();
		}

		template <typename T>
		void mem_write(uint32 address, T value) __restrict {
			const auto result = try_mem_write_except(address, value);
			if (result.has_value()) [[unlikely]]
			{
				throw CPU_Exception{ CPU_Exception::Type::AdES, program_counter_, result.value() };
			}
		}

		uintptr_t get_mem_write_jit(uint32 address, uint32 size) __restrict {
			if (mmu_type_ != mmu::host) {
				address += stack_size_;
			}
			const uintptr_t val_ptr = (uintptr_t)memory_source_->write_at(address, size);
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
			const uintptr_t val_ptr = (uintptr_t)memory_source_->write_at(address, size);
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
		for (size_t i = 1/*0*/; i < num_registers /*- 1*/; ++i) { // don't check $zero
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
