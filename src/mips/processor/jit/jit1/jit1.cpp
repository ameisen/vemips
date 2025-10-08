#include "pch.hpp"
#include "jit1.hpp"

#include <cassert>

#include <algorithm>
#include <mutex>

#include "codegen.hpp"
#include "system.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "instructions/instructions_table.hpp"
#include "platform/platform_headers.hpp"
#include "processor/processor.hpp"

using namespace mips;

#define JIT_INSTRUCTION_SEPARATE 0
#define JIT_INSERT_IDENTIFIERS 0

// this can and should be global to all JITs.
namespace {
	static constinit size_t global_exec_data_size = 0x1000;
	static std::mutex g_global_exec_data_lock;
	static std::shared_ptr<char[]> g_global_exec_data;
}

extern "C" uintptr jit1_springboard(uintptr instruction, uintptr processor, uintptr pc_runner, uintptr, uintptr, uintptr);
extern "C" void jit1_drop_signal();

// todo: make either non-static or into a proper allocator 
namespace {
	_pragma_small_code
	static _nothrow size_t get_allocation_granularity() noexcept
	{
		SYSTEM_INFO info = {};
		GetSystemInfo(&info);
		return info.dwAllocationGranularity;
	}
	_pragma_default_code

	static std::mutex free_chunks_lock;
	static std::vector<void*> free_chunks;
	static const size_t allocation_granularity = get_allocation_granularity();

	_forceinline _result_noalias _nothrow static void* allocate_executable(const size_t size) noexcept {
		void* result = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		xassert(result != nullptr);

		return result;
	}

	template <size_t ChunkSize>
	_forceinline _result_noalias _nothrow static void* allocate_executable_chunk() noexcept {
		{
			std::unique_lock lock{free_chunks_lock};
			if (!free_chunks.empty())
			{
				void* const result = free_chunks.back();
				free_chunks.pop_back();
				return result;
			}
		}

		const size_t alloc_size = std::max(
			allocation_granularity,
			ChunkSize
		);

		const auto new_ptr = static_cast<char* const __restrict>(
			allocate_executable(alloc_size)
		);

		{
			const size_t num_chunks = alloc_size / ChunkSize;
			char* __restrict ptr = new_ptr;

			std::unique_lock lock{free_chunks_lock};
			for (size_t i = 1; i < num_chunks; ++i)
			{
				ptr += ChunkSize;
				free_chunks.push_back(ptr);
			}
		}

		return new_ptr;
	}

	_forceinline _nothrow static void free_executable(void * const ptr) noexcept {
		const BOOL result = VirtualFree(ptr, 0, MEM_RELEASE);
		xassert(result != 0);
	}

	_forceinline _nothrow static void free_executable_chunk(void * const ptr) noexcept {
		std::unique_lock lock{free_chunks_lock};
		free_chunks.push_back(ptr);
	}
}

void jit1::initialize_chunk(chunk_data& __restrict chunk) {
	if (!chunk.chunk) {
		chunk.chunk.reset(new Chunk);

		make_unique_inline(chunk.chunk->m_patches);
		chunk.offset = m_chunkoffset_allocator.allocate();
		chunk.offset->fill(0);
	}
}

#if !USE_LEVELED_MAP
_forceinline jit1::MapLevel1::~MapLevel1() {
	for (auto&& data : m_Data) {
		data.release();
	}
}

_forceinline jit1::chunk_data& jit1::MapLevel1::operator [] (uint32 idx) {
	auto&& __restrict data = m_Data[idx];
	if (!data.contained()) {
		data.initialize();
	}
	return data;
}
#endif

namespace {
	_cold _nothrow void RI_Exception (instruction_t, processor & __restrict processor) noexcept
	{
		processor.set_trapped_exception({ CPU_Exception::Type::RI, processor.get_program_counter() });
	}

	_cold _nothrow void OV_Exception (processor & __restrict processor) noexcept
	{
		processor.set_trapped_exception({ CPU_Exception::Type::Ov, processor.get_program_counter() });
	}

	_cold _nothrow void TR_Exception (uint32 code, processor & __restrict processor) noexcept
	{
		processor.set_trapped_exception({ CPU_Exception::Type::Tr, processor.get_program_counter(), code });
	}

	_cold _nothrow void CpU_Exception (uint32 code, processor & __restrict processor) noexcept
	{
		processor.set_trapped_exception({ CPU_Exception::Type::CpU, processor.get_program_counter(), code });
	}

	_cold _nothrow void AdEL_Exception (uint32 address, processor & __restrict processor) noexcept
	{
		processor.set_trapped_exception({ CPU_Exception::Type::AdEL, processor.get_program_counter(), address });
	}

	_cold _nothrow void AdES_Exception(uint32 address, processor & __restrict processor) noexcept
	{
		processor.set_trapped_exception({ CPU_Exception::Type::AdES, processor.get_program_counter(), address });
	}
	_pragma_default_code
}

jit1::jit1(processor & __restrict _processor) : processor_(_processor)
{
	// todo : not thread safe
	std::unique_lock lock{g_global_exec_data_lock};

	if (!g_global_exec_data)
	{
		char* const exec_data = static_cast<char *>(allocate_executable(global_exec_data_size));

		g_global_exec_data = global_exec_data = {
			exec_data,
			&free_executable
		};

		const Xbyak::Label call_instruction_hazard;

		Jit1_CodeGen cg{ *this, reinterpret_cast<uint8 *>(exec_data), global_exec_data_size };
		{
			static constexpr const auto offsets = processor::recompiler_offsets<>::get<int8>();

			cg.mov(cg.qword[cg.rbp + offsets.ic], cg.rdi);		// save instruction count
			cg.mov(cg.dword[cg.rbp + offsets.flags], cg.ebx);
			cg.mov(cg.dword[cg.rbp + offsets.dbt], cg.esi);  // set it in the interpreter
			cg.mov(cg.dword[cg.rbp + instructions::GPRegister<>{Jit1_CodeGen::mips_fp}.get_offset()], cg.r15d);
			cg.set(cg.rax, reinterpret_cast<intptr>(&jit1_drop_signal));
			cg.jmp_ex<true>(cg.rax);
			{
				const Xbyak::Label no_redirect;

				// ecx = current address
				// rax = call address
				// rdx = return address - not a true call, this is intended to be jumped to
				// r9  = argument
				cg.L(call_instruction_hazard);
				cg.push(cg.rdx);
				cg.mov(cg.qword[cg.rbp + offsets.ic], cg.rdi);
				cg.mov(cg.dword[cg.rbp + offsets.pc], cg.ecx);
				cg.mov(cg.dword[cg.rbp + offsets.flags], cg.ebx);
				cg.lea(cg.rdx, cg.qword[cg.rbp - 128]);
				cg.mov(cg.rcx, cg.r9);
				cg.call_ex<true>(cg.rax);
				cg.test(cg.rax, cg.rax);
				cg.jz(no_redirect);
				{
					// If we reach here, we altered the current chunk, meaning we need to jump to a different offset.
					cg.inc(cg.rdi);
					cg.push(cg.rax.changeBit(sizeof(void*) * 8));
					cg.jmp(cg.rax);
				}
				cg.L(no_redirect);
				cg.pop(cg.rax);
				cg.jmp(cg.rax);
			}
		}

		cg.ready();

		call_instruction_hazard_ptr_ = call_instruction_hazard.getAddress();

		_clear_cache(exec_data, exec_data + cg.getSize());
	}
}

jit1::~jit1() = default;


namespace
{
	void increment_instruction_statistic(const char * __restrict instruction, processor & __restrict processor)
	{
		processor.increment_instruction_statistic(instruction);
	}

	void increment_jit_emulated_instruction_statistic(const char * __restrict instruction, processor & __restrict processor)
	{
		processor.increment_jit_emulated_instruction_statistic(instruction);
	}

	static uint32 should_debug_break(const processor * __restrict proc)
	{
		return proc->get_guest_system()->get_debugger()->should_interrupt_execution();
	}

	struct cache_flusher final {
		Jit1_CodeGen& codegen;
		uint8* const start_address;

		cache_flusher(Jit1_CodeGen & codegen) : codegen(codegen), start_address(codegen.get_current_address()) {
		}

		~cache_flusher() {
			_clear_cache(start_address, codegen.get_current_address());
		}
	};
}

void Jit1_CodeGen::write_chunk(jit1::ChunkOffset & __restrict chunk_offset, jit1::Chunk & __restrict chunk, uint32 start_address, bool update) {
	xassert((start_address % 4) == 0);

	const auto& this_processor = jit_.processor_;

	cache_flusher scoped_cache_flusher(*this);

	const uint32 base_address = start_address & ~(jit1::ChunkSize - 1);
	const uint32 last_address = base_address + (jit1::ChunkSize - 1);

	constexpr uint32 chunk_start_offset = 0;

	uint32 start_index = 0;
	if (update)
	{
#if 0
		if (chunk.m_has_fixups)
		{
			chunk.m_has_fixups = false;
		}
#endif
		// Can't make this work with xbyak
#if 0
		else
		{
			start_index = start_address - chunk.m_offset;
			start_index /= 4u;
			// also shift the 'data' pointer to this offset.
			chunk_start_offset = chunk_offset[start_index];
		}
#endif

		if (auto* patches = chunk.m_patches.get())
		{
			patches->clear();
		}
	}
	else
	{
		chunk.m_offset = start_address;
		chunk.m_chunk_offset = &chunk_offset;
	}

	if (chunk_start_offset == 0)
	{
		L(intrinsics_.chunk_start);
		L(intrinsics_.code_start);
	}

	const Xbyak::Label intrinsic_ex;
	const Xbyak::Label intrinsic_ex_no_pc;

	const instructions::InstructionInfo * __restrict prev_instruction_info_ptr = nullptr;
	const instructions::InstructionInfo * __restrict instruction_info_ptr = nullptr;
	instruction_t prev_instruction = 0;
	instruction_t instruction = 0;

	static constexpr uint32 num_instructions = jit1::NumInstructionsChunk;
	for (
		uint32 i = start_index;
		i < num_instructions;
		++i, prev_instruction_info_ptr = instruction_info_ptr, prev_instruction = instruction
	)
	{
		const auto& cur_label = get_instruction_offset_label(i);
		L(cur_label);
		chunk_offset[i] = uint32(this->getSize()) + chunk_start_offset;

		bool cti = false;
		bool alters_memory = false;
		bool cti_test = false;
		bool compact_branch = false;
		bool delay_branch = false;
		bool possible_after_delaybranch = false;
		bool possible_after_delaybranch_hazard = false;
		bool store_handled = false;

		const uint32 current_address = start_address;

		if (i == start_index)
		{
#if 0
			// For the purpose of handling self-modifying code, we always emit delay branch logic for the first instruction of a chunk
			// so that we do not potentially need to change it after the fact, causing a cascade of sorts.
			// TODO : this really sucks since it will always make the first instruction an indeterminate branch,
			// which is the slowest by a very large margin. It's probably faster to just rebuild adjacent chunks as well...
			possible_after_delaybranch = true;
			possible_after_delaybranch_hazard = true;
#endif

			if (const instruction_t * __restrict prev_ptr = this_processor.safe_mem_fetch_exec<const instruction_t>(current_address - 4))
			{
				prev_instruction = *prev_ptr;
				prev_instruction_info_ptr = mips::FindExecuteInstruction(prev_instruction);
			}
		}

		except_result exception_result = except_result::none;

		std::function<insert_function_type> insert_function;
		insert_location insert_function_location = {};

#if JIT_INSERT_IDENTIFIERS
		const Xbyak::Label id_label;
		jmp(id_label);
		nop(4, false);
		set(eax, current_address);
		nop(4, false);
		L(id_label);
#endif
		
		if (this_processor.ticked_ && this_processor.debugging_)
		{
			mov(dword[rbp + offsets.pc], current_address);
			lea(rcx, qword[rbp - 128]);
			std::ignore = call_ex<true>(ptr_cast(&should_debug_break), rax);
			test(eax, eax);
			jnz(intrinsics_.save_return, T_NEAR);
			cmp(rdi, r14);
			je(intrinsics_.save_return, T_NEAR);
		}
		else if (this_processor.debugging_)
		{
			mov(dword[rbp + offsets.pc], current_address);
			lea(rcx, qword[rbp - 128]);
			std::ignore = call_ex<true>(ptr_cast(&should_debug_break), rax);
			test(eax, eax);
			jnz(intrinsics_.save_return, T_NEAR);
		}
		else if (this_processor.ticked_)
		{
			// check if we are already at our tick count.
			mov(eax, current_address);
			cmp(rdi, r14);
			je(intrinsics_.save_return_eax_pc, T_NEAR);
		}

		if (current_address >= 4)
		{
			const size_t ins_start_size = getSize();

			if (prev_instruction_info_ptr != nullptr)
			{
				if (prev_instruction_info_ptr && instructions::HasAnyFlags(prev_instruction_info_ptr->OpFlags, instructions::OpFlags::SetNoCTI))
				{
					cti_test = true;
				}
				if (prev_instruction_info_ptr && instructions::HasAnyFlags(prev_instruction_info_ptr->OpFlags, instructions::OpFlags::DelayBranch))
				{
					possible_after_delaybranch = true;
					possible_after_delaybranch_hazard = instructions::HasAnyFlags(prev_instruction_info_ptr->OpFlags, instructions::OpFlags::Hazard);
				}
			}

			const instruction_t * __restrict ptr = this_processor.safe_mem_fetch_exec<const instruction_t>(current_address);
			if (ptr)
			{
				instruction = *ptr;
				instruction_info_ptr = mips::FindExecuteInstruction(instruction);

				if (instruction_info_ptr)
				{
					if (this_processor.collect_stats_)
					{
						// dispatch a stat call.
						set(rcx, intptr(instruction_info_ptr->Name));
						call(intrinsics_.stats);
					}

					compact_branch = instructions::HasAnyFlags(instruction_info_ptr->OpFlags, instructions::OpFlags::CompactBranch);
					delay_branch = instructions::HasAnyFlags(instruction_info_ptr->OpFlags, instructions::OpFlags::DelayBranch);

					if (cti_test && !this_processor.disable_cti_)
					{
						// CTI instruction prefix.
						if (instructions::HasAnyFlags(instruction_info_ptr->OpFlags, instructions::OpFlags::ControlInstruction))
						{
							const Xbyak::Label no_ex;

							test(ebx, processor::flag::no_cti);
							jz(no_ex);
							set(ecx, current_address);
							jmp(intrinsics_.ri, T_NEAR);
							L(no_ex);
						}
						else
						{
							// This cannot be a CTI. Clear the CTI bit.
							// C6 42 7F 00 
							// mov byte [rdx + 0x7F], 0

							and_(ebx, ~processor::flag::no_cti);
						}
					}

					// Was the instruction a memory-altering instruction?
					// TODO : temporary for debugging
					if (false)
					{
						exception_result = insert_procedure_check_hazard(current_address, *instruction_info_ptr, instruction);
					}
					else if (instructions::HasAnyFlags(instruction_info_ptr->OpFlags, instructions::OpFlags::RequiresCoP0) && !jit_.processor_.has_coprocessor0_support())
					{
						set(ecx, current_address);
						jmp(intrinsics_.cpu, T_NEAR);
						exception_result = except_result::always_throw;
					}
					else if (instructions::HasAnyFlags(instruction_info_ptr->OpFlags, instructions::OpFlags::Store))
					{
						if (
							const auto store_result = write_STORE(chunk_offset, current_address, instruction, *instruction_info_ptr);
							!store_result
						)
						{
							store_handled = false;

							if (this_processor.collect_stats_)
							{
								// dispatch a stat call.
								set(rcx, intptr(instruction_info_ptr->Name));
								call(intrinsics_.emulated_stats);
							}

							exception_result = insert_procedure_check_hazard(current_address, *instruction_info_ptr, instruction);
						}
						else
						{
							store_handled = true;
							exception_result = store_result.value();
						}
						alters_memory = exception_result != except_result::always_throw && exception_result != except_result::always_except;
					}
					else if (
						((instruction_info_ptr->OpFlags & (instructions::OpFlags::Load | instructions::OpFlags::COP1)) == instructions::OpFlags::Load)
					)
					{
						if (
							const auto load_result = write_LOAD(chunk_offset, current_address, instruction, *instruction_info_ptr);
							!load_result
						)
						{
							if (this_processor.collect_stats_)
							{
								// dispatch a stat call.
								set(rcx, intptr(instruction_info_ptr->Name));
								call(intrinsics_.emulated_stats);
							}

							exception_result = insert_procedure_check_hazard(current_address, *instruction_info_ptr, instruction);
						}
						else
						{
							exception_result = load_result.value();
						}
					}
					else if (compact_branch)
					{
						std::tie(
							insert_function,
							insert_function_location,
							exception_result
						) = write_compact_branch(chunk, chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (delay_branch)
					{
						exception_result = write_delay_branch(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_NOP))
					{
						// nop
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SUBU))
					{
						write_PROC_SUBU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SUB))
					{
						exception_result = write_PROC_SUB(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_OR))
					{
						write_PROC_OR(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_NOR))
					{
						write_PROC_NOR(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_AND))
					{
						write_PROC_AND(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_ORI))
					{
						write_PROC_ORI(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_ANDI))
					{
						write_PROC_ANDI(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_MOVE))
					{
						write_PROC_MOVE(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_ADDIU))
					{
						write_PROC_ADDIU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					//else if (IS_INSTRUCTION(instruction_info_ptr, PROC_ADDI))
					//{
					//	exception_result = write_PROC_ADDI(chunk_offset, current_address, instruction, *instruction_info_ptr);
					//}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_ADDU))
					{
						write_PROC_ADDU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_ADD))
					{
						exception_result = write_PROC_ADD(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_AUI))
					{
						write_PROC_AUI(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SELEQZ))
					{
						write_PROC_SELEQZ(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SELNEZ))
					{
						write_PROC_SELNEZ(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SLT))
					{
						write_PROC_SLT(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SLTU))
					{
						write_PROC_SLTU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SLTI))
					{
						write_PROC_SLTI(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SLTIU))
					{
						write_PROC_SLTIU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, COP1_MFC1_v))
					{
						write_COP1_MFC1(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, COP1_MTC1_v))
					{
						write_COP1_MTC1(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, COP1_MFHC1_v))
					{
						write_COP1_MFHC1(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, COP1_MTHC1_v))
					{
						write_COP1_MTHC1(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, COP1_SEL_f) || IS_INSTRUCTION(instruction_info_ptr, COP1_SEL_d))
					{
						write_COP1_SEL(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_MUL))
					{
						write_PROC_MUL(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_MUH))
					{
						write_PROC_MUH(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_MULU))
					{
						write_PROC_MULU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_MUHU))
					{
						write_PROC_MUHU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_DIV))
					{
						write_PROC_DIV(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_MOD))
					{
						write_PROC_MOD(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_DIVU))
					{
						write_PROC_DIVU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_MODU))
					{
						write_PROC_MODU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_XOR))
					{
						write_PROC_XOR(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_XORI))
					{
						write_PROC_XORI(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SEB))
					{
						write_PROC_SEB(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SEH))
					{
						write_PROC_SEH(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_EHB))
					{
						write_PROC_EHB(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SLL))
					{
						write_PROC_SLL(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SRL))
					{
						write_PROC_SRL(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SRA))
					{
						write_PROC_SRA(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_RDHWR))
					{
						exception_result = write_PROC_RDHWR(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SLLV))
					{
						write_PROC_SLLV(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SRLV))
					{
						write_PROC_SRLV(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SYNC))
					{
						write_PROC_SYNC(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SYNC))
					{
						exception_result = write_PROC_SYNCI(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (
						const auto&& write_trap_func = [&] -> std::optional<decltype(&Jit1_CodeGen::write_PROC_TNE)>
						{
							if (IS_INSTRUCTION(instruction_info_ptr, PROC_TEQ))
							{
								return std::make_optional(&Jit1_CodeGen::write_PROC_TEQ);
							}
							else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TGE))
							{
								return std::make_optional(&Jit1_CodeGen::write_PROC_TGE);
							}
							else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TGEU))
							{
								return std::make_optional(&Jit1_CodeGen::write_PROC_TGEU);
							}
							else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TLT))
							{
								return std::make_optional(&Jit1_CodeGen::write_PROC_TLT);
							}
							else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TLTU))
							{
								return std::make_optional(&Jit1_CodeGen::write_PROC_TLTU);
							}
							else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TNE))
							{
								return std::make_optional(&Jit1_CodeGen::write_PROC_TNE);
							}
							else
							{
								return std::nullopt;
							}
						}();
						write_trap_func.has_value()
					)
					{
						exception_result = (this->*write_trap_func.value())(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SYSCALL))
					{
						exception_result = write_PROC_SYSCALL(chunk_offset, current_address, instruction, *instruction_info_ptr, intrinsic_ex_no_pc);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_BREAK))
					{
						exception_result = write_PROC_BREAK(chunk_offset, current_address, instruction, *instruction_info_ptr, intrinsic_ex_no_pc);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SIGRIE))
					{
						exception_result = write_PROC_SIGRIE(chunk_offset, current_address, instruction, *instruction_info_ptr, intrinsic_ex_no_pc);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_EXT))
					{
						write_PROC_EXT(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_INS))
					{
						write_PROC_INS(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_LSA))
					{
						write_PROC_LSA(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_CLZ))
					{
						write_PROC_CLZ(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_CLO))
					{
						write_PROC_CLO(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else
					{
						if (this_processor.collect_stats_)
						{
							// dispatch a stat call.
							set(rcx, intptr(instruction_info_ptr->Name));
							call(intrinsics_.emulated_stats);
						}

						exception_result = insert_procedure_check_hazard(current_address, *instruction_info_ptr, instruction);
					}

					cti = instruction_info_ptr->Flags.control;
				}
				else
				{
					// RI
					set(ecx, current_address);
					jmp(intrinsics_.ri, T_NEAR);
					exception_result = except_result::always_throw;
				}
			}
			else
			{
				// AdEL
				set(ecx, current_address);
				jmp(intrinsics_.adel_identity, T_NEAR);
				exception_result = except_result::always_throw;
				instruction = 0;
				instruction_info_ptr = nullptr;
			}

			const size_t ins_size = getSize() - ins_start_size;
			jit_.largest_instruction_ = std::max(jit_.largest_instruction_, ins_size);
		}
		else
		{
			// AdEL
			set(ecx, current_address);
			jmp(intrinsics_.adel_identity, T_NEAR);
			exception_result = except_result::always_throw;
			instruction = 0;
			instruction_info_ptr = nullptr;
		}
		// Epilog for store ops, as we need to test afterwards to see if they may have altered JIT memory.
		//if (!terminate_instruction)
		{
			// increment pc
			// 49 FF C0
			// inc r8
			inc(rdi);
		}

		bool can_throw = (exception_result & (except_result::can_except | except_result::always_except)) != except_result::none;
		bool terminate_instruction = (exception_result & (except_result::always_except | except_result::always_throw | except_result::always_exits)) != except_result::none;

		const auto exception_check_epilog = [&]
		{
			// exception checking epilog
			if (can_throw)
			{
				// Presently only used for subroutine calls, which save pc.

				// 8A 42 CC 84 C0 74 01 C3 
				// mov al, byte [rdx + 0xCC]
				// test al, al ; CC = ex_offset
				// je no_ex
				// inc qword [rdx + 0x7F] ; ic_offset
				// ret
				// no_ex:
				test(ebx, processor::flag::trapped_exception);
				jnz(intrinsics_.check_ex, T_NEAR);
			}
		};

		if (insert_function_location == insert_location::before_epilog)
		{
			insert_function(chunk, chunk_offset, current_address);
		}

		if (!terminate_instruction)
		{
			if (insert_function_location == insert_location::before_exception_check)
			{
				insert_function(chunk, chunk_offset, current_address);
			}

			exception_check_epilog();

			if (insert_function_location == insert_location::before_delaybranch_check)
			{
				insert_function(chunk, chunk_offset, current_address);
			}

			if (possible_after_delaybranch)
			{
				handle_delay_branch(chunk, chunk_offset, current_address - 4, prev_instruction, *prev_instruction_info_ptr, possible_after_delaybranch_hazard);
			}
		}
		else
		{
			if (insert_function_location == insert_location::before_exception_check)
			{
				insert_function(chunk, chunk_offset, current_address);
			}

			exception_check_epilog();

			if (insert_function_location == insert_location::before_delaybranch_check)
			{
				insert_function(chunk, chunk_offset, current_address);
			}
		}

		if (insert_function_location == insert_location::after_epilog)
		{
			insert_function(chunk, chunk_offset, current_address);
		}

#if JIT_INSTRUCTION_SEPARATE
		nop(8, false);
#endif

		start_address += 4;
	}

	chunk.ends_with_delay_branch = prev_instruction_info_ptr && HasAnyFlags(prev_instruction_info_ptr->OpFlags, instructions::OpFlags::DelayBranch);

	intrinsic_write_patch_jump(
		chunk,
		last_address + 1,
		edx,
		true
	);

	{
		const Xbyak::Label save;

		L(intrinsics_.intrinsic_start);

		if (intrinsics_.ri.used) {
			L(intrinsics_.ri);
			set(rax, uintptr(RI_Exception));
			jmp(intrinsic_ex, T_SHORT);
		}

		if (intrinsics_.adel_identity.used) {
			L(intrinsics_.adel_identity);
			set(rax, uintptr(AdEL_Exception));
			jmp(intrinsic_ex, T_SHORT);
		}

		if (intrinsics_.adel.used) {
			L(intrinsics_.adel);
			mov(dword[rbp + offsets.pc], eax);
			set(rax, uintptr(AdEL_Exception));
			jmp(intrinsic_ex_no_pc, T_SHORT);
		}

		if (intrinsics_.ades.used) {
			L(intrinsics_.ades);
			mov(dword[rbp + offsets.pc], eax);
			set(rax, uintptr(AdES_Exception));
			jmp(intrinsic_ex, T_SHORT);
		}

		if (intrinsics_.ov.used) {
			L(intrinsics_.ov);
			set(rax, uintptr(OV_Exception));
			jmp(intrinsic_ex, T_SHORT);
		}

		if (intrinsics_.tr.used) {
			L(intrinsics_.tr);
			set(rax, uintptr(TR_Exception));
			mov(dword[rbp + offsets.pc], ecx);
			mov(ecx, edx);
			jmp(intrinsic_ex_no_pc, T_SHORT);
		}

		if (intrinsics_.cpu.used) {
			L(intrinsics_.cpu);
			set(rax, uintptr(CpU_Exception));
			jmp(intrinsic_ex, T_SHORT);
		}

		if (isReferenced(intrinsic_ex) || isReferenced(intrinsic_ex_no_pc)) {
			if (isReferenced(intrinsic_ex)) {
				L(intrinsic_ex);
				mov(dword[rbp + offsets.pc], ecx);
			}
			L(intrinsic_ex_no_pc);
			call(save);
			lea(rdx, qword[rbp - 128]);
			jmp_ex<true>(rax);
		}

		if (intrinsics_.check_ex.used) {
			L(intrinsics_.check_ex);
			jmp(intrinsics_.save_return, T_SHORT);
		}

		if (intrinsics_.save_return_eax_pc.used) {
			L(intrinsics_.save_return_eax_pc);
			mov(dword[rbp + offsets.pc], eax);
			jmp(intrinsics_.save_return, T_SHORT);
		}

		L(save);
		mov(qword[rbp + offsets.ic], rdi);		// save instruction count
		mov(dword[rbp + offsets.flags], ebx);
		mov(dword[rbp + offsets.dbt], esi);  // set it in the interpreter
		mov(dword[rbp + instructions::GPRegister<>{Jit1_CodeGen::mips_fp}.get_offset()], r15d);
		ret();

		if (intrinsics_.save_return.used) {
			L(intrinsics_.save_return);
			call(save);
			escape_ret();
		}

		if (this_processor.collect_stats_) {
			// dispatch a stat call.
			if (intrinsics_.stats.used) {
				L(intrinsics_.stats);
				lea(rdx, qword[rbp - 128]);
				std::ignore = call_ex<true, 8>(ptr_cast(&increment_instruction_statistic), rax);
				ret();
			}

			if (intrinsics_.emulated_stats.used) {
				L(intrinsics_.emulated_stats);
				lea(rdx, qword[rbp - 128]);
				std::ignore = call_ex<true, 8>(ptr_cast(&increment_jit_emulated_instruction_statistic), rax);
				ret();
			}
		}
	}

	xassert(!hasUndefinedLabel());
	const bool has_patches = [&chunk]
	{
		if (auto* patches = chunk.m_patches.get())
		{
			return !patches->empty();
		}

		return false;
	}();
	ready(has_patches ? PROTECT_RWE : PROTECT_RE);
	//chunk.m_datasize = getSize();
	const uint8 * __restrict data = getCode();

	//this->

	xassert(chunk_offset.back() < getSize());
	xassert(getSize() <= jit1::MaxChunkRealSize);

	// Finalize the data stream and then allocate a new chunk for it, if necessary.
	uint32 total_size = uint32(this->getSize()) + chunk_start_offset;
	// The current chunk is large enough for our data.
	//memcpy((char *)chunk.m_data + chunk_start_offset, data, getSize());

	if (!update) {
		jit_.chunks_.push_back(&chunk);
		std::ranges::sort(jit_.chunks_);
	}

	static_assert(xbyak_throws, "must implement error checks");
}

void jit1::populate_chunk(ChunkOffset & __restrict chunk_offset, Chunk & __restrict chunk, const uint32 start_address, const bool update)
{
	if (!chunk.m_data)
	{
		chunk.m_datasize = MaxChunkRealSize;
		chunk.m_data = static_cast<uint8 *>(allocate_executable_chunk<MaxChunkRealSize>());
	}
	Jit1_CodeGen cg{ *this, chunk.m_data, chunk.m_datasize };
	cg.write_chunk(chunk_offset, chunk, start_address, update);
}

namespace
{
	class jit_scope_guard final
	{
		processor* processor_ = nullptr;

	public:
		jit_scope_guard() = delete;
		explicit _nothrow jit_scope_guard(processor& in_processor) noexcept : processor_(&in_processor)
		{
			processor_->in_jit = true;
		}
		jit_scope_guard(const jit_scope_guard&) = delete;
		_nothrow jit_scope_guard(jit_scope_guard&& other) noexcept
			: processor_(other.processor_)
		{
			other.processor_ = nullptr;
		}

		jit_scope_guard& operator=(const jit_scope_guard&) = delete;
		_nothrow jit_scope_guard& operator=(jit_scope_guard&& other) noexcept
		{
			processor_ = other.processor_;
			other.processor_ = nullptr;
			return *this;
		}

		_nothrow ~jit_scope_guard() noexcept
		{
			if (auto* const processor = processor_) [[likely]]
			{
				processor->in_jit = false;
			}
		}
	};
}

void jit1::execute_instruction(const uint32 address)
{
	current_executing_chunk_address_ = address;

	processor& processor = processor_;
	const auto* guest = processor.get_guest_system();

	// RCX, `mov`d from the machine code	// RDX		 // R8
	auto instruction = get_instruction(address);
	__pragma(pack(1)) struct
	{
		uintptr coprocessor1;
		uint32 flags;
		uint32 delay_branch_target;
		uint64 target_instruction;
		uint32 frame_pointer;
	} parameter_pack = {
		.coprocessor1 = uintptr(processor.get_coprocessor<1>()),
		.flags = uint32(processor.flags_),
		.delay_branch_target = processor.branch_target_,
		.target_instruction = processor.target_instructions_,
		.frame_pointer = processor.get_register<>(processor::named_registers::frame_pointer)
	};

	processor.increment_jit_transition_statistic();

	const uintptr result = [&] {
		jit_scope_guard guard{processor};
		return jit1_springboard(uintptr(instruction), uintptr(&processor), processor.instruction_count_, uintptr(&parameter_pack), 0, 0);
	}();
	if _unlikely(guest->is_execution_complete()) [[unlikely]]
	{
		if _likely(guest->is_execution_success()) [[likely]] {
			ExecutionCompleteException::throw_helper();
		}
		else {
			ExecutionFailException::throw_helper();
		}
	}
	if (processor.get_flags(processor::flag::trapped_exception))
	{
		// there was an exception.
		processor.clear_flags(processor::flag::trapped_exception);
		processor.trapped_exception_.rethrow_helper();
	}
}

jit1::jit_instructionexec_t jit1::get_instruction(const uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);

	Chunk * __restrict chunk;
	ChunkOffset * __restrict chunk_offset;

	bool chunk_exists = true;

#if USE_CACHE
	const cache_element* cache = nullptr;

	for (const auto& cached : last_cached)
	{
		if (cached.chunk_address_ == mapped_address)
		{
			cache = &cached;
			break;
		}
	}

	if _likely(cache) [[likely]]
	{
		chunk = cache->chunk_;
		chunk_offset = cache->chunk_offset_;
	}
	else
#endif
	{
		const auto cached_value = lookup_cache_.get(address);
		if _likely(!lookup_cache_.is_sentinel(cached_value)) [[likely]] {
			return cached_value;
		}
		/*
		if (const auto cached_value = lookup_cache_.get(address); cached_value.has_value()) {
			return cached_value.value();
		}
		*/

		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */
		const auto result = jit_map_->get(address);
		chunk_exists = result.second;
		const auto& __restrict chunk_data = result.first;

		chunk = chunk_data.chunk.get();
		chunk_offset = chunk_data.offset;

#if USE_CACHE
		last_cached[last_cached_index++ % last_cached.size()] = {
			.chunk_ = chunk,
			.chunk_offset_ = chunk_offset,
			.chunk_address_ = mapped_address
		};
#endif
	}

	const uint32 address_offset = (address - mapped_address) / 4u;

	if (!chunk_exists) {
		populate_chunk(*chunk_offset, *chunk, mapped_address, false);
	}

	const auto* __restrict chunk_instruction = chunk->m_data + (*chunk_offset)[address_offset];

	const auto result = std::bit_cast<const jit_instructionexec_t>(chunk_instruction);

	lookup_cache_.set(address, result);

	return result;
}

jit1::jit_instructionexec_t jit1::fetch_instruction(const uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	Chunk* __restrict chunk;
	ChunkOffset* __restrict chunk_offset;

#if USE_CACHE
	const cache_element* cache = nullptr;

	for (const auto& cached : last_cached)
	{
		if (cached.chunk_address_ == mapped_address)
		{
			cache = &cached;
			break;
		}
	}

	if _likely(cache) [[likely]]
	{
		chunk = cache->chunk_;
		chunk_offset = cache->chunk_offset_;
	}
	else
#endif
	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */
		const auto* result = jit_map_->fetch(address);
		if (result == nullptr)
		{
			return nullptr;
		}
		const auto& __restrict chunk_data = *result;

		chunk = chunk_data.chunk.get();
		chunk_offset = chunk_data.offset;

#if USE_CACHE
		last_cached[last_cached_index++ % last_cached.size()] = {
			.chunk_ = chunk,
			.chunk_offset_ = chunk_offset,
			.chunk_address_ = mapped_address
		};
#endif
	}

	const auto* __restrict chunk_instruction = chunk->m_data + (*chunk_offset)[address_offset];

	return std::bit_cast<jit_instructionexec_t>(chunk_instruction);
}

jit1::Chunk * jit1::get_chunk(const uint32 address) const
{
#if USE_CACHE
	const uint32 mapped_address = address & ~(ChunkSize - 1);

	for (const auto& cached : last_cached)
	{
		if (cached.chunk_address_ == mapped_address)
		{
			return cached.chunk_;
		}
	}
#endif

	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */

		const auto* result = jit_map_->fetch(address);
		if (result == nullptr)
		{
			return nullptr;
		}

		return result->chunk.get();
	}
}

_nothrow void jit1::Chunk::release() noexcept
{
	if (auto* data = m_data)
	{
		free_executable_chunk(data);
		m_data = nullptr;
	}
}

_nothrow jit1::chunk_data::~chunk_data() noexcept {
	m_chunkoffset_allocator.release(offset);

	release();
}

_nothrow void jit1::chunk_data::release() noexcept {
	chunk.reset();
}
