#include "pch.hpp"

#include "system.hpp"
#include "common.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <memory>
#include <new>
#include <span>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <fmt/format.h>

#include "debugger.hpp"
#include "memory_source.hpp"
#include "mmu.hpp"
#include "statistics.hpp"
#include "elf/elf.hpp"
#include "platform/win32/host_mmu.hpp"
#include "processor/processor.hpp"
#include "processor/jit/jit.hpp"


using namespace mips;

namespace
{
	static constexpr char dummy_memory_value = char(0xFE);
}

class sys_memory_source final : public mips::memory_source {
	std::unique_ptr<char[]> m_Memory; // includes shadow memory for instructions
	std::vector<mips::processor *> m_RegisteredProcessors;
	std::vector<std::pair<uptr_guest, usize_guest>> m_ExecutableBlocks;
	usize_guest memory_size_;
public:
	explicit sys_memory_source(const bool with_shadow_memory, const usize_guest size)
		: m_Memory(std::make_unique_for_overwrite<char[]>(
			size +
			(with_shadow_memory ? size : 0U)
		))
		, memory_size_(size)
	{
	}
	virtual ~sys_memory_source() override = default;

private:
	[[nodiscard]]
	_forceinline
	static _func_const _nothrow const void* at_impl(
		const char* const memory,
		const usize_guest memory_size,
		const uptr_guest offset,
		const usize_guest size
	) noexcept
	{
		const usize end_offset = usize(offset) + size;
		if _unlikely(end_offset > memory_size) [[unlikely]] {
			return nullptr;
		}
		return memory + offset;
	}

public:
	[[nodiscard]]
	virtual _pure _nothrow void * get_ptr() noexcept override {
		return get_memory_ptr();
	}

	[[nodiscard]]
	virtual _pure _nothrow const void * get_ptr() const noexcept override {
		return get_memory_ptr();
	}

	[[nodiscard]]
	virtual _pure _nothrow void * get_shadow_ptr() noexcept override {
		return get_shadow_memory_ptr();
	}

	[[nodiscard]]
	virtual _pure _nothrow const void * get_shadow_ptr() const noexcept override {
		return get_shadow_memory_ptr();
	}

	[[nodiscard]]
	_pure _forceinline _nothrow const char* get_memory_ptr() const noexcept
	{
		return m_Memory.get();
	}

	[[nodiscard]]
	_pure _forceinline _nothrow char* get_memory_ptr() noexcept
	{
		return m_Memory.get();
	}

	[[nodiscard]]
	_pure _forceinline _nothrow const char* get_shadow_memory_ptr() const noexcept
	{
		return get_memory_ptr() + memory_size_;
	}

	[[nodiscard]]
	_pure _forceinline _nothrow char* get_shadow_memory_ptr() noexcept
	{
		return get_memory_ptr() + memory_size_;
	}

	[[nodiscard]]
	virtual _pure _nothrow usize_guest get_size() const noexcept override {
		return memory_size_;
	}

	[[nodiscard]]
	virtual _pure _nothrow bool is_readable(const uptr_guest offset) const noexcept override {
		return at(offset, 1) != nullptr;
	}

	[[nodiscard]]
	virtual _pure _nothrow bool is_writable(const uptr_guest offset) const noexcept override {
		if (at(offset, 1) == nullptr) {
			return false;
		}

		// Check Executable Blocks
		for (auto&& pair : m_ExecutableBlocks) {
			if _unlikely(offset >= pair.first && offset + 1 <= pair.second) [[unlikely]] {
				return false;
			}
		}

		return true;
	}

	[[nodiscard]]
	virtual _pure _nothrow const void* at(const uptr_guest offset, const usize_guest size) const noexcept override {
		return at_impl(get_memory_ptr(), memory_size_, offset, size);
	}

	[[nodiscard]]
	virtual _pure _nothrow const void* at_instruction(const uptr_guest offset, const usize_guest size) const noexcept override {
		return at_impl(get_shadow_memory_ptr(), memory_size_, offset, size);
	}

	[[nodiscard]]
	virtual _pure _nothrow const void* at_exec(const uptr_guest offset, const usize_guest size) const noexcept override {
		const char* const memory = get_shadow_memory_ptr();

		const usize end_offset = usize(offset) + size;
		if _unlikely(end_offset > memory_size_) [[unlikely]] {
			return nullptr;
		}
		if (!m_ExecutableBlocks.empty()) {
			for (auto&& exec_block : m_ExecutableBlocks) {
				if (offset >= exec_block.first && end_offset <= exec_block.second) {
					return memory + offset;
				}
			}
		}
		else {
			return memory + offset;
		}
		return nullptr;
	}

	[[nodiscard]]
	virtual _pure _nothrow void* write_at(const uptr_guest offset, const usize_guest size) noexcept override {
		const usize end_offset = usize(offset) + size;
		if _unlikely(end_offset > memory_size_) [[unlikely]] {
			return nullptr;
		}
		// Check Executable Blocks
		for (auto&& pair : m_ExecutableBlocks) {
			if _unlikely(offset >= pair.first && end_offset <= pair.second) [[unlikely]] {
				return nullptr;
			}
		}
		return m_Memory.get() + offset;
	}

	virtual _nothrow void register_processor(mips::processor* cpu) noexcept override {
		m_RegisteredProcessors.push_back(cpu);
	}

	virtual _nothrow void unregister_processor(mips::processor* __restrict cpu) noexcept override {
		const auto find_iterator = std::find(m_RegisteredProcessors.begin(), m_RegisteredProcessors.end(), cpu);
		xassert(find_iterator != m_RegisteredProcessors.end());
		m_RegisteredProcessors.erase(find_iterator);
	}

	virtual _nothrow void set_executable_memory(const elf::binary& __restrict binary) noexcept override {
		for (auto&& section : binary.sections_) {
			if (section.executable) {
				m_ExecutableBlocks.emplace_back(section.memory_extent.offset, section.memory_extent.end_offset());
			}
		}
		std::ranges::stable_sort(m_ExecutableBlocks);
	}
};

namespace mips {
	namespace {
		class options_validation_exception final : std::exception {
			std::string message_;

		public:
			explicit options_validation_exception(const std::string& message) : message_(message) {
			}

			explicit options_validation_exception(std::string&& message) : message_(std::move(message)) {
			}

			options_validation_exception(const options_validation_exception&) = default;
			options_validation_exception(options_validation_exception&&) = default;

			options_validation_exception& operator=(const options_validation_exception&) = default;
			options_validation_exception& operator=(options_validation_exception&&) = default;

			//template <typename... Args>
			//explicit options_validation_exception(fmt::format_string<Args...> format, Args&&... args) : options_validation_exception(
			//	fmt::format(format, std::forward<Args>(args)...)
			//) {
			//}

			virtual ~options_validation_exception() noexcept override = default;

			[[nodiscard]]
			virtual _pure _nothrow const char* what () const noexcept override {
				return message_.c_str();
			}
		};
	}
}

void mips::system::options::validate() const {
	usize_guest required_align = memory_alignment;
	if (mmu_type == mmu::host) {
		required_align = 0x1000;
	}
	const usize_guest min_memory = align_up_pow2(0x10000u, required_align);
	constexpr const usize_guest min_stack = 0x1000u;

	if ((total_memory % required_align) != 0) [[unlikely]] {
		throw options_validation_exception(fmt::format("total memory ({}) is not aligned to required alignment of {}", total_memory, required_align));
	}

	if (total_memory < min_memory) [[unlikely]] {
		throw options_validation_exception(fmt::format("total memory ({}) is less than the minimum requirement of {}", total_memory, min_memory));
	}

	if (stack_memory && (stack_memory % required_align) != 0) [[unlikely]] {
		throw options_validation_exception(fmt::format("stack memory ({}) is not aligned to required alignment of {}", stack_memory, required_align));
	}

	if (stack_memory && stack_memory < min_stack) [[unlikely]] {
		throw options_validation_exception(fmt::format("total stack memory ({}) is less than the minimum requirement of {}", stack_memory, min_stack));
	}

	if (read_only_exec && mmu_type != mmu::emulated) [[unlikely]] {
		throw options_validation_exception("'rox' requires the emulated MMU to be enabled");
	}
}

void system::initialize(const elf::binary & __restrict binary) {
	usize_guest stack_offset = options_.stack_memory;

	char* __restrict mem_data;
	char* __restrict shadow_mem_data;
	usize_guest mem_size;
	if (options_.mmu_type == mmu::emulated) {
		stack_offset = 0; // I believe that this is right.
		mem_data = static_cast<char*>(memory_source_->get_ptr());
		shadow_mem_data = static_cast<char*>(memory_source_->get_shadow_ptr());
		mem_size = memory_source_->get_size();
	}
	else if (options_.mmu_type == mmu::host) {
		stack_offset = 0;
		mem_data = static_cast<char*>(host_mmu_->get_pointer());
		shadow_mem_data = static_cast<char*>(host_mmu_->get_shadow_pointer());
		mem_size = options_.total_memory;
	}
	else {
		mem_data = memory_.get();
		shadow_mem_data = requires_shadow_memory() ? (memory_.get() + options_.total_memory) : nullptr;
		mem_size = options_.total_memory;
	}

	// Initialize to a dummy value to make errors easier to find
	std::memset(mem_data, dummy_memory_value, mem_size);
	if (mem_data != shadow_mem_data && shadow_mem_data != nullptr)
	{
		std::memset(shadow_mem_data, dummy_memory_value, mem_size);
	}

	uptr_guest highest_used_address = 0;
	// Validate that there is enough memory to hold the binary.
	if (!binary.sections_.empty()) {
		const uptr_guest preadjusted_final_mapped_address = binary.sections_.back().memory_extent.end_offset();
		const uptr_guest final_mapped_address = preadjusted_final_mapped_address + stack_offset;
		highest_used_address = std::max(highest_used_address, preadjusted_final_mapped_address);
		if _unlikely(final_mapped_address > mem_size) [[unlikely]] {
			throw std::runtime_error("ELF binary's mappings are out of range for System");
		}
		if (options_.mmu_type == mmu::host) {
			if _unlikely(final_mapped_address + options_.stack_memory > mem_size) [[unlikely]] {
				throw std::runtime_error("ELF binary's mappings are out of range for System");
			}
		}

		for (auto&& section : binary.sections_) { // these are considered to be the 'not free memory' areas.
			const usize_guest offset = section.memory_extent.offset + stack_offset;

			// Initialize the sections.
			if (section.file_extent.size) {
				std::memcpy(
					mem_data + offset,
					binary.raw_data_stream_.address_offset(section.file_extent.offset),
					section.file_extent.size
				);
			}

			if (section.memory_extent.size > section.file_extent.size) {
				std::memset(
					mem_data + offset + section.file_extent.size,
					section.zero_init ? 0x00 : dummy_memory_value,
					usize(section.memory_extent.size) - section.file_extent.size
				);
			}
		}
	}

	system_break_ = system_break_base_ = align_up<sizeof(uptr_guest)>(highest_used_address);

	// validate that the entry address is at least somewhat sensible
	if _unlikely((binary.entry_address_ + options_.stack_memory) > mem_size) [[unlikely]] {
		throw std::runtime_error("ELF Entry Address is out of range of the system");
	}

	// Set program counter.
	processor_->set_program_counter(binary.entry_address_);

	// Set up stack.

	// Set stack pointer
	uptr_guest stack_start;
	if (options_.stack_memory == 0) {
		stack_start = mem_size;
	}
	else {
		// Stack now starts at top of memory due to splitting the address space.
		stack_start = stack_offset;
	}

	constexpr std::array<std::span<const char>, 2> arguments = { "vemips", "mips32r6" };
	std::array<uptr_guest, arguments.size()> argument_addresses;
	usize argument_idx = 0;
	for (auto&& argument : arguments) {
		const usize_guest strlenz = checked_cast<usize_guest>(argument.size() + 1);
		stack_start -= strlenz;
		argument_addresses[argument_idx++] = uptr_guest(stack_start - stack_offset);
		std::memcpy(mem_data + usize_guest(stack_start), argument.data(), strlenz);
	}

	if ((stack_start % 16) != 0) {
		stack_start -= (stack_start % 16);
	}

	constexpr auto argument_count = argument_addresses.size() /*- 1*/;

	constexpr usize stack_array_size = 38 + argument_count;

	constexpr uint32 hwcaps = 1; // HWCAP_MIPS_r6

	const auto stack_tuple = std::tuple_cat(
		std::array{uint32(argument_count)},
		argument_addresses,
		std::array{
			0U,  // arguments null terminator
			0U,  // envp null terminator
			32U, // AT_SYSINFO
			0U,  // pair
			33U, // AT_SYSINFO_EHDR
			0U,  // pair
			16U, // AT_HWCAP
			hwcaps,
			6U, // AT_PAGESZ
			0x100U,
			17U, // AT_CLKTCK
			100U,
			3U, // AT_PHDR
			binary.program_headers_,
			4U, // AT_PHENT
			binary.program_headers_size_,
			5U, // AT_PHNUM
			binary.program_header_count_,
			7U,  // AT_BASE
			0U,  // pair
			8U,  // AT_FLAGS
			0U,  // pair
			9U,  // AT_ENTRY
			binary.entry_address_,  // pair
			11U,  // AT_UID
			0U,
			12U,  // AT_EUID
			0U,  // pair
			13U,  // AT_GID
			0U,  // pair
			14U,  // AT_EGID
			0U,  // pair
			23U,  // AT_SECURE
			0U,  // pair
			15U,  // AT_PLATFORM
			argument_addresses.back(),  // pair
			0U
		}
	);

	const std::array<uptr_guest, stack_array_size> stack_array = std::apply(
		[](auto... n){ return std::array<uptr_guest, sizeof...(n)>{n...}; },
		stack_tuple
	);

	// align stack_start.
	constexpr usize stack_array_byte_size = usize(stack_array.size()) * sizeof(uptr_guest);
	constexpr usize align_req = 16 - (stack_array_byte_size % 16);
	if (align_req != 16) {
		stack_start -= align_req;
	}

	stack_start -= stack_array_byte_size;

	std::memcpy(mem_data + uptr_guest(stack_start), stack_array.data(), stack_array_byte_size);
	xassert((stack_start % 16) == 0);
	if (stack_offset) {
		stack_start -= stack_offset;
	}
	processor_->set_register<uptr_guest>(29, uptr_guest(stack_start));

	if (shadow_mem_data)
	{
		std::memcpy(shadow_mem_data, mem_data, mem_size);
	}
}

system::system(capabilities&& capabilities, const options & __restrict init_options, const elf::binary & __restrict binary)
	: options_(init_options)
	, capabilities_(std::move(capabilities))
{
	mips::processor::options cpu_options = {
		.guest_system = this,
		.jit_type = init_options.jit_type,
		.llsc_type = init_options.llsc_type,
		.mmu_type = init_options.mmu_type,
		.stack = init_options.stack_memory,
		.rox = init_options.read_only_exec,
		.collect_stats = init_options.collect_statistics,
		.disable_cti = init_options.disable_cti,
		.ticked = init_options.ticked,
		.debugging = init_options.debug
	};

	const bool with_shadow_memory = requires_shadow_memory();

	switch (init_options.mmu_type) {
	case mmu::emulated:
		memory_source_ = new sys_memory_source(with_shadow_memory, init_options.total_memory);
		cpu_options.mem_src = memory_source_;
		break;
	case mmu::host:
		host_mmu_ = new platform::host_mmu(with_shadow_memory, init_options.total_memory, init_options.stack_memory);
		cpu_options.mem_ptr = (char*)host_mmu_->get_pointer();
		cpu_options.shadow_mem_ptr = (char*)host_mmu_->get_shadow_pointer();
		cpu_options.mem_size = init_options.total_memory;
		break;
	default:
		const usize_guest base_total_memory = with_shadow_memory ?
			align_up<memory_alignment>(init_options.total_memory) :
			init_options.total_memory;

		memory_ = decltype(memory_){
			static_cast<char*>(::operator new[](
				usize(base_total_memory) +
				(with_shadow_memory ? base_total_memory : 0U),
				std::align_val_t{memory_alignment}
			))
		};
		cpu_options.mem_ptr = memory_.get();
		if (with_shadow_memory)
		{
			cpu_options.shadow_mem_ptr = memory_.get() + base_total_memory;
			shadow_memory_offset_ = base_total_memory;
		}
		cpu_options.mem_size = init_options.total_memory;
		break;
	}

	processor_ = new processor(cpu_options);
	initialize(binary);
	if (init_options.read_only_exec && memory_source_) {
		memory_source_->set_executable_memory(binary);
	}

	if (init_options.debug) {
		debugger_ = new debugger(init_options.debug_port, *this);
		fmt::println("** Waiting for debugger connection on port {}", init_options.debug_port);
		debugger_->wait();
		fmt::println("** Debugger attached.");
	}
}

system::system(const options & __restrict init_options, const elf::binary & __restrict binary)
	: system({}, init_options, binary)
{
}

system::~system() {
	delete debugger_;
	delete processor_;
	delete memory_source_;
	delete host_mmu_;
}

void system::clock(const uint64 clocks) __restrict {
	if _unlikely(!processor_) [[unlikely]] {
		return;
	}

	uint64 end_target = processor_->target_instructions_ + clocks;

	do {
		if _unlikely(debugger_ && debugger_->should_pause()) [[unlikely]] {
			debugger_->wait();
			if _unlikely(debugger_->should_kill()) [[unlikely]] {
				std::exit(1);
			}
		}
		processor_->execute(clocks);
	} while _likely(!options_.ticked || processor_->instruction_count_ < end_target);
}

_pure _nothrow uint64 system::get_instruction_count() const __restrict noexcept {
	return processor_->get_instruction_count();
}

_pure _nothrow const statistics* system::get_statistics() const __restrict noexcept {
	return processor_->get_statistics();
}

_pure _nothrow std::pair<const char*, size_t> system::get_jit_max_instruction_size() const __restrict noexcept {
	return processor_->get_jit_max_instruction_size();
}

_nothrow bool system::requires_instruction_cache_directory() const __restrict noexcept
{
	switch (options_.self_modifying_code)
	{
		case system::options::policy::undefined:
			return options_.jit_type != JitType::None;

		case system::options::policy::disabled:
			return false;

		default:
			return true;
	}
}

_pure _nothrow bool system::requires_shadow_memory() const __restrict noexcept
{
	switch (options_.self_modifying_code)
	{
		case system::options::policy::undefined:
		case system::options::policy::disabled: // TODO : was true for this to match JIT behavior
			return false;

		default:
			return true;
	}
}

_pure _nothrow std::vector<std::string> system::compare(const system& __restrict other) const noexcept
{
	std::vector<std::string> result;

	if (
		auto processor_result = processor_->compare(*other.processor_);
		!processor_result.empty()
	)
	{
		result.append_range(std::move(processor_result));
	}

	const auto compare_memory = [&] <bool Shadow> () {
		const auto memory_span = Shadow ? get_shadow_memory() : get_memory();
		const auto other_memory_span = Shadow ? other.get_shadow_memory() : other.get_memory();

		const usize_guest memory_size = checked_cast<usize_guest>(memory_span.size());
		const usize_guest other_memory_size = checked_cast<usize_guest>(other_memory_span.size());
		if (memory_size != other_memory_size)
		{
			result.emplace_back(fmt::format(
				"{}memory size mismatch: {} != {}", Shadow ? "shadow " : "",
				memory_size,
				other_memory_size
			));
		}

		if (memory_size == 0)
		{
			return;
		}

		// TODO : this can be sped up by tracking, during SXS, what is actually written.
		if (std::memcmp(memory_span.data(), other_memory_span.data(), memory_size) == 0)
		{
			return;
		}

		constexpr usize max_memory_results = 16;

		static_assert(memory_alignment > sizeof(uint32));

		const uint32* const __restrict memory_ptr = reinterpret_cast<const uint32* __restrict>(memory_span.data());
		const uint32* const __restrict other_memory_ptr = reinterpret_cast<const uint32* __restrict>(other_memory_span.data());

		std::vector<uptr_guest> memory_result;
		for (
			uptr_guest i = 0, ptr_index = 0;
			i < memory_size;
			i += sizeof(uint32), ++ptr_index
		)
		{
			if (memory_ptr[ptr_index] != other_memory_ptr[ptr_index])
			{
				memory_result.push_back(i);

				if (memory_result.size() >= max_memory_results)
				{
					break;
				}
			}
		}

		xassert(!memory_result.empty());

		if (memory_result.empty()) [[unlikely]]
		{
			result.emplace_back(fmt::format("{}memory mismatch", Shadow ? "shadow " : ""));
		}
		else
		{
			std::string memory_result_str = fmt::format("{}memory mismatches:", Shadow ? "shadow " : "");

			for (const uptr_guest address : memory_result)
			{
				memory_result_str += fmt::format(" {:08X}", address);
			}

			if (memory_result.size() >= max_memory_results)
			{
				memory_result_str += " ...";
			}

			result.emplace_back(std::move(memory_result_str));
		}
	};

	compare_memory.operator()<false>();
	compare_memory.operator()<true>();

	return result;
}
