#include "pch.hpp"

#include "processor.hpp"
#include "mips/coprocessor/coprocessor1/coprocessor1.hpp"
#include "mips/instructions/instructions.hpp"
#include "mips/processor/jit/jit1/jit1.hpp"
#include "mips/system.hpp"
#include <cstdio>

#define NEW_SYSCALL 1

using namespace mips;

thread_local processor * __restrict g_currentprocessor = nullptr;
thread_local coprocessor * __restrict g_currentcoprocessor = nullptr;

__forceinline processor * __restrict get_current_processor() {
	return g_currentprocessor;
}

__forceinline coprocessor * __restrict get_current_coprocessor() {
	return g_currentcoprocessor;
}

__forceinline void set_current_coprocessor(coprocessor * __restrict cop) {
	g_currentcoprocessor = cop;
}

#if !USE_STATIC_INSTRUCTION_SEARCH
namespace mips::instructions {
	extern void finish_map_build();
}
#endif

processor::processor(const options & __restrict _options) :
	m_jit_type(_options.jit_type),
	m_memory_source(_options.mem_src),
	m_readonly_exec(_options.rox),
	m_collect_stats(_options.collect_stats),
	 m_disable_cti(_options.disable_cti),
	m_ticked(_options.ticked),
	m_mmu_type(_options.mmu_type),
	m_stack_size(_options.stack),
	m_guest_system(_options.guest_system),
	m_debugging(_options.debugging)
{
#if !USE_STATIC_INSTRUCTION_SEARCH
	mips::instructions::finish_map_build();
#endif
	// Set up coprocessor 1 (FPU)
	m_coprocessors[1] = new coprocessor1(*this);
	if (m_memory_source) {
		m_memory_source->register_processor(this);
	}

#if !TABLEGEN
	switch (_options.jit_type) {
		case JitType::None:
			m_jit1 = nullptr; break;
		case JitType::Jit:
			m_jit1 = new jit1(*this); break;
	}
#endif

	if (m_mmu_type != mmu::emulated) {
		// if there is no MMU, we need to set up some basic pointers.
		m_memory_ptr = uint64(_options.mem_ptr);
		m_memory_size = _options.mem_size;
	}

	static const volatile bool RunNever = false;
	if (RunNever) {
		get_current_processor();
		get_current_coprocessor();
		set_current_coprocessor(nullptr);
	}
}

processor::~processor() {
	for (coprocessor * __restrict cop : m_coprocessors) {
		delete cop;
	}
	if (m_memory_source) {
		m_memory_source->unregister_processor(this);
	}

#if !TABLEGEN
	switch (m_jit_type) {
		case JitType::Jit:
			delete m_jit1; break;
	}
#endif
}

void processor::delay_branch(uint32 pointer) __restrict {
	m_branch_target = pointer;
	set_flags(flag_bit(flag::branch_delay) | flag_bit(flag::no_cti));
}

void processor::compact_branch(uint32 pointer) __restrict {
	set_program_counter(pointer);
	set_flags(flag_bit(flag::pc_changed));
}

void processor::set_no_cti() __restrict {
	set_flags(flag_bit(flag::no_cti));
}

void processor::set_link(uint32 pointer) __restrict {
	m_registers[31] = pointer;
}

instruction_t processor::get_instruction() const __restrict {
	if ((m_program_counter % 4) != 0) {
		throw CPU_Exception{ CPU_Exception::Type::RI, m_program_counter };
	}
	const instruction_t instruction_ptr = mem_fetch<const instruction_t>(m_program_counter);

	return instruction_ptr;
}

template <typename T>
struct updater {
	T * __restrict variable_ptr = nullptr;
	T new_value = 0;
	~updater() {
		if (variable_ptr) {
			(*variable_ptr) = new_value;
		}
	}
};

template <typename T, uint32 count>
struct incrementer {
	T * __restrict variable_ptr = nullptr;
	~incrementer() {
		if (variable_ptr) {
			(*variable_ptr) += count;
		}
	}
};

// Sets the program counter. This is meant to be used as an external function to set up the CPU
void processor::set_program_counter(uint32 address) __restrict {
	m_program_counter = address;
}

// Gets the program counter
uint32 processor::get_program_counter() __restrict {
	return m_program_counter;
}

#if !TABLEGEN
extern const mips::instructions::InstructionInfo * __restrict get_instruction(instruction_t i);
extern bool execute_instruction(instruction_t i, mips::processor & __restrict p);
#endif

// REMOVE THIS WHEN UNIFIED INSTRUCTIONS ARE IN
void processor::ExecuteInstruction(bool branch_delay) __restrict {
#if !TABLEGEN
	try {
		g_currentprocessor = this;

		switch (m_jit_type) {
			// if this is a branch delay slot, force interpretive mode. Fall through to the interpreter.
			case JitType::Jit:
			if (!branch_delay) {
				m_jit1->execute_instruction(m_program_counter);
				break;
			}
			default: {
				instruction_t instruction = get_instruction();
				if (m_collect_stats) {
					const auto * __restrict info = ::get_instruction(instruction);
					if (info) {
						++m_instruction_stats[info->Name];
						info->Proc(instruction, *this);
					}
					else {
						throw m_trapped_exception;
					}
				}
				else {
					if (!execute_instruction(instruction, *this)) {
						throw CPU_Exception{ CPU_Exception::Type::RI, this->get_program_counter() };
					}
				}
				if (m_jit_type == JitType::Jit) {
					if (get_flags(flag_bit(flag::trapped_exception))) {
						throw m_trapped_exception;
					}
				}
			} break;
		}
	}
	catch (const CPU_Exception & __restrict ex) {
		m_from_exception = true;
		m_guest_system->handle_exception(ex);
	}
#endif
}

struct cti_clear {
	processor * __restrict m_Processor = nullptr;

	~cti_clear() {
		if (m_Processor) {
			m_Processor->clear_flags(processor::flag_bit(processor::flag::no_cti));
		}
	}
};

void processor::execute(uint64 clocks) __restrict {
	m_target_instructions = m_instruction_count + clocks;

	while (!m_ticked || m_instruction_count < m_target_instructions) {
		m_registers[0] = 0; // $0 is _always_ 0
		for (coprocessor * __restrict cop : m_coprocessors) {
			if (cop) {
				cop->clock();
			}
		}

		incrementer<uint32, 4>	program_counter_incrementer;
		updater<uint32>			 program_counter_updater;
		cti_clear					 cti_delay_updater;

		const uint32 start_program_counter = m_program_counter;

		if (m_jit_type == JitType::None) {
			const bool branch_delay = get_flags(flag_bit(flag::branch_delay));

			program_counter_incrementer.variable_ptr = branch_delay ? nullptr : &m_program_counter;

			program_counter_updater.variable_ptr = branch_delay ? &m_program_counter : nullptr;
			program_counter_updater.new_value = (!branch_delay) ? (m_program_counter + 4) : m_branch_target;
			if (get_flags(flag_bit(flag::no_cti))) {
				cti_delay_updater.m_Processor = this;
			}

			++m_instruction_count;
			clear_flags(flag_bit(flag::branch_delay));
		}
		else {
			if (m_from_exception) {
				if (get_flags(flag_bit(flag::branch_delay))) {
					m_program_counter = m_branch_target;
					clear_flags(flag_bit(flag::branch_delay));
				}
				else {
					m_program_counter += 4;
				}
				m_from_exception = false;
			}
		}

		ExecuteInstruction(false);

		// If the program counter has changed through a compact branch, then do NOT increment the program counter.
		if (get_flags(flag_bit(flag::pc_changed))) {
			clear_flags(flag_bit(flag::pc_changed));
			program_counter_updater.variable_ptr = nullptr;
			program_counter_incrementer.variable_ptr = nullptr;
		}

		// Were we interrupted by the debugger? If so, drop to caller.
		if (m_debugging && m_guest_system->get_debugger()->should_pause()) {
			return;
		}
	}
}

void processor::memory_touched(uint32 pointer, uint32 size) __restrict {
	if (m_readonly_exec) {
		return;
	}
#if !TABLEGEN
	switch (m_jit_type) {
		case JitType::Jit: {
			m_jit1->memory_touched(pointer);
		} break;
	}
#endif
}

void processor::memory_touched_jit(uint32 pointer, uint32 size) __restrict {
}

size_t processor::get_jit_max_instruction_size() const __restrict {
	switch (m_jit_type) {
		case JitType::Jit:
			return m_jit1->get_max_instruction_size();
	}
	return 0;
}
