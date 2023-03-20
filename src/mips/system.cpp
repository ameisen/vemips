#include "pch.hpp"

#include "system.hpp"
#include "elf/elf.hpp"

#include <cassert>

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

void mips::system::options::validate() const __restrict {
	uint32 required_align = 1;
	if (mmu_type == mmu::host) {
		required_align = 0x1000;
	}
	const uint32 min_memory = align_up(0x10000u, required_align);
	constexpr const uint32 min_stack = 0x1000u;

	if ((total_memory % required_align) != 0) {
		char buffer[512];
		sprintf(buffer, "total memory (%u) is not aligned to required alignment of %u", total_memory, required_align);
		throw std::string(buffer);
	}

	if (total_memory < min_memory) {
		char buffer[512];
		sprintf(buffer, "total memory (%u) is less than the minimum requirement of %u", total_memory, min_memory);
		throw std::string(buffer);
	}

	if (stack_memory && (stack_memory % required_align) != 0) {
		char buffer[512];
		sprintf(buffer, "stack memory (%u) is not aligned to required alignment of %u", stack_memory, required_align);
		throw std::string(buffer);
	}

	if (stack_memory && stack_memory < min_stack) {
		char buffer[512];
		sprintf(buffer, "total stack memory (%u) is less than the minimum requirement of %u", stack_memory, min_stack);
		throw std::string(buffer);
	}

	if (read_only_exec && mmu_type != mmu::emulated) {
		throw std::string("rox requires the emulated MMU to be enabled");
	}
}

void system::initialize(const elf::binary & __restrict binary) {
	char * __restrict mem_data;
	uint32 mem_size;
	if (m_options.mmu_type == mmu::emulated) {
		mem_data = (char *)m_memory_source->get_ptr();
		mem_size = m_memory_source->get_size();
	}
	else {
		mem_data = m_memory.data();
		mem_size = uint32(m_memory.size());
	}

	uint32 stack_offset = m_options.stack_memory;
	if (m_options.mmu_type == mmu::host) {
		stack_offset = 0;
		mem_data = (char *)m_host_mmu->get_pointer();
		mem_size = m_options.total_memory;
	}

	uint32 highest_used_addr = 0;
	// Validate that there is enough memory to hold the binary.
	if (!binary.sections_.empty()) {
		const uint32 preadjusted_final_mapped_address = binary.sections_.back().memory_extent.end_offset();
		const uint32_t finalMappedAddress = preadjusted_final_mapped_address + stack_offset;
		highest_used_addr = std::max(highest_used_addr, preadjusted_final_mapped_address);
		if (finalMappedAddress > mem_size) {
			throw std::runtime_error("ELF Binary's mappings are out of range for System");
		}
		if (m_options.mmu_type == mmu::host) {
			if (finalMappedAddress + m_options.stack_memory > mem_size) {
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
	m_system_break = highest_used_addr;

	// validate that the entry address is at least somewhat sensible
	if ((binary.entry_address_ + m_options.stack_memory) > mem_size) {
		throw std::runtime_error("ELF Entry Address is out of range of the system");
	}

	// Set program counter.
	m_processor->set_program_counter(binary.entry_address_);

	// Set up stack.

	// Set stack pointer
	uint64 stack_start;
	if (m_options.stack_memory == 0) {
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
	m_processor->set_register<uint32>(29, uint32(stack_start));
}


system::system(const options & __restrict init_options, const elf::binary & __restrict binary) : m_options(init_options)
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
		m_memory_source = new sys_memory_source(init_options.total_memory);
		cpu_options.mem_src = m_memory_source;
		break;
	case mmu::host:
		m_host_mmu = new platform::host_mmu(init_options.total_memory, init_options.stack_memory);
		cpu_options.mem_ptr = (char*)m_host_mmu->get_pointer();
		cpu_options.mem_size = init_options.total_memory;
		break;
	default:
		m_memory.resize(init_options.total_memory);
		cpu_options.mem_ptr = m_memory.data();
		cpu_options.mem_size = uint32(m_memory.size());
		break;
	}

	m_processor = new processor(cpu_options);
	initialize(binary);
	if (init_options.read_only_exec) {
		m_memory_source->set_executable_memory(binary);
	}

	if (init_options.debug) {
		m_debugger = new debugger(init_options.debug_port, *this);
		printf("** Waiting for debugger connection on port %u\n", init_options.debug_port);
		m_debugger->wait();
		printf("** Debugger attached.\n");
	}
}

system::~system() {
	delete m_debugger;
	delete m_processor;
	delete m_memory_source;
	delete m_host_mmu;
}

void system::clock(const uint64 clocks) __restrict {
	if _unlikely(!m_processor) [[unlikely]] {
		return;
	}

	while (!m_options.ticked || m_processor->instruction_count_ < m_processor->target_instructions_) {
		if _unlikely(m_debugger && m_debugger->should_pause()) [[unlikely]] {
			m_debugger->wait();
			if (m_debugger->should_kill()) {
				exit(1);
			}
		}
		m_processor->execute(clocks);
	}
}

uint64 system::get_instruction_count() const __restrict {
	return m_processor->get_instruction_count();
}

const std::unordered_map<const char *, size_t> & system::get_stats_map() const __restrict {
	return m_processor->get_stats_map();
}

size_t system::get_jit_max_instruction_size() const __restrict {
	return m_processor->get_jit_max_instruction_size();
}
