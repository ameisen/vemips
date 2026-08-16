#include "pch.hpp"

#include "processor.hpp"
#include <common.hpp>

#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <type_traits>
#include <variant>
#include <fmt/format.h>

#include "mips/debugger.hpp"
#include "mips/exception.hpp"
#include "mips/interpreter/cache/hash_cache.hpp"
#include "mips/mips_common.hpp"
#include "mips/mmu.hpp"
#include "mips/system.hpp"
#include "mips/coprocessor/coprocessor.hpp"
#include "mips/coprocessor/coprocessor1/coprocessor1.hpp"
#include "mips/instructions/instructions.hpp"
#include "mips/instructions/instructions_support.hpp"
#if !VEMIPS_TABLEGEN
#	include "instructions/instructions_table.hpp"
#	include "mips/processor/jit/jit1/jit1.hpp"
#	include "global_state.hpp"
#endif

#include "platform/platform_headers.hpp"


using namespace mips;

#if !USE_STATIC_INSTRUCTION_SEARCH
namespace mips::instructions {
	extern void finish_map_build();
}
#endif

processor::processor(const options & __restrict options)
	// if there is no MMU, we need to set up some basic pointers.
	: memory_ptr_(options.mmu_type != mmu::emulated ? options.mem_ptr : nullptr)
	, shadow_memory_ptr_(options.mmu_type != mmu::emulated ? value_or(options.shadow_mem_ptr, options.mem_ptr) : nullptr)
	, memory_size_(options.mmu_type != mmu::emulated ? options.mem_size : 0)
	, stack_size_(options.stack)
	, coprocessors_{{
		{},
		std::make_unique<coprocessor1>(*this), // Set up coprocessor 1 (FPU)
		{},
		{}
	}}
	, memory_source_(options.mem_src)
	, statistics_(options.collect_stats ? std::make_unique<decltype(statistics_)::element_type>() : nullptr)
	, guest_system_(options.guest_system)
	, llsc_type_(options.llsc_type)
	, mmu_type_(options.mmu_type)
	, readonly_exec_(options.rox)
	, ticked_(options.ticked)
	, collect_stats_(options.collect_stats)
	, disable_cti_(options.disable_cti)
	, handles_execution_hazards_(false)
#if !VEMIPS_TABLEGEN
	, handles_instruction_hazards_(false && guest_system_->requires_instruction_cache_directory())
#endif
	, handles_memory_hazards_(false)
	, debugging_(options.debugging)
#if !VEMIPS_TABLEGEN
	, requires_shadow_memory_(guest_system_->requires_shadow_memory())
#endif
{
	xassert(guest_system_ != nullptr);

#if !USE_STATIC_INSTRUCTION_SEARCH
	mips::instructions::finish_map_build();
#endif
	if (memory_source_) {
		memory_source_->register_processor(this);
	}

#if !VEMIPS_TABLEGEN
	switch (options.jit_type) {
		case JitType::None:
			jit_ = std::monostate{};
			break;
		case JitType::Jit:
			jit_ = new jit1(*this);
			break;
		default:
			xunreachable("unknown jit");
	}
#endif
}

processor::~processor() {
	if (memory_source_) {
		memory_source_->unregister_processor(this);
	}

#if !VEMIPS_TABLEGEN
	if (!std::holds_alternative<std::monostate>(jit_))
	{
		std::visit([](auto&& jit)
		{
			if constexpr (!std::is_same_v<std::remove_cvref_t<decltype(jit)>, std::monostate>)
			{
				delete jit;
			}
		}, jit_);
	}
#endif
}

_nothrow void processor::delay_branch(const uptr_guest pointer) __restrict noexcept {
	branch_target_ = pointer;
	set_flags(flag::branch_delay | flag::no_cti);
}

_nothrow void processor::compact_branch(const uptr_guest pointer) __restrict noexcept {
	set_program_counter<true>(pointer);
}

_nothrow void processor::set_no_cti() __restrict noexcept {
	set_flags(flag::no_cti);
}

_nothrow void processor::set_link(const uptr_guest pointer) noexcept {
	registers_[31] = pointer;
}

namespace {
	template <typename T, bool Enabled = true>
	class updater final {
		T* __restrict variable_ptr_ = nullptr;
		T new_value_ = 0;

	public:
		[[nodiscard]]
		_nothrow updater() noexcept = default;
		[[nodiscard]]
		updater(const updater&) = delete;
		[[nodiscard]]
		_nothrow _forceinline updater(updater&& __restrict other) noexcept
			: variable_ptr_(other.variable_ptr_)
			, new_value_(other.new_value_)
		{
			other.variable_ptr_ = nullptr;
		}
		[[nodiscard]]
		_nothrow _forceinline updater(T* __restrict ptr, const T value) noexcept
			: variable_ptr_(ptr)
			, new_value_(value)
		{}

		updater& operator=(const updater&) = delete;
		_nothrow _forceinline updater& operator=(updater&& other) noexcept
		{
			if ((variable_ptr_ = other.variable_ptr_)) [[likely]]
			{
				other.variable_ptr_ = nullptr;
			}
			new_value_ = other.new_value_;

			return *this;
		}

		_nothrow _forceinline ~updater() noexcept {
			if (variable_ptr_) [[likely]] {
				(*variable_ptr_) = new_value_;
			}
		}

		_nothrow _forceinline void reset(T* pointer, const T& value) noexcept
		{
			variable_ptr_ = pointer;
			new_value_ = value;
		}

		_nothrow _forceinline void reset(T* pointer) noexcept
		{
			variable_ptr_ = pointer;
		}

		_nothrow _forceinline void reset() noexcept
		{
			variable_ptr_ = nullptr;
		}

		[[nodiscard]] explicit _pure
		_nothrow _forceinline operator bool() const noexcept
		{
			return variable_ptr_ != nullptr;
		}
	};

	template <typename T>
	class updater<T, false> final {
	public:
		[[nodiscard]]
		_nothrow updater() noexcept = default;
		[[nodiscard]]
		updater(const updater&) = delete;
		[[nodiscard]]
		_nothrow updater(updater&& __restrict other) noexcept = default;
		[[nodiscard]]
		_nothrow _forceinline updater(T* __restrict ptr, const T value) noexcept {}
		updater& operator=(const updater&) = delete;
		_nothrow updater& operator=(updater&& other) noexcept = default;
		_nothrow ~updater() noexcept = default;
		// ReSharper disable CppMemberFunctionMayBeStatic
		_nothrow _forceinline void reset(T* pointer, const T& value) noexcept {}
		_nothrow _forceinline void reset(T* pointer) noexcept {}
		_nothrow _forceinline void reset() noexcept {}
		// ReSharper restore CppMemberFunctionMayBeStatic
		[[nodiscard]] explicit _pure
		_nothrow _forceinline operator bool() const noexcept { return false; }
	};

	template <typename T, uint32 count>
	class incrementer final {
		T* __restrict variable_ptr_ = nullptr;

	public:
		[[nodiscard]]
		_nothrow incrementer() noexcept = default;
		[[nodiscard]]
		incrementer(const incrementer&) = delete;
		[[nodiscard]]
		_nothrow _forceinline incrementer(incrementer&& __restrict other) noexcept
			: variable_ptr_(other.variable_ptr_)
		{
			other.variable_ptr_ = nullptr;
		}

		incrementer& operator=(const incrementer&) = delete;

		_nothrow _forceinline incrementer& operator=(incrementer&& other) noexcept
		{
			if ((variable_ptr_ = other.variable_ptr_)) [[likely]]
			{
				other.variable_ptr_ = nullptr;
			}

			return *this;
		}

		_nothrow _forceinline ~incrementer() noexcept {
			if (T* ptr = variable_ptr_) [[likely]] {
				(*ptr) += count;
			}
		}

		_nothrow _forceinline void reset(T* pointer = nullptr) noexcept
		{
			variable_ptr_ = pointer;
		}
	};

	struct cti_clear final {
		processor * __restrict processor = nullptr;

		[[nodiscard]]
		_nothrow _forceinline cti_clear() noexcept = default;
		cti_clear(const cti_clear&) = delete;
		_nothrow _forceinline cti_clear(cti_clear&& other) noexcept :
			processor(other.processor)
		{
			other.processor = nullptr;
		}
		cti_clear& operator=(const cti_clear&) = delete;

		_nothrow _forceinline cti_clear& operator=(cti_clear&& other) noexcept
		{
			if ((processor = other.processor)) [[likely]]
			{
				other.processor = nullptr;
			}

			return *this;
		}

		_nothrow _forceinline ~cti_clear() noexcept {
			if (auto* const ptr = processor) {
				ptr->clear_flags(processor::flag::no_cti);
			}
		}
	};
}

namespace mips {
	struct processor_executor final
	{
		processor_executor() = delete;

		template <processor::execution_state State>
		[[gnu::flatten]]
		static _forceinline void execute_chain6(processor& processor, const uint64 clocks)
		{
			_block_forceinline
			return processor.execute_internal<State.with_coprocessor_mask<0b0010>()>(clocks);
		}

		template <processor::execution_state State>
		[[gnu::flatten]]
		static _forceinline void execute_chain5(processor& processor, const uint64 clocks)
		{
			switch (processor.mmu_type_)
			{
				case mmu::emulated:
					execute_chain6<State.with_mmu_type<mmu::emulated>()>(processor, clocks);
					return;
				case mmu::host:
					execute_chain6<State.with_mmu_type<mmu::host>()>(processor, clocks);
					return;
				case mmu::none:
					execute_chain6<State.with_mmu_type<mmu::none>()>(processor, clocks);
					return;
				default:
					xunreachable("Unknown MMU");
			}
		}

		template <processor::execution_state State>
		[[gnu::flatten]]
		static _forceinline void execute_chain4(processor& processor, const uint64 clocks)
		{
			if (processor.handles_instruction_hazards()) _block_forceinline
			{
				execute_chain5<State.with_instruction_hazards<true>()>(processor, clocks);
			}
			else _block_forceinline
			{
				execute_chain5<State.with_instruction_hazards<false>()>(processor, clocks);
			}
		}

		template <processor::execution_state State>
		[[gnu::flatten]]
		static _forceinline void execute_chain3(processor& processor, const uint64 clocks)
		{
			if _unlikely(processor.collect_stats_) [[unlikely]] _block_forceinline
			{
				execute_chain4<State.with_collect_stats<true>()>(processor, clocks);
			}
			else _block_forceinline
			{
				execute_chain4<State.with_collect_stats<false>()>(processor, clocks);
			}
		}

		template <processor::execution_state State>
		[[gnu::flatten]]
		static _forceinline void execute_chain2(processor& processor, const uint64 clocks)
		{
			if (!std::holds_alternative<std::monostate>(processor.jit_)) _block_forceinline
			{
				execute_chain3<State.with_jit<true>()>(processor, clocks);
			}
			else _block_forceinline
			{
				execute_chain3<State.with_jit<false>()>(processor, clocks);
			}
		}

		template <processor::execution_state State>
		[[gnu::flatten]]
		static _forceinline void execute_chain1(processor& processor, const uint64 clocks)
		{
			if _unlikely(processor.debugging_) [[unlikely]] _block_forceinline
			{
				execute_chain2<State.with_debugging<true>()>(processor, clocks);
			}
			else _block_forceinline
			{
				execute_chain2<State.with_debugging<false>()>(processor, clocks);
			}
		}

		[[gnu::flatten]]
		static _forceinline void execute_chain0(processor& processor, const uint64 clocks)
		{
			if (processor.ticked_) _block_forceinline
			{
				execute_chain1<processor::execution_state{}.with_ticked<true>()>(processor, clocks);
			}
			else _block_forceinline
			{
				execute_chain1<processor::execution_state{}.with_ticked<false>()>(processor, clocks);
			}
		}
	};
}

[[gnu::flatten]]
void processor::execute(const uint64 clocks)
{
#if !VEMIPS_TABLEGEN
	const auto scoped_processor_state = global_state::processor::get_scoped<true>(this);

	_block_forceinline
	processor_executor::execute_chain0(*this, clocks);
#endif
}

namespace
{
	template <typename TFunction>
	class on_scope_exit final
	{
		TFunction functor;

	public:
		_nothrow _forceinline on_scope_exit(TFunction&& _functor) noexcept :
			functor(std::move(_functor))
		{}

		_nothrow ~on_scope_exit() noexcept
		{
			functor();
		}
	};
}

template <processor::execution_state State>
_forceinline void processor::execute_internal(const uint64 clocks) {
#if !VEMIPS_TABLEGEN
	if constexpr (State.ticked) {
		target_instructions_ = instruction_count_ + clocks;
	}

	const mips::system* const __restrict guest_system = State.debugging ? guest_system_ : nullptr;
	auto& instruction_cache = instruction_cache_;

	uptr_guest program_counter = program_counter_;

	const auto fetch_instruction = [this] _forceinline_lambda((const uptr_guest address) noexcept)
	{
		return mem_fetch_universal_noexcept<
			const instruction_t,
			mem_fetch_state{
              .mmu_type = State.mmu_type,
              .for_instruction = true
			}
		>(address);
	};

	using instruction_cache_t = std::decay_t<decltype(instruction_cache)>;
	const instruction_cache_t::line* __restrict cache_line =
		(instruction_cache_t::is_indexable && !State.jit) ?
			&instruction_cache.get_cache_line(program_counter, fetch_instruction) :
			nullptr;
	uptr_guest cache_line_base_ptr = cache_line ? cache_line->base_address : 0;

	_clang_pragma(clang diagnostic push)
	_clang_pragma(clang diagnostic ignored "-Wignored-attributes")
	while (_likely(!State.ticked || _likely(instruction_count_ < target_instructions_))) [[likely]] {
	_clang_pragma(clang diagnostic pop)
	{
		xassert(registers_[0] == 0); // $0 is _always_ 0

		//const on_scope_exit reload_program_counter {[&, this] () noexcept {
		// TODO : Ideally we put this at the end... but that actually dramatically slows this down...
		program_counter = program_counter_;
		//}};

		if constexpr (State.coprocessor_mask != 0)
		{
			const std::unique_ptr<coprocessor>* const coprocessors = coprocessors_.data();
			for (uint32 i = 0; i < coprocessors_.size(); ++i)
			{
				if (State.coprocessor_mask & make_bit<uint8>(i))
				{
					if (
						auto* const cop = coprocessors[i].get();
						cop && cop->needs_clock()
					)
					{
						cop->clock();
					}
				}
			}
		}

		incrementer<uptr_guest, 4>                        program_counter_incrementer;
		updater<uptr_guest>                               program_counter_updater;
		updater<uptr_guest, ALWAYS_REFRESH_BRANCH_TARGET> branch_target_updater;
		// ReSharper disable once CppTooWideScope
		cti_clear                                         cti_delay_updater;

		scoped_flag flags { this };

		if constexpr (State.jit) {
			if (from_exception_) {
				if (enumeration::has_all_clear(*flags, flag::branch_delay)) {
					program_counter = branch_target_;
				}
				else {
					program_counter += 4;
				}
				program_counter_ = program_counter;
				from_exception_ = false;
			}
		}
		else {
			const bool branch_delay = enumeration::has_all_clear(*flags, flag::branch_delay);

			if (branch_delay)
			{
				program_counter_incrementer.reset(nullptr);
				program_counter_updater.reset(&program_counter_, branch_target_);
				branch_target_updater.reset(&branch_target_, 0u);
				//branch_target_ = 0u;
			}
			else
			{
				program_counter_incrementer.reset(&program_counter_);
				program_counter_updater.reset(nullptr, program_counter + 4);
			}

			if (enumeration::has_all(*flags, flag::no_cti)) {
				cti_delay_updater.processor = this;
			}

			++instruction_count_;
		}

		{
			try {
				if (
					State.instruction_hazards &&
					// TODO FIXME : I don't think that this is quite right - the delay branch IH should propagate to just IH when it is out of scope.
					enumeration::has_all_masked(
						*flags,
						flag::instruction_hazard | flag::instruction_hazard_delay_branch,
						flag::instruction_hazard
					)
				)
				{
					clear_instruction_hazards_inline(flags);
				}

				flags.flush();

				if ((program_counter % 4) != 0) [[unlikely]] {
					throw_helper<CPU_Exception::Type::RI>();
				}

				if constexpr (State.jit) {
					// if this is a branch delay slot, force interpretive mode. Fall through to the interpreter.
					std::visit(
						[this, &flags, program_counter](auto&& __restrict jit)
						{
							if constexpr (
								// clang rules around '__restrict' are very strange, and don't match MSVC or GCC.
								std::is_same_v<decltype(jit), std::monostate& __restrict> ||
								std::is_same_v<decltype(jit), std::monostate&> 
							)
							{
								xunreachable("State.jit is true but not JIT is present");
								return;
							}
							else
							{
								{
									const auto scoped_jit_state = global_state::jit::get_scoped<true>(jit);
									jit->execute_instruction(program_counter);
								}

								if (enumeration::has_all_clear(*flags, flag::trapped_exception)) [[unlikely]] {
									trapped_exception_.rethrow_helper();
								}
							}
						},
						jit_
					);

					// TODO : handle case where JIT intentionally drops back to interpreter within an instruction
				}
				else
				{
					if constexpr (instruction_cache_t::is_indexable)
					{
						xassert(cache_line != nullptr);

						if (!instruction_cache_t::line::contains_static(cache_line_base_ptr, program_counter)) [[unlikely]]
						{
							cache_line = &instruction_cache.get_cache_line(
								program_counter,
								fetch_instruction
							);
							cache_line_base_ptr = cache_line->base_address;
						}

						const uint32 line_offset = instruction_cache_t::get_index_from_ptr(program_counter);

						const auto entry = cache_line->data[line_offset];

						if constexpr (State.collect_stats)
						{
							if (instruction_cache_t::is_valid_index(entry.index)) [[likely]]
							{
								const auto& __restrict info = instructions::AllInstructions[entry.index].get();
								increment_instruction_statistic(info.Name);
							}
						}

						entry.execute_instruction(
							program_counter,
							*this
						);
					}
					else if constexpr (State.collect_stats)
					{
						instruction_t instruction_idx;

						try
						{
							instruction_idx = instruction_cache.execute_instruction_ret(
								program_counter,
								*this,
								fetch_instruction
							);
						}
						catch (const CPU_Exception& __restrict)
						{
							if (
								const auto entry = instruction_cache.get_entry(program_counter, fetch_instruction);
								instruction_cache_t::is_valid_index(entry.index)
							)
							{
								const auto& __restrict info = instructions::AllInstructions[entry.index].get();
								increment_instruction_statistic(info.Name);
							}

							throw;
						}

						const auto& __restrict info = instructions::AllInstructions[instruction_idx].get();
						increment_instruction_statistic(info.Name);
					}
					else
					{
						instruction_cache.execute_instruction(
							program_counter,
							*this,
							fetch_instruction
						);
					}
				}

				flags.reload();
			}
			catch (const CPU_Exception& __restrict ex) {
				//if constexpr (HasFlag(InsT::Flags, OpFlags::WritesGPRegister))
				{
					// Clear zero register
					_block_forceinline
					set_register(0, 0U);
				}

				program_counter_ = program_counter;

				from_exception_ = true;
				try {
					guest_system_->handle_exception(ex);
				}
				catch (...) {
					if constexpr (State.jit) {
						++instruction_count_;
					}
					program_counter_incrementer.reset();
					throw;
				}
			}
		}

		// TODO : only call me per-instruction, except for JIT
		// If the program counter has changed through a compact branch, then do NOT increment the program counter.
		if (enumeration::has_all_clear(*flags, flag::pc_changed)) [[unlikely]] {
			program_counter_updater.reset();
			program_counter_incrementer.reset();
		}

		// Were we interrupted by the debugger? If so, drop to caller.
		if constexpr (State.debugging)
		{
			if (guest_system->get_debugger()->should_pause()) [[unlikely]] {
				return;
			}
		}
		}
	}
#endif
}

_nothrow void processor::memory_touched(const uptr_guest pointer, const usize_guest size) __restrict noexcept {
	handle_llsc(pointer);

	if (readonly_exec_) {
		return;
	}
}

_nothrow void processor::memory_touched_jit(const uptr_guest pointer, const usize_guest size) __restrict noexcept {
	handle_llsc(pointer);
}

namespace
{
	static _nothrow usize_guest get_address_offset(const void* const address, const char* const base) noexcept
	{
		const intptr diff = static_cast<const char* const>(address) - base;
		xassert(diff >= 0 && static_cast<uintptr>(diff) <= std::numeric_limits<usize_guest>::max());
		return static_cast<usize_guest>(diff);
	}
}

_nothrow std::optional<uptr_guest> processor::mem_poke_host(const uptr_guest address, const usize_guest size) const __restrict noexcept
{
	xassert(mmu_type_ == mmu::host);

	LPEXCEPTION_POINTERS exception_info = {};
	__try {
		const volatile char* source = memory_ptr_;
		for (usize_guest i = 0; i < size; ++i)
		{
			[[maybe_unused]] char temp = source[uptr_guest(address + i)];
		}

		return {};
	}
	// EXCEPTION_CONTINUE_EXECUTION
	__except([] (const LPEXCEPTION_POINTERS ex) noexcept
	{
		switch (ex->ExceptionRecord->ExceptionCode)
		{
			case STATUS_ACCESS_VIOLATION:
			case EXCEPTION_IN_PAGE_ERROR:
				return EXCEPTION_EXECUTE_HANDLER;
			default: [[unlikely]]
				return EXCEPTION_CONTINUE_SEARCH;
		}
	}(exception_info = GetExceptionInformation()))
	{
		const void* const exception_address = exception_info->ExceptionRecord->ExceptionAddress;
		const usize_guest exception_offset = get_address_offset(exception_address, memory_ptr_);

		return exception_offset;
	}
}

_nothrow std::optional<uptr_guest> processor::mem_fetch_host_internal(const char* __restrict const source, void* __restrict const dst, const uptr_guest address, const usize_guest size) const __restrict noexcept
{
	xassert(mmu_type_ == mmu::host);

	LPEXCEPTION_POINTERS exception_info = {};
	// TODO : handle literal edge case - overflows 32-bit address space
	__try {
		std::memcpy(
			dst,
			source + address,
			size
		);

		return {};
	}
	__except([] (const LPEXCEPTION_POINTERS ex) noexcept
	{
		switch (ex->ExceptionRecord->ExceptionCode)
		{
			case STATUS_ACCESS_VIOLATION:
			case EXCEPTION_IN_PAGE_ERROR:
				return EXCEPTION_EXECUTE_HANDLER;
			default: [[unlikely]]
				return EXCEPTION_CONTINUE_SEARCH;
		}
	}(exception_info = GetExceptionInformation()))
	{
		const void* const exception_address = exception_info->ExceptionRecord->ExceptionAddress;
		const usize_guest exception_offset = get_address_offset(exception_address, source);

		return exception_offset;
	}
}

_nothrow std::optional<uptr_guest> processor::mem_write_host(const void* const src, const uptr_guest address, const usize_guest size) __restrict noexcept
{
	xassert(mmu_type_ == mmu::host);

	LPEXCEPTION_POINTERS exception_info = {};
	// TODO : handle literal edge case - overflows 32-bit address space
	__try {
		std::memcpy(
			memory_ptr_ + address,
			src,
			size
		);
		memory_touched(address, size);

		return {};
	}
	__except([] (const LPEXCEPTION_POINTERS ex) noexcept
	{
		switch (ex->ExceptionRecord->ExceptionCode)
		{
			case STATUS_ACCESS_VIOLATION:
			case EXCEPTION_IN_PAGE_ERROR:
				return EXCEPTION_EXECUTE_HANDLER;
			default: [[unlikely]]
				return EXCEPTION_CONTINUE_SEARCH;
		}
	}(exception_info = GetExceptionInformation()))
	{
		const void* const exception_address = exception_info->ExceptionRecord->ExceptionAddress;
		const usize_guest exception_offset = get_address_offset(exception_address, memory_ptr_);

		// TODO : this is likely in violation of llsc::fine
		memory_touched(address, exception_offset - address);

		return exception_offset;
	}
}

_nothrow std::pair<const char*, usize> processor::get_jit_max_instruction_size() const __restrict noexcept {
#if !VEMIPS_TABLEGEN
	return std::visit(
		overloads {
			[](std::monostate) -> std::pair<const char*, usize> { return { nullptr, 0 }; },
			[](auto&& __restrict jit) -> std::pair<const char*, usize>
			{
				return jit->get_max_instruction_size();
			}
		},
		jit_
	);
#else
	return { nullptr, 0 };
#endif
}

_nothrow uint32 processor::get_cache_line_size() const __restrict noexcept
{
#if !VEMIPS_TABLEGEN
	return std::visit(
		overloads {
			[](std::monostate) -> uint32 { return 0x100U; }, // 64/0x80 would be standard, but the JIT uses 0x100
			[](auto&& __restrict jit) -> uint32
			{
				return jit->get_chunk_size();
			}
		},
		jit_
	);
#else
	return 0U;
#endif
}

_nothrow void processor::invalidate_instruction_cache(const uptr_guest guest_address) noexcept
{
#if !VEMIPS_TABLEGEN
	if (!handles_instruction_hazards_)
	{
		return;
	}

	const uint32 cache_line_size = get_cache_line_size();
	instruction_cache_.invalidate(guest_address, cache_line_size);
#endif
}

_nothrow void processor::clear_memory_hazards(const memory_hazards hazards) noexcept
{
	
}

_nothrow bool processor::handles_memory_hazards(const memory_hazards hazards) const noexcept
{
	return (hazards & (memory_hazards::global_barrier_SYNCI)) != memory_hazards::none;
}

_nothrow void processor::clear_execution_hazards() noexcept
{
	// do nothing
	// handles_execution_hazards_
}

_nothrow bool processor::clear_instruction_hazards(const guest_span chunk) noexcept
{
	xassert(!in_jit || !chunk.is_empty());

	if (!handles_instruction_hazards_)
	{
		return false;
	}

	scoped_flag flags { *this };
	if (!enumeration::has_all_clear(flags.get(), processor::flag::instruction_hazard))
	{
		return false;
	}

	return false;
}

// TODO : __restrict might be UB since it's possible to pass processor::flags_...
_nothrow bool processor::clear_instruction_hazards_inline(flag& __restrict flags) noexcept
{
	xassert(!in_jit);

	if (!handles_instruction_hazards_)
	{
		return false;
	}

	if (!enumeration::has_all_clear(flags, processor::flag::instruction_hazard))
	{
		return false;
	}

	return false;
}

_nothrow bool processor::clear_pending_instruction_hazards() noexcept
{
	if (!handles_instruction_hazards_)
	{
		return false;
	}

	bool any_cleared = false;

	return any_cleared;
}

_nothrow std::vector<std::string> processor::compare(const processor& __restrict other) const noexcept {
	std::vector<std::string> result;
	
	if (std::memcmp(registers_.data(), other.registers_.data(), registers_.size()) != 0)
	{
		for (size_t i = 0; i < registers_.size(); ++i) {
			if _unlikely(registers_[i] != other.registers_[i]) [[unlikely]] {
				fmt::println(stderr, "Register {:2} Mismatch: {:08X} != {:08X}", i, registers_[i], other.registers_[i]);
				result.emplace_back(fmt::format("register[{:2}] Mismatch: {:08X} != {:08X}", i, registers_[i], other.registers_[i]));
			}
		}
	}
	if _unlikely(/*!from_exception_ && */(program_counter_ != other.program_counter_)) [[unlikely]] {
		result.emplace_back(fmt::format("program counter mismatch: {:08X} != {:08X}", program_counter_, other.program_counter_));
	}
	if _unlikely(instruction_count_ != other.instruction_count_) [[unlikely]] {
		result.emplace_back(fmt::format("instruction count mismatch: {} != {}", instruction_count_, other.instruction_count_));
	}
	if _unlikely(target_instructions_ != other.target_instructions_) [[unlikely]] {
		result.emplace_back(fmt::format("target instructions mismatch: {} != {}", target_instructions_, other.target_instructions_));
	}
	
	const flag flags = get_all_flags();
	const flag other_flags = other.get_all_flags();

	if (flags != other_flags) [[unlikely]] {
		result.emplace_back(fmt::format(
			"flag mismatch: {:08X} != {:08X}",
			std::to_underlying(flags),
			std::to_underlying(other_flags)
		));
	}

	const bool has_branch_delay = enumeration::has_all(flags, flag::branch_delay);

	// ReSharper disable once CppUnreachableCode, CppVariableCanBeMadeConstexpr
	const bool test_branch_target = ALWAYS_REFRESH_BRANCH_TARGET || has_branch_delay;

	if _unlikely(
		test_branch_target &&
		branch_target_ != other.branch_target_
	) [[unlikely]] {
		result.emplace_back(fmt::format(
			"branch target mismatch: {:08X} != {:08X}",
			branch_target_,
			other.branch_target_
		));
	}

	for (size_t i = 0; i < coprocessors_.size(); ++i)
	{
		if ((coprocessors_[i].get() != nullptr) != (other.coprocessors_[i].get() != nullptr))
		{
			result.emplace_back(fmt::format(
				"coprocessor presence mismatch: cop{} [{}] != cop{} [{}]",
				i, coprocessors_[i].get() != nullptr,
				i, other.coprocessors_[i].get() != nullptr
			));
			continue;
		}

		if (coprocessors_[i].get() == nullptr)
		{
			continue;
		}

		if (
			auto coprocessor_result = coprocessors_[i]->compare(*other.coprocessors_[i].get());
			!coprocessor_result.empty()
		)
		{
			result.emplace_back(fmt::format("coprocessor {} mismatch", i));
			result.append_range(std::move(coprocessor_result));
		}
	}

	return result;
}
