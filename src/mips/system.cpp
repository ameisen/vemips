#include "pch.hpp"

#include "system.hpp"
#include "elf/elf.hpp"

#include <fmt/format.h>

#include <cassert>
#include <string>

using namespace mips;

namespace {
	template <typename T>
	static constexpr T align_up(T value, T align) {
		return (value + (align - 1)) & ~(align - 1);
	}
}

class sys_memory_source final : public mips::memory_source {
	std::vector<char> m_Memory;
	std::vector<mips::processor *> m_RegisteredProcessors;
	std::vector<std::pair<uint32, uint32>> m_ExecutableBlocks;
public:
	explicit sys_memory_source(const uint32 size) : m_Memory(size) {}
	virtual ~sys_memory_source() override = default;

	virtual void * get_ptr() override {
		return m_Memory.data();
	}

	virtual const void * get_ptr() const override {
		return m_Memory.data();
	}

	virtual uint32 get_size() const override {
		return uint32(m_Memory.size());
	}

	virtual bool is_readable(const uint32 offset) const override {
		return at(offset, 1) != nullptr;
	}

	virtual bool is_writable(const uint32 offset) const override {
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

	virtual const void * at(const uint32 offset, const uint32 size) const override {
		const size_t end_offset = size_t(offset) + size;
		if _unlikely(end_offset > uint32(m_Memory.size())) [[unlikely]] {
			return nullptr;
		}
		return m_Memory.data() + offset;
	}
	virtual const void * at_exec(const uint32 offset, const uint32 size) const override {
		const size_t end_offset = size_t(offset) + size;
		if _unlikely(end_offset > uint32(m_Memory.size())) [[unlikely]] {
			return nullptr;
		}
		if (!m_ExecutableBlocks.empty()) {
			for (auto&& exec_block : m_ExecutableBlocks) {
				if (offset >= exec_block.first && end_offset <= exec_block.second) {
					return m_Memory.data() + offset;
				}
			}
		}
		else {
			return m_Memory.data() + offset;
		}
		return nullptr;
	}
	virtual void * write_at(const uint32 offset, const uint32 size) override {
		const size_t end_offset = size_t(offset) + size;
		if _unlikely(end_offset > uint32(m_Memory.size())) [[unlikely]] {
			return nullptr;
		}
		// Check Executable Blocks
		for (auto&& pair : m_ExecutableBlocks) {
			if _unlikely(offset >= pair.first && end_offset <= pair.second) [[unlikely]] {
				return nullptr;
			}
		}
		return m_Memory.data() + offset;
	}

	virtual void register_processor(mips::processor * cpu) override {
		m_RegisteredProcessors.push_back(cpu);
	}

	virtual void unregister_processor(mips::processor * __restrict cpu) override {
		m_RegisteredProcessors.erase(std::find(m_RegisteredProcessors.begin(), m_RegisteredProcessors.end(), cpu));
	}

	virtual void set_executable_memory(const elf::binary & __restrict binary) override {
		for (auto&& section : binary.sections_) {
			if (section.executable) {
				m_ExecutableBlocks.emplace_back(section.memory_extent.offset, section.memory_extent.end_offset());
			}
		}
		std::stable_sort(m_ExecutableBlocks.begin(), m_ExecutableBlocks.end());
	}
};

namespace mips {
	namespace {
		class options_validation_exception final : std::exception {
			std::string message_;

		public:
			explicit options_validation_exception(const std::string &message) : message_(message) {
			}

			explicit options_validation_exception(std::string&& message) : message_(message) {
			}

			//template <typename... Args>
			//explicit options_validation_exception(fmt::format_string<Args...> format, Args&&... args) : options_validation_exception(
			//	fmt::format(format, std::forward<Args>(args)...)
			//) {
			//}

			virtual ~options_validation_exception() noexcept override = default;

			virtual const char * what () const noexcept override {
				return message_.c_str();
			}
		};
	}
}

void mips::system::options::validate() const {
	uint32 required_align = 1;
	if (mmu_type == mmu::host) {
		required_align = 0x1000;
	}
	const uint32 min_memory = align_up(0x10000u, required_align);
	constexpr const uint32 min_stack = 0x1000u;

	if ((total_memory % required_align) != 0) {
		throw options_validation_exception(fmt::format("total memory ({}) is not aligned to required alignment of {}", total_memory, required_align));
	}

	if (total_memory < min_memory) {
		throw options_validation_exception(fmt::format("total memory ({}) is less than the minimum requirement of {}", total_memory, min_memory));
	}

	if (stack_memory && (stack_memory % required_align) != 0) {
		throw options_validation_exception(fmt::format("stack memory ({}) is not aligned to required alignment of {}", stack_memory, required_align));
	}

	if (stack_memory && stack_memory < min_stack) {
		throw options_validation_exception(fmt::format("total stack memory ({}) is less than the minimum requirement of {}", stack_memory, min_stack));
	}

	if (read_only_exec && mmu_type != mmu::emulated) {
		throw options_validation_exception("'rox' requires the emulated MMU to be enabled");
	}
}

void system::initialize(const elf::binary & __restrict binary) {
	char * __restrict mem_data;
	uint32 mem_size;
	if (options_.mmu_type == mmu::emulated) {
		mem_data = (char *)memory_source_->get_ptr();
		mem_size = memory_source_->get_size();
	}
	else {
		mem_data = memory_.data();
		mem_size = uint32(memory_.size());
	}

	uint32 stack_offset = options_.stack_memory;
	if (options_.mmu_type == mmu::host) {
		stack_offset = 0;
		mem_data = (char *)host_mmu_->get_pointer();
		mem_size = options_.total_memory;
	}

	uint32 highest_used_address = 0;
	// Validate that there is enough memory to hold the binary.
	if (!binary.sections_.empty()) {
		const uint32 preadjusted_final_mapped_address = binary.sections_.back().memory_extent.end_offset();
		const uint32_t final_mapped_address = preadjusted_final_mapped_address + stack_offset;
		highest_used_address = std::max(highest_used_address, preadjusted_final_mapped_address);
		if _unlikely(final_mapped_address > mem_size) [[unlikely]] {
			throw std::runtime_error("ELF Binary's mappings are out of range for System");
		}
		if (options_.mmu_type == mmu::host) {
			if _unlikely(final_mapped_address + options_.stack_memory > mem_size) [[unlikely]] {
				throw std::runtime_error("ELF Binary's mappings are out of range for System");
			}
		}

		for (auto&& section : binary.sections_) { // these are considered to be the 'not free memory' areas.
			// Initialize the sections.
			if (section.file_extent.size) {
				memcpy(
					mem_data + section.memory_extent.offset + stack_offset,
					binary.raw_data_stream_.address_offset(section.file_extent.offset),
					section.file_extent.size
				);
			}
			if (section.memory_extent.size > section.file_extent.size) {
				memset(
					mem_data + section.memory_extent.offset + section.file_extent.size + stack_offset,
					0,
					section.memory_extent.size - section.file_extent.size
				);
			}
		}
	}
	system_break_ = highest_used_address;

	// validate that the entry address is at least somewhat sensible
	if _unlikely((binary.entry_address_ + options_.stack_memory) > mem_size) [[unlikely]] {
		throw std::runtime_error("ELF Entry Address is out of range of the system");
	}

	// Set program counter.
	processor_->set_program_counter(binary.entry_address_);

	// Set up stack.

	// Set stack pointer
	uint64 stack_start;
	if (options_.stack_memory == 0) {
		stack_start = mem_size;
	}
	else {
		// Stack now starts at top of memory due to splitting the address space.
		stack_start = stack_offset;
	}

	const std::array<const char * __restrict, 2> arguments = { "vemips", "mips32r6" };
	std::array<uint32, arguments.size()> argument_addresses;
	size_t argument_idx = 0;
	for (auto&& argument : arguments) {
		const size_t strlenz = strlen(argument) + 1;
		stack_start -= strlenz;
		argument_addresses[argument_idx++] = uint32(stack_start - stack_offset);
		memcpy(mem_data + uint32(stack_start), argument, strlenz);
	}

	if ((stack_start % 16) != 0) {
		stack_start -= (stack_start % 16);
	}

	constexpr auto argument_count = argument_addresses.size() - 1;

	std::vector<uint32> stack_vector;
	stack_vector.reserve(38 + argument_count);

	stack_vector.push_back(uint32(argument_count));
	for (size_t i = 0; i < argument_count; ++i) {
		stack_vector.push_back(argument_addresses[i]);
	}
	stack_vector.push_back(0); // arguments null terminator
	stack_vector.push_back(0); // envp null terminator
	stack_vector.push_back(32); // AT_SYSINFO
	stack_vector.push_back(0);  // pair
	stack_vector.push_back(33); // AT_SYSINFO_EHDR
	stack_vector.push_back(0);  // pair
	const uint32 hwcaps = 1; // HWCAP_MIPS_r6
	stack_vector.push_back(16); // AT_HWCAP
	stack_vector.push_back(hwcaps);
	stack_vector.push_back(6); // AT_PAGESZ
	stack_vector.push_back(0x100);
	stack_vector.push_back(17); // AT_CLKTCK
	stack_vector.push_back(100);
	stack_vector.push_back(3); // AT_PHDR
	stack_vector.push_back(binary.program_headers_);
	stack_vector.push_back(4); // AT_PHENT
	stack_vector.push_back(binary.program_headers_size_);
	stack_vector.push_back(5); // AT_PHNUM
	stack_vector.push_back(binary.program_header_count_);
	stack_vector.push_back(7);  // AT_BASE
	stack_vector.push_back(0);  // pair
	stack_vector.push_back(8);  // AT_FLAGS
	stack_vector.push_back(0);  // pair
	stack_vector.push_back(9);  // AT_ENTRY
	stack_vector.push_back(binary.entry_address_);  // pair
	stack_vector.push_back(11);  // AT_UID
	stack_vector.push_back(0);
	stack_vector.push_back(12);  // AT_EUID
	stack_vector.push_back(0);  // pair
	stack_vector.push_back(13);  // AT_GID
	stack_vector.push_back(0);  // pair
	stack_vector.push_back(14);  // AT_EGID
	stack_vector.push_back(0);  // pair
	stack_vector.push_back(23);  // AT_SECURE
	stack_vector.push_back(0);  // pair
	stack_vector.push_back(15);  // AT_PLATFORM
	stack_vector.push_back(argument_addresses.back());  // pair
	stack_vector.push_back(0);

	// align stack_start.
	const uint32 vector_size = uint32(stack_vector.size() * sizeof(uint32));
	uint32 align_req = 16 - (vector_size % 16);
	if (align_req == 16) {
		align_req = 0;
	}
	stack_start -= align_req;

	stack_start -= stack_vector.size() * sizeof(uint32);

	memcpy(mem_data + uint32(stack_start), stack_vector.data(), stack_vector.size() * sizeof(uint32));
	xassert((stack_start % 16) == 0);
	if (stack_offset) {
		stack_start -= stack_offset;
	}
	processor_->set_register<uint32>(29, uint32(stack_start));
}


system::system(const options & __restrict init_options, const elf::binary & __restrict binary) : options_(init_options)
{
	mips::processor::options cpu_options = {
		.guest_system = this,
		.jit_type = init_options.jit_type,
		.mmu_type = init_options.mmu_type,
		.stack = init_options.stack_memory,
		.rox = init_options.read_only_exec,
		.collect_stats = init_options.record_instruction_stats,
		.disable_cti = init_options.disable_cti,
		.ticked = init_options.ticked,
		.icache = init_options.instruction_cache,
		.debugging = init_options.debug
	};

	switch (init_options.mmu_type) {
	case mmu::emulated:
		memory_source_ = new sys_memory_source(init_options.total_memory);
		cpu_options.mem_src = memory_source_;
		break;
	case mmu::host:
		host_mmu_ = new platform::host_mmu(init_options.total_memory, init_options.stack_memory);
		cpu_options.mem_ptr = (char*)host_mmu_->get_pointer();
		cpu_options.mem_size = init_options.total_memory;
		break;
	default:
		memory_.resize(init_options.total_memory);
		cpu_options.mem_ptr = memory_.data();
		cpu_options.mem_size = uint32(memory_.size());
		break;
	}

	processor_ = new processor(cpu_options);
	initialize(binary);
	if (init_options.read_only_exec) {
		memory_source_->set_executable_memory(binary);
	}

	if (init_options.debug) {
		debugger_ = new debugger(init_options.debug_port, *this);
		fmt::println("** Waiting for debugger connection on port {}", init_options.debug_port);
		debugger_->wait();
		fmt::println("** Debugger attached.");
	}
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
				exit(1);
			}
		}
		processor_->execute(clocks);
	} while _likely(!options_.ticked || processor_->instruction_count_ < end_target);
}

uint64 system::get_instruction_count() const __restrict {
	return processor_->get_instruction_count();
}

const std::unordered_map<const char *, size_t> & system::get_stats_map() const __restrict {
	return processor_->get_stats_map();
}

std::unordered_map<const char *, size_t> & system::get_stats_map() __restrict {
	return processor_->get_stats_map();
}

size_t system::get_jit_max_instruction_size() const __restrict {
	return processor_->get_jit_max_instruction_size();
}
