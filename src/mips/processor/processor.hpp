#pragma once

#include "mips/config.hpp"
#include <common.hpp>

#include <array>
#include <cstddef>
#include <cstring>
#include <expected>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include "jit/jit.hpp"
#include "mips/exception.hpp"
#include "mips/llsc.hpp"
#include "mips/memory_source.hpp"
#include "mips/mips_common.hpp"
#include "mips/mmu.hpp"
#include "mips/statistics.hpp"
#include "mips/coprocessor/coprocessor.hpp"
#include "mips/instructions/instructions_common.hpp"
#if !VEMIPS_TABLEGEN
#	include "mips/interpreter/cache/hash_cache.hpp"
#endif

// TODO : doesn't handle wraparound address properly.

namespace mips::instructions {
	struct InstructionInfo;
}

class CodeGen;
namespace mips {
	enum class memory_hazards : uint32;
	class system;
	class jit1;
	class coprocessor;
	class coprocessor1;
	struct processor_executor;

	class processor final {
		// remove when we have unified instructions
		friend class CPUI;
		friend class jit1;
		friend class system;
		friend class Jit1_CodeGen;

	public:
		/// <summary>
		/// Pointer alignment for LL addresses. Is 8 instead of 4 because of `LLWP`/`SCWP` instruction pairs.
		/// TODO : could make it handle those alignments, though we'd need some conditional logic. 
		/// TODO : being 8B instead of 4B aligned means that 'fine' LLSC granularity isn't as fine as it should be.
		/// </summary>
		static constexpr usize_guest load_link_align = 8;

		using register_type = uptr_guest;
		enum class flag : uint32 {
			none = 0u,
			branch_delay = bit<uint32, 0U>,
			no_cti = bit<uint32, 1U>,
			trapped_exception = bit<uint32, 2U>,
			pc_changed = bit<uint32, 3U>,
			instruction_hazard = bit<uint32, 4U>,
			instruction_hazard_delay_branch = bit<uint32, 5U>,
		};

		class scoped_flag final
		{
			_no_unique
			processor& __restrict processor_;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
			flag value_;

		public:
			[[nodiscard]]
			_nothrow _forceinline explicit scoped_flag(processor& processor) noexcept :
				processor_(processor),
				value_(processor.flags_)
			{}

			[[nodiscard]]
			_nothrow _forceinline explicit scoped_flag(processor* processor) noexcept :
				scoped_flag(*processor)
			{}
			
			_nothrow _forceinline ~scoped_flag() noexcept
			{
				processor_.flags_ = value_;
			}

			_nothrow _forceinline void flush() const __restrict noexcept {
				processor_.flags_ = value_;
			}
			
			_nothrow _forceinline void reload() __restrict noexcept {
				value_ = processor_.flags_;
			}
				
			[[nodiscard]]
			_pure
			_nothrow _forceinline flag& get() __restrict & noexcept {
				return value_;
			}

			[[nodiscard]]
			_pure
			_nothrow _forceinline flag& get() __restrict && noexcept = delete;

			[[nodiscard]]
			_pure
			_nothrow _forceinline flag get() const __restrict & noexcept {
				return value_;
			}

			[[nodiscard]]
			_pure
			_nothrow _forceinline flag& operator*() __restrict & noexcept {
				return value_;
			}

			[[nodiscard]]
			_pure
			_nothrow _forceinline flag& operator*() __restrict && noexcept = delete;

			[[nodiscard]]
			_pure
			_nothrow _forceinline flag operator*() const __restrict & noexcept {
				return value_;
			}

			_nothrow _forceinline scoped_flag& operator=(const flag value) __restrict noexcept {
				value_ = value;
				return *this;
			}

			[[nodiscard]]
			_pure
			// ReSharper disable once CppNonExplicitConversionOperator
			_nothrow _forceinline operator flag() const __restrict & noexcept {
				return value_;
			}

			[[nodiscard]]
			_pure
			// ReSharper disable once CppNonExplicitConversionOperator
			_nothrow _forceinline operator flag&() __restrict & noexcept {
				return value_;
			}

			[[nodiscard]]
			_pure
			// ReSharper disable once CppNonExplicitConversionOperator
			_nothrow _forceinline operator flag&() __restrict && noexcept = delete;
		};

		/// How should misaligned addresses be handled?
		enum class misaligned_address_handling : uint8
		{
			/// An Address Exception should be raised
			exception = 0,
			/// The address should be aligned
			align,
			/// Perform the operation on the unaligned address
			keep,
		};

		/* ABI information put here for now:
		// O32: https://refspecs.linuxfoundation.org/elf/mipsabi.pdf
		//  result:       r2-r3
		//  result FP:    f0:f1-f2:f3
		//  param:        r4-r7
		//  param FP:     f12:f13-f14:f15
		//  volatile:     r8-r15, r24-r25
		//  non-volatile: r16-r23, r30
		//  kernel:       r26-r27
		//  gp:           r28
		//  sp:           r29
		//  fp:           r30
		//  ra:           r31
		// EABI: https://sourceware.org/legacy-ml/binutils/2003-06/msg00436.html
		//  result:       r2-r3
		//  result float: f0-f1
		//  param:        r4-r11
		//  param FP:     f12-f19
		//  volatile:     r1-r15, r24, r25
		//  non-volatile: r16-r23, r30
		//  kernel:       r26-r27
		//  gp:           r28
		//  sp:           r29
		//  fp:           r30
		//  ra:           r31
		*/

		enum class SystemAbi : uint8 {
			O32 = 0,
			EABI = 1,
		};

		struct named_registers_base {
			named_registers_base() = delete;

			static constexpr uint8 zero = 0U;
			static constexpr uint8 non_volatile[8] {
				16U,
				17U,
				18U,
				19U,
				20U,
				21U,
				22U,
				23U
			};
			static constexpr uint8 kernel[2] {
				26U,
				27U
			};
			static constexpr uint8 global_pointer = 28U;
			static constexpr uint8 stack_pointer = 29U;
			static constexpr uint8 frame_pointer = 30U;
			static constexpr uint8 return_address = 31U;
		};

		template <SystemAbi Abi>
		struct named_registers;

		template <>
		struct named_registers<SystemAbi::O32> final : named_registers_base
		{
			static constexpr uint8 result[] {
				2U,
				3U
			};
			static constexpr uint8 parameter[] {
				4U,
				5U,
				6U,
				7U
			};
			static constexpr uint8 volatile_tmp[] {
				1U,
				8U,
				9U,
				10U,
				11U,
				12U,
				13U,
				14U,
				15U,
				24U,
				25U
			};
		};

		// https://sourceware.org/legacy-ml/binutils/2003-06/msg00436.html
		template <>
		struct named_registers<SystemAbi::EABI> final : named_registers_base
		{
			static constexpr uint8 result[] {
				2U,
				3U
			};
			static constexpr uint8 parameter[] {
				4U,
				5U,
				6U,
				7U,
				8U,
				9U,
				10U,
				11U,
			};
			static constexpr uint8 volatile_tmp[] {
				1U,
				2U,
				3U,
				4U,
				5U,
				6U,
				7U,
				8U,
				9U,
				10U,
				11U,
				12U,
				13U,
				14U,
				15U,
				24U,
				25U
			};
		};

		static constexpr size_t num_registers = 32;
		static constexpr size_t num_coprocessors = coprocessor::max;

	private:

		using register_file = std::array<register_type, num_registers /*- 1*/>;

		// These are at the highest point to be easier on the JIT. < 127 B addresses are grand.
		alignas(64) register_file registers_{ 0 };
		flag                      flags_ = flag::none;

		uptr_guest                branch_target_ = 0;
		uptr_guest                program_counter_ = 0;
		mutable uptr_guest        load_link_ = 0;
	public:
		uint32                    user_value = 0; // TODO REPLACE WITH COP0
	private:
		uint64                    instruction_count_ = 0;
		uint64                    target_instructions_ = std::numeric_limits<uint64>::max();
		char*                     memory_ptr_ = nullptr;
		char*                     shadow_memory_ptr_ = nullptr;
		const usize_guest         memory_size_ = 0;
		const usize_guest         stack_size_ = 0;

	private:

		const std::array<std::unique_ptr<coprocessor>, num_coprocessors> coprocessors_;
		memory_source* const __restrict memory_source_ = nullptr;

	public: // TODO : clean up later, though not using std::variant most likely (exception overhead)
		std::variant<
			std::monostate,
			jit1* __restrict
		> jit_;
	private:

		const std::unique_ptr<statistics> statistics_;

		system* const guest_system_ = nullptr;

		CPU_Exception									  trapped_exception_ = {};

		#if !VEMIPS_TABLEGEN
		_no_unique
		interpreter::cache::hash_cache<0x100, 16, 2> instruction_cache_;
		#endif

		const llsc llsc_type_ : 2;
		const mmu mmu_type_ : 2;

		const bool readonly_exec_ : 1 = false;
		const bool ticked_ : 1 = false;
		const bool collect_stats_ : 1 = false;
		const bool disable_cti_ : 1 = false;
		const bool handles_execution_hazards_ : 1 = false;
		const bool handles_instruction_hazards_ : 1 = false;
		const bool handles_memory_hazards_ : 1 = false;
		const bool debugging_ : 1 = false;
		const bool requires_shadow_memory_ : 1 = false;
	public:
		bool in_jit : 1 = false;
	private:
		bool from_exception_ : 1 = false;

		/// How should misaligned addresses in loads/stores be handled?
		const misaligned_address_handling misaligned_address_handling_ : std::bit_width(std::to_underlying(misaligned_address_handling::keep)) = misaligned_address_handling::keep;

	public:
#pragma region dynamic recompiler support
		template <typename TType = int16>
		requires (std::is_integral_v<TType>)
		struct recompiler_offsets final
		{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winvalid-offsetof"
			// base
			const TType base        = value_assert<int16>(0                                      ); // TODO : handle multiple inheritance offsets?
			// flags
			const TType flags       = value_assert<int16>(offsetof(processor, flags_             ));
			// program counter
			const TType pc          = value_assert<int16>(offsetof(processor, program_counter_   ));
			// delay branch target
			const TType dbt         = value_assert<int16>(offsetof(processor, branch_target_     ));
			// load-link address
			const TType ll          = value_assert<int16>(offsetof(processor, load_link_         ));
			// instruction count
			const TType ic          = value_assert<int16>(offsetof(processor, instruction_count_ ));
			// registers
			const TType registers   = value_assert<int16>(offsetof(processor, registers_         ));
			// memory store address
			const TType memory_ptr  = value_assert<int16>(offsetof(processor, memory_ptr_        ));
			// memory store size
			const TType memory_size = value_assert<int16>(offsetof(processor, memory_size_       ));
			// user value
			const TType user_value  = value_assert<int16>(offsetof(processor, user_value         ));
#pragma clang diagnostic pop

			template <typename TIntegerType>
			requires (std::is_integral_v<TIntegerType>)
			static constexpr recompiler_offsets<TIntegerType> get(const std::make_signed_t<TIntegerType> offset = -128)
			{
				return {
					.base        = value_assert<TIntegerType>(recompiler_offsets{}.base        + offset),
					.flags       = value_assert<TIntegerType>(recompiler_offsets{}.flags       + offset),
					.pc          = value_assert<TIntegerType>(recompiler_offsets{}.pc          + offset),
					.dbt         = value_assert<TIntegerType>(recompiler_offsets{}.dbt         + offset),
					.ll          = value_assert<TIntegerType>(recompiler_offsets{}.ll          + offset),
					.ic          = value_assert<TIntegerType>(recompiler_offsets{}.ic          + offset),
					.registers   = value_assert<TIntegerType>(recompiler_offsets{}.registers   + offset),
					.memory_ptr  = value_assert<TIntegerType>(recompiler_offsets{}.memory_ptr  + offset),
					.memory_size = value_assert<TIntegerType>(recompiler_offsets{}.memory_size + offset),
					.user_value  = value_assert<TIntegerType>(recompiler_offsets{}.user_value  + offset),
				};
			}
		};
#pragma endregion
	public:
		[[nodiscard]]
		_pure // not technically pure, but shouldn't access global memory that's not off of `this` or `other`.
		_nothrow std::vector<std::string> compare(const processor& __restrict other) const noexcept;

		_nothrow _forceinline void increment_instruction_statistic(const char *name) const noexcept
		{
			if (collect_stats_)
			{
				++(statistics_->instructions[name]);
			}
		}

		_nothrow _forceinline void increment_jit_emulated_instruction_statistic(const char *name) const noexcept
		{
			if (collect_stats_)
			{
				++(statistics_->jit_emulated_instructions[name]);
			}
		}

		_nothrow _forceinline void increment_jit_transition_statistic() const noexcept
		{
			if (collect_stats_)
			{
				++(statistics_->jit_transitions);
			}
		}

		[[nodiscard]]
		_pure
		_nothrow _forceinline const statistics* get_statistics() const noexcept {
			return collect_stats_ ? &*statistics_ : nullptr;
		}

		[[nodiscard]]
		_pure
		_nothrow std::pair<const char*, usize> get_jit_max_instruction_size() const __restrict noexcept;

		[[nodiscard]]
		_pure
		_nothrow _forceinline uint64 get_instruction_count() const __restrict noexcept {
			return instruction_count_;
		}

		[[nodiscard]]
		_pure
		_nothrow _forceinline JitType get_jit_type() const __restrict noexcept {

			return std::visit(
				overloads {
					[](std::monostate) { return JitType::None; },
					[](jit1* __restrict) { return JitType::Jit; },
				},
				jit_
			);
		}

		_nothrow _forceinline void set_trapped_exception(const CPU_Exception & __restrict ex) noexcept {
			set_flags(flag::trapped_exception);
			trapped_exception_ = ex;
		}

		_nothrow _forceinline void set_trapped_exception(const CPU_Exception::Type ex_type, const uint32 code = 0) noexcept {
			set_trapped_exception(CPU_Exception{ ex_type, get_program_counter(), code });
		}

		struct options final {
			memory_source* __restrict mem_src = nullptr;
			char* mem_ptr = nullptr;
			char* shadow_mem_ptr = nullptr;
			system* guest_system = nullptr;
			JitType jit_type = JitType::Jit;
			llsc llsc_type = llsc::fine;
			mmu mmu_type = mmu::emulated;
			usize_guest stack = 0;
			usize_guest mem_size = 0;
			bool rox : 1 = false;
			bool collect_stats : 1 = false;
			bool disable_cti : 1 = false;
			bool ticked : 1 = false;
			bool debugging : 1 = false;
		};

		processor(const options& __restrict options);
		~processor();

		template <typename Self>
		[[nodiscard]]
		_pure
		_nothrow _forceinline auto get_coprocessor(this Self&& self, const uint32 idx) noexcept ->
			copy_qualifiers_ptr<decltype(self), coprocessor>
		{
			xassert(idx < self.coprocessors_.size());
			return std::forward<Self>(self).coprocessors_[idx].get();
		}

		template <typename Self>
		[[nodiscard]]
		_pure
		_nothrow _forceinline auto get_coprocessor(this Self&& self, const coprocessor::types type) noexcept ->
			copy_qualifiers_ptr<decltype(self), coprocessor>
		{
			xassert(coprocessor::is_valid(type));
			return std::forward<Self>(self).get_coprocessor(std::to_underlying(type));
		}
	private:
		template <std::underlying_type_t<coprocessor::types> Index>
		[[nodiscard]]
		_pure
		_nothrow _forceinline auto get_coprocessor_impl(this auto&& self) noexcept -> std::conditional_t<
			Index == std::to_underlying(coprocessor::types::floating_point),
			_make_qual(coprocessor1 *),
			_make_qual(coprocessor *)
		>
		requires (
			Index < std::tuple_size_v<decltype(coprocessors_)>
		)
		{
			_make_qual(coprocessor*) coprocessor = self.coprocessors_[Index].get();

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
		_pure
		_nothrow _forceinline auto get_coprocessor(this auto&& self) noexcept -> std::conditional_t<
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
		_pure
		_nothrow _forceinline auto get_coprocessor(this auto&& self) noexcept requires(
			coprocessor::is_valid(Type)
		)
		{
			static constexpr const auto index = std::to_underlying(Type);

			return self.template get_coprocessor_impl<index>();
		}

		template <typename Self>
		[[nodiscard]]
		_pure
		_nothrow _forceinline auto get_fpu_coprocessor(this Self&& self) noexcept
		{
			return std::forward<Self>(self).template get_coprocessor<coprocessor::types::floating_point>();
		}

		// Sets the program counter.
		template <bool SetFlag = false>
		_nothrow _forceinline void set_program_counter(const uptr_guest address) __restrict noexcept {
			program_counter_ = address;

			if constexpr (SetFlag)
			{
				set_flags(flag::pc_changed);
			}
		}

		// Gets the program counter.
		[[nodiscard]]
		_pure
		_nothrow _forceinline uptr_guest get_program_counter() const __restrict noexcept {
			return program_counter_;
		}

		[[nodiscard]]
		_pure
		_nothrow uptr_guest get_branch_target() const __restrict noexcept {
			return branch_target_;
		}

		[[nodiscard]]
		_pure
		_nothrow uint32 get_user_value() const __restrict noexcept {
			return user_value;
		}

		[[nodiscard]]
		_pure
		_nothrow bool supports_llsc() const __restrict noexcept {
			return llsc_type_ != llsc::none;
		}

		[[nodiscard]]
		_pure
		_nothrow llsc get_llsc_type() const __restrict noexcept {
			return llsc_type_;
		}

		[[nodiscard]]
		_pure
		_nothrow uptr_guest get_load_link() const __restrict noexcept {
			return load_link_;
		}

		_nothrow void set_load_link(const uptr_guest address) __restrict noexcept {
			load_link_ = address;
		}

		// Execute Clock
		void execute(uint64 clocks = 1);

		[[nodiscard]]
		_nothrow _forceinline _pure const register_file& get_register_file() const noexcept
		{
			return registers_;
		}

		// Get the register as a specific type
		template <typename T = register_type>
		[[nodiscard]]
		_pure
		_nothrow _forceinline T get_register(const uint32 idx) const noexcept {
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
				std::memcpy(&result, &registers_[idx /*- 1*/], sizeof(result));
				return result;
			}
		}

		// Set the register from a given type
		template <typename T = register_type>
		_nothrow _forceinline void set_register(const uint32 idx, const T value) noexcept {
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
					register_type dst;
					T             src;
				} caster{
					.src = value
				};
				registers_[idx /*- 1*/] = caster.dst;
			}
		}

		// Set up a delay branch for the processor to the given pointer
		_nothrow void delay_branch(uptr_guest pointer) __restrict noexcept;

		// Set up a compact branch for the processor to the given pointer
		_nothrow void compact_branch(uptr_guest pointer) __restrict noexcept;

		// Sets the next instruction as being in the forbidden slot
		_nothrow void set_no_cti() __restrict noexcept;

		[[nodiscard]]
		_pure
		_nothrow _forceinline bool get_no_cti() const __restrict noexcept {
			return get_flags(flag::no_cti);
		}

		// Sets the link register
		_nothrow void set_link(uptr_guest pointer) noexcept;

		// TODO : this needs to be a public API in `system`.
		_nothrow void memory_touched(uptr_guest pointer, usize_guest size) __restrict noexcept;
		_nothrow void memory_touched_jit(uptr_guest pointer, usize_guest size) __restrict noexcept;

	private:
		struct execution_state final
		{
			bool ticked;
			bool debugging;
			bool jit;
			bool collect_stats;
			bool instruction_hazards;
			mmu mmu_type;
			uint8 coprocessor_mask;

			template <bool Value>
			constexpr _func_const _forceinline _nothrow execution_state with_ticked() const noexcept { return { Value, debugging, jit, collect_stats, instruction_hazards, mmu_type, coprocessor_mask }; }

			template <bool Value>
			constexpr _func_const _forceinline _nothrow execution_state with_debugging() const noexcept { return { ticked, Value, jit, collect_stats, instruction_hazards, mmu_type, coprocessor_mask }; }

			template <bool Value>
			constexpr _func_const _forceinline _nothrow execution_state with_jit() const noexcept { return { ticked, debugging, Value, collect_stats, instruction_hazards, mmu_type, coprocessor_mask }; }

			template <bool Value>
			constexpr _func_const _forceinline _nothrow execution_state with_collect_stats() const noexcept { return { ticked, debugging, jit, Value, instruction_hazards, mmu_type, coprocessor_mask }; }

			template <bool Value>
			constexpr _func_const _forceinline _nothrow execution_state with_instruction_hazards() const noexcept { return { ticked, debugging, jit, collect_stats, Value, mmu_type, coprocessor_mask }; }

			template <mmu Value>
			constexpr _func_const _forceinline _nothrow execution_state with_mmu_type() const noexcept { return { ticked, debugging, jit, collect_stats, instruction_hazards, Value, coprocessor_mask }; }

			template <uint8 Value> requires ((8U - std::countl_zero(Value)) <= num_coprocessors)
			constexpr _func_const _forceinline _nothrow execution_state with_coprocessor_mask() const noexcept { return { ticked, debugging, jit, collect_stats, instruction_hazards, mmu_type, Value }; }
		};

		template <execution_state State>
		_forceinline void execute_internal(uint64 clocks = 1);

		_forceinline _nothrow void handle_llsc(const uptr_guest address) __restrict noexcept {
			switch (llsc_type_)
			{
				case llsc::fine: [[likely]] {
					if ((address & ~(load_link_align - 1)) == load_link_) [[unlikely]]
					{
						load_link_ = 0;
					}
					return;
				}

				case llsc::coarse: {
					load_link_ = 0;
					return;

				case llsc::none:
					return;
				}
			}

			xunreachable("unknown LLSC type");
		}

		[[nodiscard]]
		_nothrow std::optional<uptr_guest> mem_poke_host(uptr_guest address, usize_guest size) const __restrict noexcept;
		[[nodiscard]]
		_nothrow std::optional<uptr_guest> mem_fetch_host_internal(const char* __restrict source, void* __restrict dst, uptr_guest address, usize_guest size) const __restrict noexcept;
		template <bool ForInstruction>
		[[nodiscard]]
		_nothrow std::optional<uptr_guest> mem_fetch_host(void* __restrict dst, const uptr_guest address, const usize_guest size) const __restrict noexcept
		{
			return mem_fetch_host_internal(
				ForInstruction ? shadow_memory_ptr_ : memory_ptr_,
				dst,
				address,
				size
			);
		}
		[[nodiscard]]
		_nothrow std::optional<uptr_guest> mem_write_host(const void* src, uptr_guest address, usize_guest size) __restrict noexcept;

		template <usize_guest Size>
		[[nodiscard]]
		_func_const
		static _nothrow _forceinline bool zero_check_address(const uptr_guest address) noexcept
		{
			const sptr_guest signed_address = sptr_guest(address);

			return
				signed_address >= -ssize_guest(Size - 1) &&
				signed_address < 1;
		}

	public:
		template <typename T>
		void mem_poke(uptr_guest address) const __restrict {
			if (mmu_type_ == mmu::host) {
				// TODO : handle literal edge case - overflows 32-bit address space
				if (const auto result = mem_poke_host(address, sizeof(T)); result.has_value()) [[unlikely]]
				{
					throw_helper<CPU_Exception::Type::AdEL>(result.value() );
				}
				
				return;
			}
			else {
				if _unlikely(zero_check_address<sizeof(T)>(address)) [[unlikely]] {
					throw_helper<CPU_Exception::Type::AdEL>(address);
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					if _unlikely(nullptr != memory_source_->at(address, sizeof(T))) [[unlikely]] {
						return;
					}
				}
				else if _unlikely(address + sizeof(T) <= memory_size_) [[unlikely]] {
					return;
				}
			}

			// error path
			[&] _noinline {
				if (mmu_type_ == mmu::emulated)
				{
					if (
						const std::optional<uptr_guest> invalid_address = memory_source_->get_first_unreadable(address, sizeof(T));
						invalid_address.has_value()
					)
					{
						throw_helper<CPU_Exception::Type::AdEL>(invalid_address.value() );
					}
				}
				else
				{
					const usize_guest stack_offset = mmu_type_ != mmu::host ? stack_size_ : 0;

					for (usize_guest i = 0; i < sizeof(T); ++i)
					{
						if (
							const uptr_guest offset_address = address + i;
							offset_address + stack_offset >= memory_size_ || offset_address == 0U
						)
						{
							throw_helper<CPU_Exception::Type::AdEL>(offset_address );
						}
					}
				}

				throw_helper<CPU_Exception::Type::AdEL>(address );
			}();
		}

		template <typename T, bool ForInstruction>
		[[nodiscard]]
		_nothrow std::expected<T, uptr_guest> try_mem_fetch_except(uptr_guest address) const __restrict noexcept {
			if (mmu_type_ == mmu::host) {
				// TODO : handle literal edge case - overflows 32-bit address space
				std::remove_const_t<T> result;
				if (
					const auto result_error = mem_fetch_host<ForInstruction>(&result, address, sizeof(T));
					result_error.has_value()
				) [[unlikely]]
				{
					return std::unexpected(result_error.value());
				}

				return result;
			}
			else {
				if _unlikely(zero_check_address<sizeof(T)>(address)) [[unlikely]] {
					return std::unexpected(0U);
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					const T* __restrict val_ptr = static_cast<const T * __restrict>(
						ForInstruction ?
							memory_source_->at_instruction(address, sizeof(T)) :
							memory_source_->at(address, sizeof(T))
					);
					if _likely(val_ptr) [[likely]] {
						return *val_ptr;
					}
				}
				else if _likely(address + sizeof(T) <= memory_size_) [[likely]] {
					return *reinterpret_cast<const T * __restrict>(
						(ForInstruction ? shadow_memory_ptr_ : memory_ptr_) + address
					);
				}
			}

			// error path
			return [&] _noinline /*_nothrow () noexcept*/ {
				if (mmu_type_ == mmu::emulated)
				{
					if (
						const std::optional<uptr_guest> invalid_address = memory_source_->get_first_unreadable(address, sizeof(T));
						invalid_address.has_value()
					)
					{
						return std::unexpected(invalid_address.value());
					}
				}
				else
				{
					const usize_guest stack_offset = mmu_type_ != mmu::host ? stack_size_ : 0;

					for (usize_guest i = 0; i < sizeof(T); ++i)
					{
						if (
							const uptr_guest offset_address = address + i;
							offset_address + stack_offset>= memory_size_ || offset_address == 0U
						)
						{
							return std::unexpected(offset_address);
						}
					}
				}

				return std::unexpected(address);
			}();
		}

		template <typename T>
		[[nodiscard]]
		_nothrow std::optional<T> try_mem_fetch(const uptr_guest address) const __restrict noexcept {
			if (
				const auto result = try_mem_fetch_except<T, false>(address);
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
		_nothrow std::optional<T> try_mem_fetch_instruction(const uptr_guest address) const __restrict noexcept {
			if (
				const auto result = try_mem_fetch_except<T, true>(address);
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
		T mem_fetch(const uptr_guest address) const __restrict {
			if (
				const auto result = try_mem_fetch_except<T, false>(address);
				result.has_value()
			) [[likely]]
			{
				return result.value();
			}
			else
			{
				throw_helper<CPU_Exception::Type::AdEL>(result.error() );
			}
		}

		template <typename T>
		[[nodiscard]]
		T mem_fetch_instruction(const uptr_guest address) const __restrict {
			if (
				const auto result = try_mem_fetch_except<T, true>(address);
				result.has_value()
			) [[likely]]
			{
				return result.value();
			}
			else
			{
				throw_helper<CPU_Exception::Type::AdEL>(result.error());
			}
		}

		template <typename T>
		[[nodiscard]]
		_nothrow const T * mem_fetch_debugger(uptr_guest address) const __restrict noexcept {
			if (mmu_type_ == mmu::host)
			{
				// TODO : handle literal edge case - overflows 32-bit address space
				T result;
				if (
					const auto result_error = mem_fetch_host<false>(&result, address, sizeof(T));
					result_error.has_value()
				) [[unlikely]]
				{
					return nullptr;
				}

				return reinterpret_cast<const T * __restrict>(memory_ptr_ + address);
			}
			else {
				if _unlikely(zero_check_address<sizeof(T)>(address)) [[unlikely]] {
					return nullptr;
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					const T* __restrict val_ptr = static_cast<const T * __restrict>(memory_source_->at(address, sizeof(T)));
					if _likely(val_ptr) [[likely]] {
						return val_ptr;
					}
				}
				else if _likely(address + sizeof(T) <= memory_size_) [[likely]] {
					return reinterpret_cast<const T * __restrict>(memory_ptr_ + address);
				}
			}
			return nullptr;
		}

		template <typename T>
		[[nodiscard]]
		_nothrow const T * __restrict safe_mem_fetch_exec(uptr_guest address) const __restrict noexcept {
			if (mmu_type_ == mmu::host) {
				// TODO : handle literal edge case - overflows 32-bit address space
				std::remove_const_t<T> result;
				if (
					const auto result_error = mem_fetch_host<true>(&result, address, sizeof(T));
					result_error.has_value()
				) [[unlikely]]
				{
					return nullptr;
				}

				return reinterpret_cast<const T * __restrict>(shadow_memory_ptr_ + address);
			}
			else {
				if _unlikely(zero_check_address<sizeof(T)>(address)) [[unlikely]] {
					return nullptr;
				}

				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					const T* __restrict val_ptr = static_cast<const T * __restrict>(memory_source_->at_exec(address, sizeof(T)));
					if _likely(val_ptr) [[likely]] {
						return val_ptr;
					}
				}
				else if _likely(address + sizeof(T) <= memory_size_) [[likely]] {
					return reinterpret_cast<const T * __restrict>(shadow_memory_ptr_ + address);
				}
			}

			return nullptr;
		}

		template <bool FromShadow = false>
		[[nodiscard]]
		_nothrow const void* get_memory_address(uptr_guest address) const __restrict noexcept {
			if (mmu_type_ == mmu::host)
			{
				if _likely(address >= memory_size_) [[unlikely]] {
					return nullptr;
				}

				return (FromShadow ? shadow_memory_ptr_ : memory_ptr_) + address;
			}
			else
			{
				address += stack_size_;

				if (mmu_type_ == mmu::emulated)
				{
					if constexpr (FromShadow)
					{
						return memory_source_->at_instruction(address, 1);
					}
					else
					{
						return memory_source_->at(address, 1);
					}
				}
				else
				{
					if _likely(address >= memory_size_) [[unlikely]] {
						return nullptr;
					}

					return (FromShadow ? shadow_memory_ptr_ : memory_ptr_) + address;
				}
			}
		}

		template <typename T>
		[[nodiscard]]
		_nothrow std::optional<uptr_guest> try_mem_write_except(uptr_guest address, T value) __restrict noexcept {
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
				if _unlikely(zero_check_address<sizeof(T)>(address)) [[unlikely]] {
					return 0U;
				}

				const uptr_guest unshifted_address = address;
				address += stack_size_;

				if (mmu_type_ == mmu::emulated) {
					*static_cast<T * __restrict>(memory_source_->write_at(address, sizeof(T))) = value;
					memory_touched(unshifted_address, sizeof(T));
					return {};
				}
				else if _likely(address + sizeof(T) <= memory_size_) [[likely]] {
					*reinterpret_cast<T * __restrict>(memory_ptr_ + address) = value;
					memory_touched(unshifted_address, sizeof(T));
					return {};
				}
			}

			// error path
			return [&] _noinline /*_nothrow () noexcept*/ {
				if (mmu_type_ == mmu::emulated)
				{
					if (
						const std::optional<uptr_guest> invalid_address = memory_source_->get_first_unreadable(address, sizeof(T));
						invalid_address.has_value()
					)
					{
						return invalid_address.value();
					}
				}
				else
				{
					const usize_guest stack_offset = mmu_type_ != mmu::host ? stack_size_ : 0;

					for (usize_guest i = 0; i < sizeof(T); ++i)
					{
						if (
							const uptr_guest offset_address = address + 1;
							offset_address + stack_offset >= memory_size_ || offset_address == 0U
						)
						{
							return offset_address;
						}
					}
				}

				return address;
			}();
		}

		struct mem_fetch_state final
		{
			mmu mmu_type;
			bool for_instruction;
		};

		// TODO : coalesce all flags
		template <typename T, mem_fetch_state State>
		[[nodiscard, gnu::flatten]]
		_nothrow _forceinline std::expected<T, uptr_guest> mem_fetch_universal_noexcept(uptr_guest address) const __restrict noexcept {
			switch (State.mmu_type)
			{
				case mmu::none:
					{
						if _unlikely(zero_check_address<sizeof(T)>(address)) [[unlikely]] {
							return std::unexpected(0U);
						}

						address += stack_size_;

						if _likely(address + sizeof(T) <= memory_size_) [[likely]] {
							return *reinterpret_cast<const T * __restrict>(
								(State.for_instruction ? shadow_memory_ptr_ : memory_ptr_) + address
							);
						}

						break;
					}

				case mmu::host:
					{
						// TODO : handle literal edge case - overflows 32-bit address space
						std::remove_const_t<T> result;
						if (
							const auto result_error = mem_fetch_host<State.for_instruction>(&result, address, sizeof(T));
							result_error.has_value()
						) [[unlikely]]
						{
							return std::unexpected(result_error.value());
						}

						return result;
					}

				case mmu::emulated:
					{
						if _unlikely(zero_check_address<sizeof(T)>(address)) [[unlikely]] {
							return std::unexpected(0U);
						}

						address += stack_size_;

						const T* __restrict val_ptr = static_cast<const T * __restrict>(
							State.for_instruction ?
								memory_source_->at_instruction(address, sizeof(T)) :
								memory_source_->at(address, sizeof(T))
						);

						if _likely(val_ptr) [[likely]] {
							return *val_ptr;
						}

						break;
					}
			}

			// error path
			return [&] _noinline /*_nothrow () noexcept*/ {
				if constexpr (State.mmu_type == mmu::emulated)
				{
					if (
						const std::optional<uptr_guest> invalid_address = memory_source_->get_first_unreadable(address, sizeof(T));
						invalid_address.has_value()
					)
					{
						return std::unexpected(invalid_address.value());
					}
				}
				else
				{
					const usize_guest stack_offset = State.mmu_type != mmu::host ? stack_size_ : 0;

					for (usize_guest i = 0; i < sizeof(T); ++i)
					{
						if (
							const uptr_guest offset_address = address + i;
							offset_address + stack_offset>= memory_size_ || offset_address == 0U
						)
						{
							return std::unexpected(offset_address);
						}
					}
				}

				return std::unexpected(address);
			}();
		}

		template <typename T, mem_fetch_state State>
		[[nodiscard, gnu::flatten]]
		_forceinline T mem_fetch_universal(const uptr_guest address) const __restrict {
			if (
				const auto result = mem_fetch_universal_noexcept<T, State>(address);
				result.has_value()
			) [[likely]]
			{
				return result.value();
			}
			else
			{
				throw_helper<CPU_Exception::Type::AdEL>(result.error());
			}
		}

		template <typename T>
		void mem_write(uptr_guest address, T value) __restrict {
			const auto result = try_mem_write_except(address, value);
			if (result.has_value()) [[unlikely]]
			{
				throw_helper<CPU_Exception::Type::AdES>(result.value());
			}
		}

		// TODO : move stack size add to JIT itself
		[[nodiscard]]
		_nothrow uintptr get_mem_write_jit(uptr_guest address, const usize_guest size) __restrict noexcept {
			const uptr_guest unshifted_address = address;
			if (mmu_type_ != mmu::host) {
				address += stack_size_;
			}
			const uintptr val_ptr = reinterpret_cast<uintptr>(memory_source_->write_at(address, size));
			if _likely(val_ptr) [[likely]] {
				memory_touched(unshifted_address, size);
			}
			return val_ptr;
		}

		// TODO : move stack size add to JIT itself
		[[nodiscard]]
		_nothrow uintptr get_mem_read_jit(uptr_guest address, const usize_guest size) const __restrict noexcept {
			if (mmu_type_ != mmu::host) {
				address += stack_size_;
			}
			const uintptr val_ptr = reinterpret_cast<uintptr>(memory_source_->at(address, size));
			return val_ptr;
		}

		[[nodiscard]]
		_pure
		_nothrow bool get_flags(const flag bits) const __restrict noexcept;

		_nothrow void set_flags(const flag bits) __restrict noexcept;

		_nothrow void clear_flags(const flag bits) __restrict noexcept;

		[[nodiscard]]
		_pure
		_nothrow _forceinline flag get_all_flags() const __restrict noexcept
		{
			return flags_;
		}

		_nothrow _forceinline void set_all_flags(const flag flags) __restrict noexcept
		{
			flags_ = flags;
		}

		[[nodiscard]]
		_pure
		_nothrow _forceinline misaligned_address_handling get_misaligned_address_handling() const __restrict noexcept
		{
			return misaligned_address_handling_;
		}

		[[nodiscard]]
		// ReSharper disable once CppMemberFunctionMayBeStatic
		_nothrow _forceinline _func_const bool has_coprocessor0_support() const __restrict noexcept
		{
			return false;
		}

		[[nodiscard]]
		_pure
		_nothrow _forceinline system* get_guest_system() const __restrict noexcept {
			return guest_system_;
		}

		[[nodiscard]]
		_nothrow _pure uint32 get_cache_line_size() const __restrict noexcept;

		_nothrow void invalidate_instruction_cache(uptr_guest guest_address) noexcept;
		_nothrow void clear_memory_hazards(memory_hazards hazards) noexcept;
		_nothrow _pure bool handles_memory_hazards(memory_hazards hazards) const noexcept;
		_nothrow _forceinline _pure bool handles_execution_hazards() const noexcept {
			return handles_execution_hazards_;
		}
		_nothrow _forceinline _pure bool handles_instruction_hazards() const noexcept{
			return handles_instruction_hazards_;
		}
		_nothrow _forceinline _pure bool handles_memory_hazards() const noexcept{
			return handles_memory_hazards_;
		}
		_nothrow void clear_execution_hazards() noexcept;
		_nothrow bool clear_instruction_hazards(guest_span chunk) noexcept;
		_nothrow bool clear_instruction_hazards_inline(flag& __restrict flags) noexcept;
		_nothrow bool clear_pending_instruction_hazards() noexcept;

	private:
		_pragma_small_code
		template <CPU_Exception::Type ExceptionType>
		[[noreturn]]
		_cold _noinline void throw_helper(const uint32 code) const __restrict
		{
			throw CPU_Exception(ExceptionType, get_program_counter(), code);
		}
		_pragma_default_code

		_pragma_small_code
		template <CPU_Exception::Type ExceptionType>
		[[noreturn]]
		_cold _noinline void throw_helper() const __restrict
		{
			throw CPU_Exception(ExceptionType, get_program_counter(), 0);
		}
		_pragma_default_code

		friend struct processor_executor;
	};

	MAKE_BITFLAG_ENUM(processor::flag)

	[[nodiscard]]
	_pure _nothrow inline bool processor::get_flags(const flag bits) const __restrict noexcept {
		return !!(flags_ & bits);
	}

	_nothrow inline void processor::set_flags(const flag bits) __restrict noexcept {
		flags_ = flags_ | bits;
	}

	_nothrow inline void processor::clear_flags(const flag bits) __restrict noexcept {
		flags_ = flags_ & ~bits;
	}
}
