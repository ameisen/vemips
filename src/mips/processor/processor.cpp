#include "pch.hpp"

#include "processor.hpp"
#include "mips/coprocessor/coprocessor1/coprocessor1.hpp"
#include "mips/instructions/instructions.hpp"
#if !VEMIPS_TABLEGEN
#include "mips/processor/jit/jit1/jit1.hpp"
#endif
#include "mips/system.hpp"

#if !VEMIPS_TABLEGEN
#include "instructions/instructions_table.hpp"
#endif

#include "instructions/instructions_support.hpp"
#include "platform/platform_headers.hpp"


using namespace mips;

namespace mips {
	namespace {
		thread_local processor* __restrict g_currentprocessor = nullptr;
		thread_local coprocessor* __restrict g_currentcoprocessor = nullptr;
	}

	processor* get_current_processor() {
		return g_currentprocessor;
	}

	coprocessor* get_current_coprocessor() {
		return g_currentcoprocessor;
	}

	void set_current_coprocessor(coprocessor* __restrict cop) {
		g_currentcoprocessor = cop;
	}


}

#if !USE_STATIC_INSTRUCTION_SEARCH
namespace mips::instructions {
	extern void finish_map_build();
}
#endif

processor::processor(const options & __restrict options) :
	// if there is no MMU, we need to set up some basic pointers.
	memory_ptr_(options.mmu_type != mmu::emulated ? options.mem_ptr : nullptr),
	shadow_memory_ptr_(options.mmu_type != mmu::emulated ? value_or(options.shadow_mem_ptr, options.mem_ptr) : nullptr),
	memory_size_(options.mmu_type != mmu::emulated ? options.mem_size : 0),
	stack_size_(options.stack),
	coprocessors_{{
		{},
		std::make_unique<coprocessor1>(*this), // Set up coprocessor 1 (FPU)
		{},
		{}
	}},
	memory_source_(options.mem_src),
	statistics_(options.collect_stats ? std::make_unique<decltype(statistics_)::element_type>() : nullptr),
	mmu_type_(options.mmu_type),
	guest_system_(options.guest_system),
	readonly_exec_(options.rox),
	ticked_(options.ticked),
	collect_stats_(options.collect_stats),
	disable_cti_(options.disable_cti),
	handles_execution_hazards_(false),
	handles_instruction_hazards_(
		(options.guest_system && options.guest_system->has_strict_noncoherence()) ||
		options.jit_type != JitType::None
	),
	debugging_(options.debugging)
{
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

void processor::delay_branch(const uint32 pointer) __restrict {
	branch_target_ = pointer;
	set_flags(flag::branch_delay | flag::no_cti);
}

void processor::compact_branch(const uint32 pointer) __restrict {
	set_program_counter(pointer);
	set_flags(flag::pc_changed);
}

void processor::set_no_cti() __restrict {
	set_flags(flag::no_cti);
}

void processor::set_link(const uint32 pointer) {
	registers_[31] = pointer;
}

instruction_t processor::get_instruction() const __restrict {
	if ((program_counter_ % 4) != 0) [[unlikely]] {
		CPU_Exception::throw_helper(CPU_Exception::Type::RI, program_counter_);
	}
	const instruction_t instruction_ptr = mem_fetch_instruction<const instruction_t>(program_counter_);

	return instruction_ptr;
}

template <typename T>
class updater final {
	T* __restrict variable_ptr = nullptr;
	T new_value = 0;

public:
	_nothrow updater() noexcept = default;
	updater(const updater&) = delete;
	_nothrow updater(updater&& __restrict other) noexcept
		: variable_ptr(other.variable_ptr)
		, new_value(other.new_value)
	{
		other.variable_ptr = nullptr;
	}

	updater& operator=(const updater&) = delete;
	_nothrow updater& operator=(updater&& other) noexcept
	{
		if ((variable_ptr = other.variable_ptr)) [[likely]]
		{
			other.variable_ptr = nullptr;
		}
		new_value = other.new_value;

		return *this;
	}

	_nothrow ~updater() noexcept {
		if (variable_ptr) [[likely]] {
			(*variable_ptr) = new_value;
		}
	}

	_nothrow void reset(T* pointer, const T& value) noexcept
	{
		variable_ptr = pointer;
		new_value = value;
	}

	_nothrow void reset(T* pointer) noexcept
	{
		variable_ptr = pointer;
	}

	_nothrow void reset() noexcept
	{
		variable_ptr = nullptr;
	}
};

template <typename T, uint32 count>
class incrementer final {
	T* __restrict variable_ptr = nullptr;

public:
	_nothrow incrementer() noexcept = default;
	incrementer(const incrementer&) = delete;
	_nothrow incrementer(incrementer&& __restrict other) noexcept
		: variable_ptr(other.variable_ptr)
	{
		other.variable_ptr = nullptr;
	}

	incrementer& operator=(const incrementer&) = delete;
	_nothrow incrementer& operator=(incrementer&& other) noexcept
	{
		if ((variable_ptr = other.variable_ptr)) [[likely]]
		{
			other.variable_ptr = nullptr;
		}

		return *this;
	}

	_nothrow ~incrementer() noexcept {
		if (variable_ptr) [[likely]] {
			(*variable_ptr) += count;
		}
	}

	_nothrow void reset(T* pointer = nullptr) noexcept
	{
		variable_ptr = pointer;
	}
};

// Sets the program counter. This is meant to be used as an external function to set up the CPU
void processor::set_program_counter(uint32 address) __restrict {
	program_counter_ = address;
}

// Gets the program counter
uint32 processor::get_program_counter() const __restrict {
	return program_counter_;
}

// REMOVE THIS WHEN UNIFIED INSTRUCTIONS ARE IN
void processor::ExecuteInstruction(const bool branch_delay) {
#if !VEMIPS_TABLEGEN
	try {
		g_currentprocessor = this;

		const bool has_jit = !std::holds_alternative<std::monostate>(jit_);

		if (has_jit && !branch_delay) {
			// if this is a branch delay slot, force interpretive mode. Fall through to the interpreter.
			std::visit([=, this](auto&& __restrict jit)
			{
				if constexpr (!std::is_same_v<std::remove_cvref_t<decltype(jit)>, std::monostate>)
				{
					jit->execute_instruction(program_counter_);
				}
			}, jit_);

			if (get_flags(flag::instruction_hazard))
			{
				clear_instruction_hazards();
			}
		}
		
		const instruction_t instruction = get_instruction();
		if _unlikely(collect_stats_) [[unlikely]] {
			if (const auto * __restrict info = instructions::get_instruction(instruction)) [[likely]] {
				increment_instruction_statistic(info->Name);
				info->Proc(instruction, *this);
			}
			else {
				trapped_exception_.rethrow_helper();
			}
		}
		else {
			if _unlikely(!instructions::execute_instruction(instruction, *this)) [[unlikely]] {
				CPU_Exception::throw_helper(CPU_Exception::Type::RI, this->get_program_counter());
			}
		}
		if (has_jit) {
			if (get_flags(flag::trapped_exception)) [[unlikely]] {
				trapped_exception_.rethrow_helper();
			}
		}
	}
	catch (const CPU_Exception& __restrict ex) {
		from_exception_ = true;
		guest_system_->handle_exception(ex);
	}
#endif
}

void processor::ExecuteInstructionExplicit(const instructions::InstructionInfo* instruction_info, const instruction_t instruction, bool branch_delay) {
#if !VEMIPS_TABLEGEN
	try {
		g_currentprocessor = this;

		if _unlikely(collect_stats_) [[unlikely]] {
			increment_instruction_statistic(instruction_info->Name);
		}
		instruction_info->Proc(instruction, *this);
	}
	catch (const CPU_Exception& __restrict ex) {
		from_exception_ = true;
		guest_system_->handle_exception(ex);
	}
#endif
}

struct cti_clear final {
	processor * __restrict processor = nullptr;

	~cti_clear() {
		if (processor) {
			processor->clear_flags(processor::flag::no_cti);
		}
	}
};

void processor::execute(const uint64 clocks) {
	if (ticked_) {
		if _unlikely(debugging_) [[unlikely]] {
			return execute_internal<true, true>(clocks);
		}
		else {
			return execute_internal<true, false>(clocks);
		}
	}
	else {
		if _unlikely(debugging_) [[unlikely]] {
			return execute_internal<false, true>(clocks);
		}
		else {
			return execute_internal<false, false>(clocks);
		}
	}
}

template <bool ticked, bool debugging>
_forceinline void processor::execute_internal(const uint64 clocks) {
	if constexpr (ticked) {
		target_instructions_ = instruction_count_ + clocks;
	}

	const mips::system* const __restrict guest_system = debugging ? guest_system_ : nullptr;

	while _likely(!ticked || instruction_count_ < target_instructions_) [[likely]] {
		xassert(registers_[0] == 0); // $0 is _always_ 0
		for (const std::unique_ptr<coprocessor>& cop : coprocessors_) {
			if (cop) {
				cop->clock();
			}
		}

		incrementer<uint32, 4>	program_counter_incrementer;
		updater<uint32>			 program_counter_updater;
		cti_clear					 cti_delay_updater;

		if (std::holds_alternative<std::monostate>(jit_)) {
			const bool branch_delay = get_flags(flag::branch_delay);

			program_counter_incrementer.reset(
				branch_delay ? nullptr : &program_counter_
			);

			program_counter_updater.reset(
				branch_delay ? &program_counter_ : nullptr,
				(!branch_delay) ? (program_counter_ + 4) : branch_target_
			);
			if (get_flags(flag::no_cti)) {
				cti_delay_updater.processor = this;
			}

			++instruction_count_;
			clear_flags(flag::branch_delay);
		}
		else {
			if (from_exception_) {
				if (get_flags(flag::branch_delay)) {
					clear_flags(flag::branch_delay);
					program_counter_ = branch_target_;
				}
				else {
					program_counter_ += 4;
				}
				from_exception_ = false;
			}
		}

		ExecuteInstruction(false);

		// If the program counter has changed through a compact branch, then do NOT increment the program counter.
		if (get_flags(flag::pc_changed)) {
			clear_flags(flag::pc_changed);
			program_counter_updater.reset();
			program_counter_incrementer.reset();
		}

		// Were we interrupted by the debugger? If so, drop to caller.
		if (debugging && guest_system->get_debugger()->should_pause()) {
			return;
		}
	}
}

bool processor::execute_explicit(const instructions::InstructionInfo* instruction_info, instruction_t instruction) {
	if _unlikely(ticked_ && instruction_count_ >= target_instructions_) [[unlikely]] {
		return false;
	}

	xassert(registers_[0] == 0); // $0 is _always_ 0
	for (const std::unique_ptr<coprocessor>& cop : coprocessors_) {
		if (cop) {
			cop->clock();
		}
	}

	incrementer<uint32, 4>	program_counter_incrementer;
	updater<uint32>			 program_counter_updater;
	cti_clear					 cti_delay_updater;

	const bool branch_delay = get_flags(flag::branch_delay);

	program_counter_incrementer.reset(
		branch_delay ? nullptr : &program_counter_
	);

	program_counter_updater.reset(
		branch_delay ? &program_counter_ : nullptr,
		((!branch_delay) ? (program_counter_ + 4) : branch_target_)
	);
	if (get_flags(flag::no_cti)) {
		cti_delay_updater.processor = this;
	}

	++instruction_count_;
	clear_flags(flag::branch_delay);

	ExecuteInstructionExplicit(instruction_info, instruction, false);

	// If the program counter has changed through a compact branch, then do NOT increment the program counter.
	if (get_flags(flag::pc_changed)) {
		clear_flags(flag::pc_changed);
		program_counter_updater.reset();
		program_counter_incrementer.reset();
	}

	// Were we interrupted by the debugger? If so, drop to caller.
	if (debugging_ && guest_system_->get_debugger()->should_pause()) {
		return false;
	}

	return true;
}

void processor::memory_touched(const uint32 pointer, uint32 size) const __restrict {
	if (readonly_exec_) {
		return;
	}
}

void processor::memory_touched_jit(uint32 pointer, uint32 size) const __restrict {
}

namespace
{
	static uint32 get_address_offset(const void* const address, const char* const base)
	{
		const intptr diff = static_cast<const char* const>(address) - base;
		xassert(diff >= 0 && diff <= std::numeric_limits<uint32>::max());
		return static_cast<uint32>(diff);
	}
}

std::optional<uint32> processor::mem_poke_host(const uint32 address, const uint32 size) const __restrict
{
	xassert(mmu_type_ == mmu::host);

	LPEXCEPTION_POINTERS exception_info = {};
	__try {
		const volatile char* source = memory_ptr_;
		for (uint32 i = 0; i < size; ++i)
		{
			[[maybe_unused]] char temp = source[uint32(address + i)];
		}

		return {};
	}
	// EXCEPTION_CONTINUE_EXECUTION
	__except([](const LPEXCEPTION_POINTERS ex)
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
		const uint32 exception_offset = get_address_offset(exception_address, memory_ptr_);

		return exception_offset;
	}
}

std::optional<uint32> processor::mem_fetch_host_internal(const char* __restrict const source, void* __restrict const dst, const uint32 address, const uint32 size) const __restrict
{
	xassert(mmu_type_ == mmu::host);

	LPEXCEPTION_POINTERS exception_info = {};
	// TODO : handle literal edge case - overflows 32-bit address space
	__try {
		memcpy(
			dst,
			source + address,
			size
		);

		return {};
	}
	__except([](const LPEXCEPTION_POINTERS ex)
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
		const uint32 exception_offset = get_address_offset(exception_address, source);

		return exception_offset;
	}
}

std::optional<uint32> processor::mem_write_host(const void* const src, const uint32 address, const uint32 size) const __restrict
{
	xassert(mmu_type_ == mmu::host);

	LPEXCEPTION_POINTERS exception_info = {};
	// TODO : handle literal edge case - overflows 32-bit address space
	__try {
		memcpy(
			memory_ptr_ + address,
			src,
			size
		);
		memory_touched(address, size);

		return {};
	}
	__except([](const LPEXCEPTION_POINTERS ex)
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
		const uint32 exception_offset = get_address_offset(exception_address, memory_ptr_);

		memory_touched(address, exception_offset - address);

		return exception_offset;
	}
}

size_t processor::get_jit_max_instruction_size() const __restrict {
#if !VEMIPS_TABLEGEN
	return std::visit(
		overloads {
			[](std::monostate) -> size_t { return 0; },
			[](auto&& __restrict jit) -> size_t
			{
				return jit->get_max_instruction_size();
			}
		},
		jit_
	);
#else
	return 0;
#endif
}

uint32 processor::get_cache_line_size() const __restrict
{
#if !VEMIPS_TABLEGEN
	return std::visit(
		overloads {
			[](std::monostate) -> uint32 { return 128U; },
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

void processor::invalidate_instruction_cache(const uint32 guest_address)
{
	if (!handles_instruction_hazards_)
	{
		return;
	}

	const uint32 cache_line_size = get_cache_line_size();
	const uint32 guest_address_line_start = guest_address & (~(cache_line_size - 1U));
}

void processor::clear_memory_hazards(const memory_hazards hazards)
{
	
}

bool processor::handles_memory_hazards(const memory_hazards hazards) const
{
	return (hazards & (memory_hazards::global_barrier_SYNCI)) != memory_hazards::none;
}

bool processor::handles_execution_hazards() const
{
	return handles_execution_hazards_;
}

bool processor::handles_instruction_hazards() const
{
	return handles_instruction_hazards_;
}

void processor::clear_execution_hazards()
{
	// do nothing
	// handles_execution_hazards_
}

bool processor::clear_instruction_hazards(const std::optional<std::pair<uint32, uint32>> current_chunk_span)
{
	xassert(!in_jit || current_chunk_span.has_value());

	if (!handles_instruction_hazards_)
	{
		return false;
	}

	if (!get_flags(processor::flag::instruction_hazard))
	{
		return false;
	}
	clear_flags(processor::flag::instruction_hazard);

	return false;
}
