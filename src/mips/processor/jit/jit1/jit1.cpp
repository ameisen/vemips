#include "pch.hpp"

#include "jit1.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "instructions/instructions_table.hpp"
#include "../../processor.hpp"
#include <cassert>
#include "codegen.hpp"
#include <algorithm>

#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN  // NOLINT(clang-diagnostic-unused-macros)
#include <Windows.h>

#include <mutex>

#include "system.hpp"

using namespace mips;

#define JIT_INSTRUCTION_SEPARATE 0
#define JIT_INSERT_IDENTIFIERS 0

// this can and should be global to all JITs.
namespace {
	static constinit size_t global_exec_data_size = 0x1000;
	static std::mutex g_global_exec_data_lock;
	static std::shared_ptr<char[]> g_global_exec_data;
	static uintptr g_ex_return = 0;
}

extern "C" uintptr jit1_springboard(uintptr instruction, uintptr processor, uintptr pc_runner, uintptr, uintptr, uintptr);
extern "C" void jit1_drop_signal();

namespace {
	static std::mutex free_chunks_lock;
	static std::vector<void*> free_chunks;
	static const size_t allocation_granularity = []
	{
		SYSTEM_INFO info = {};
		GetSystemInfo(&info);
		return info.dwAllocationGranularity;
	}();

	_forceinline __declspec(restrict, nothrow) void* allocate_executable(const size_t size) {
		return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	}

	template <size_t ChunkSize>
	_forceinline __declspec(restrict, nothrow) void* allocate_executable_chunk() {
		{
			std::unique_lock lock{free_chunks_lock};
			if (!free_chunks.empty())
			{
				auto result = free_chunks.back();
				free_chunks.pop_back();
				return result;
			}
		}

		const size_t alloc_size = std::max(
			allocation_granularity,
			ChunkSize
		);

		void* new_ptr = VirtualAlloc(nullptr, alloc_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

		const size_t num_chunks = alloc_size / ChunkSize;

		uintptr ptr = uintptr(new_ptr);
		{
			std::unique_lock lock{free_chunks_lock};
			for (size_t i = 1; i < num_chunks; ++i)
			{
				ptr += ChunkSize;
				free_chunks.push_back(reinterpret_cast<void*>(ptr));
			}
		}

		return new_ptr;
	}

	_forceinline __declspec(nothrow) void free_executable(void * const ptr) {
		VirtualFree(ptr, 0, MEM_RELEASE);
	}

	_forceinline __declspec(nothrow) void free_executable_chunk(void * const ptr) {
		std::unique_lock lock{free_chunks_lock};
		free_chunks.push_back(ptr);
	}
}

void jit1::initialize_chunk(chunk_data& __restrict chunk) {
	if (!chunk.chunk) {
		chunk.chunk = new Chunk;

		chunk.offset = m_chunkoffset_allocator.allocate();
		memset(chunk.offset->data(), 0, chunk.offset->size() * sizeof(uint32));
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

jit1::jit_instructionexec_t jit1_get_instruction(jit1 * __restrict _this, uint32 address) {
	auto result = _this->get_instruction(address);
	return result;
}

jit1::jit_instructionexec_t jit1_fetch_instruction(jit1* __restrict _this, uint32 address) {
	auto result = _this->fetch_instruction(address);
	return result;
}

namespace {
	void RI_Exception (instruction_t, processor & __restrict _processor) {
		_processor.set_trapped_exception({ CPU_Exception::Type::RI, _processor.get_program_counter() });
	}

	void OV_Exception (uint32 code, processor & __restrict _processor) {
		_processor.set_trapped_exception({ CPU_Exception::Type::Ov, _processor.get_program_counter(), code });
	}

	void TR_Exception (uint32 code, processor & __restrict _processor)
	{
		_processor.set_trapped_exception({ CPU_Exception::Type::Tr, _processor.get_program_counter(), code });
	}

	void AdEL_Exception (uint32 address, processor & __restrict _processor)
	{
		_processor.set_trapped_exception({ CPU_Exception::Type::AdEL, _processor.get_program_counter(), address });
	}

	void AdES_Exception(uint32 address, processor & __restrict _processor)
	{
		_processor.set_trapped_exception({ CPU_Exception::Type::AdES, _processor.get_program_counter(), address });
	}
}

jit1::jit1(processor & __restrict _processor) : processor_(_processor)
{
	// todo : not thread safe
	std::unique_lock lock{g_global_exec_data_lock};

	if (g_global_exec_data.get() == nullptr)
	{
		char* const exec_data = static_cast<char *>(allocate_executable(global_exec_data_size));

		g_global_exec_data = global_exec_data = {
			exec_data,
			&free_executable
		};

		Jit1_CodeGen cg{ *this, reinterpret_cast<uint8 *>(exec_data), global_exec_data_size };
		{
			g_ex_return = uintptr(exec_data + cg.getSize());

			static const int8 flags_offset = value_assert<int8>(offsetof(processor, flags_) - 128);
			static const int8 dbt_offset =  value_assert<int8>(offsetof(processor, branch_target_) - 128);
			static const int8 ic_offset =  value_assert<int8>(offsetof(processor, instruction_count_) - 128);

			cg.mov(cg.qword[cg.rbp + ic_offset], cg.rdi);		// save instruction count
			cg.mov(cg.dword[cg.rbp + flags_offset], cg.ebx);
			cg.mov(cg.dword[cg.rbp + dbt_offset], cg.esi);  // set it in the interpreter
			cg.mov(cg.dword[cg.rbp + instructions::GPRegister<>{Jit1_CodeGen::mips_fp}.get_offset()], cg.r15d);
			cg.mov(cg.rax, int64(jit1_drop_signal));
			cg.jmp(cg.rax);
		}

		cg.ready();

		_clear_cache(exec_data, exec_data + cg.getSize());
	}
}

jit1::~jit1() = default;


namespace
{
	// // RCX, mov'd from the machine code	// RDX		 // R8
	uintptr JumpFunction(instruction_t i, processor & __restrict p, uintptr f, uintptr rspv)
	{
		const mips::instructions::instructionexec_t executable = mips::instructions::instructionexec_t(f);
		executable(i, p);
		return rspv + 4;
	}

	struct bytes final
	{
		const uint8 * __restrict data;
		const size_t size;

		template <size_t N>
		bytes(const uint8(&data) [N]) :
			data(data),
			size(N)
		{
		}
	};

	struct base_fixup
	{
		uint32 offset;
		uintptr chunk_address = 0;

		base_fixup(const uint32 offset) :
			offset(offset) {}
	};

	template <typename T>
	struct fixup : public base_fixup
	{
		fixup(const uint32 offset) : base_fixup(offset) {}
	};


	struct vector_wrapper
	{
		std::vector<uint8> & __restrict data;
		std::vector<base_fixup> & __restrict fixups;

		template <typename T>
		vector_wrapper & operator << (const T &value);

		template <typename U>
		vector_wrapper & operator << (fixup<U> value);
	};


	template <typename T>
	vector_wrapper & vector_wrapper::operator << (const T &value)
	{
		const size_t sz = data.size();
		data.resize(sz + sizeof(T));
		*(T *)(data.data() + sz) = value;
		return *this;
	};

	template<>
	vector_wrapper & vector_wrapper::operator << <bytes> (const bytes &value)
	{
		const size_t sz = data.size();
		data.resize(sz + value.size);
		memcpy(data.data() + sz, value.data, value.size);
		return *this;
	};

	template <>
	vector_wrapper & vector_wrapper::operator << <uint8> (const uint8 &value)
	{
		data.push_back(value);
		return *this;
	};

	using instruction_pack = std::pair<uint8[32], size_t>;

	template <>
	vector_wrapper & vector_wrapper::operator << <instruction_pack> (const instruction_pack &value)
	{
		const size_t sz = data.size();
		data.resize(sz + value.second);
		memcpy(data.data() + sz, value.first, value.second);
		return *this;
	};

	template <typename U>
	vector_wrapper & vector_wrapper::operator << (fixup<U> value)
	{
		value.chunk_address = data.size();
		fixups.push_back(value);
		*this << U(0);
		return *this;
	}

	void collect_stats(const char * __restrict instruction, processor & __restrict processor)
	{
		processor.add_stat(instruction);
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

	cache_flusher scoped_cache_flusher(*this);

	const uint32 base_address = start_address & ~(jit1::ChunkSize - 1);
	const uint32 last_address = base_address + (jit1::ChunkSize - 1);

	static const int8 flags_offset = value_assert<int8>(offsetof(processor, flags_) - 128);
	static const int8 pc_offset =  value_assert<int8>(offsetof(processor, program_counter_) - 128);
	static const int8 dbt_offset =  value_assert<int8>(offsetof(processor, branch_target_) - 128);
	static const int8 ic_offset =  value_assert<int8>(offsetof(processor, instruction_count_) - 128);

	uint32 chunk_start_offset = 0;

	uint32 start_index = 0;
	if (update)
	{
		if (chunk.m_has_fixups)
		{
			chunk.m_has_fixups = false;
		}
		// Can't make this work with xbyak
		/*
		else
		{
			start_index = start_address - chunk.m_offset;
			start_index /= 4u;
			// also shift the 'data' pointer to this offset.
			chunk_start_offset = chunk_offset[start_index];
		}
		*/
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

	static constexpr uint32 num_instructions = jit1::NumInstructionsChunk;
	for (uint32 i = start_index; i < num_instructions; ++i)
	{
		const auto& cur_label = get_instruction_offset_label(i);
		L(cur_label);
		chunk_offset[i] = uint32(this->getSize()) + chunk_start_offset;

		bool cti = false;
		bool can_except = false;
		bool terminate_instruction = false;
		bool alters_memory = false;
		bool cti_test = false;
		bool compact_branch = false;
		bool delay_branch = false;
		bool possible_after_delaybranch = false;
		bool compact_branch_suffix_required = false;
		bool exceptions_handled = false;
		bool store_handled = false;

		const uint32 current_address = start_address;

		instruction_t prev_instruction = 0;
		const instructions::InstructionInfo * __restrict prev_instruction_info_ptr = nullptr;
		const instructions::InstructionInfo * __restrict instruction_info_ptr = nullptr;

#if JIT_INSERT_IDENTIFIERS
		const Xbyak::Label id_label;
		jmp(id_label);
		nop(4, false);
		mov(eax, int32(current_address));
		nop(4, false);
		L(id_label);
#endif
		
		if (jit_.processor_.ticked_ && jit_.processor_.debugging_)
		{
			mov(rax, int64(&should_debug_break));
			mov(dword[rbp + pc_offset], int32(current_address));
			mov(rcx, rbp);
			add(rcx, -128);
			call(rax);
			test(eax, eax);
			jnz(intrinsics_.save_return, T_NEAR);
			cmp(rdi, r14);
			je(intrinsics_.save_return, T_NEAR);
		}
		else if (jit_.processor_.debugging_)
		{
			mov(rax, int64(&should_debug_break));
			mov(dword[rbp + pc_offset], int32(current_address));
			mov(rcx, rbp);
			add(rcx, -128);
			call(rax);
			test(eax, eax);
			jnz(intrinsics_.save_return, T_NEAR);
		}
		else if (jit_.processor_.ticked_)
		{
			// check if we are already at our tick count.
			cmp(rdi, r14);
			mov(eax, int32(current_address));
			je(intrinsics_.save_return_eax_pc, T_NEAR);
		}

		if (current_address >= 4)
		{
			const size_t ins_start_size = getSize();

			// Also fetch previous instruction.
			const instruction_t * __restrict prev_ptr = jit_.processor_.safe_mem_fetch_exec<const instruction_t>(current_address - 4);
			if (prev_ptr)
			{
				prev_instruction = *prev_ptr;
				prev_instruction_info_ptr = mips::FindExecuteInstruction(prev_instruction);

				if (prev_instruction_info_ptr && (prev_instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::SetNoCTI)) != 0)
				{
					cti_test = true;
				}
				if (prev_instruction_info_ptr && (prev_instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::DelayBranch)) != 0)
				{
					possible_after_delaybranch = true;
				}
			}

			const instruction_t * __restrict ptr = jit_.processor_.safe_mem_fetch_exec<const instruction_t>(current_address);
			if (ptr)
			{
				const instruction_t instruction = *ptr;
				instruction_info_ptr = mips::FindExecuteInstruction(instruction);

				if (instruction_info_ptr)
				{
					if (jit_.processor_.collect_stats_)
					{
						// dispatch a stat call.
						mov(rcx, int64(instruction_info_ptr->Name));
						call(intrinsics_.stats);
					}

					compact_branch = ((instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::CompactBranch)) != 0);
					delay_branch = ((instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::DelayBranch)) != 0);

					if (cti_test && !jit_.processor_.disable_cti_)
					{
						// CTI instruction prefix.
						if ((instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::ControlInstruction)) != 0)
						{
							const Xbyak::Label no_ex;

							test(ebx, processor::flag::no_cti);
							jz(no_ex);
							mov(eax, int32(current_address));
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
						insert_procedure_ecx(current_address, uint64(instruction_info_ptr->Proc), instruction, *instruction_info_ptr);
						can_except = true;
					}
					else if (instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::Store))
					{
						if (write_STORE(chunk_offset, current_address, instruction, *instruction_info_ptr))
						{
							store_handled = false;
							can_except = true;
							insert_procedure_ecx(current_address, uint64(instruction_info_ptr->Proc), instruction, *instruction_info_ptr);
						}
						else
						{
							store_handled = true;
						}
						alters_memory = true;
					}
					else if (
						(instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::Load | mips::instructions::OpFlags::COP1)) == uint32(mips::instructions::OpFlags::Load)
					)
					{
						if (write_LOAD(chunk_offset, current_address, instruction, *instruction_info_ptr))
						{
							insert_procedure_ecx(current_address, uint64(instruction_info_ptr->Proc), instruction, *instruction_info_ptr);
							can_except = true;
						}
					}
					else if (compact_branch)
					{
						compact_branch_suffix_required = write_compact_branch(chunk, terminate_instruction, chunk_offset, current_address, instruction, *instruction_info_ptr);
						exceptions_handled = !compact_branch_suffix_required;
					}
					else if (delay_branch)
					{
						exceptions_handled = !write_delay_branch(terminate_instruction, chunk_offset, current_address, instruction, *instruction_info_ptr);
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
						write_PROC_SUB(chunk_offset, current_address, instruction, *instruction_info_ptr);
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
					//	write_PROC_ADDI(chunk_offset, current_address, instruction, *instruction_info_ptr);
					//}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_ADDU))
					{
						write_PROC_ADDU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_ADD))
					{
						write_PROC_ADD(chunk_offset, current_address, instruction, *instruction_info_ptr);
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
						write_PROC_RDHWR(chunk_offset, terminate_instruction, current_address, instruction, *instruction_info_ptr);
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
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TEQ))
					{
						write_PROC_TEQ(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TGE))
					{
						write_PROC_TGE(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TGEU))
					{
						write_PROC_TGEU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TLT))
					{
						write_PROC_TLT(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TLTU))
					{
						write_PROC_TLTU(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_TNE))
					{
						write_PROC_TNE(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_SYSCALL))
					{
						write_PROC_SYSCALL(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else if (IS_INSTRUCTION(instruction_info_ptr, PROC_EXT))
					{
						write_PROC_EXT(chunk_offset, current_address, instruction, *instruction_info_ptr);
					}
					else
					{
						insert_procedure_ecx(current_address, uint64(instruction_info_ptr->Proc), instruction, *instruction_info_ptr);
						can_except = true;
					}

					cti = instruction_info_ptr->Flags.control;
				}
				else
				{
					// RI
					mov(eax, int32(current_address));
					jmp(intrinsics_.ri, T_NEAR);
				}
			}
			else
			{
				// AdEL
				mov(eax, int32(current_address));
				mov(ecx, eax);
				jmp(intrinsics_.adel, T_NEAR);
			}

			const size_t ins_size = getSize() - ins_start_size;
			jit_.largest_instruction_ = std::max(jit_.largest_instruction_, ins_size);
		}
		else
		{
			// AdEL
			if (current_address == 0U)
			{
				xor_(eax, eax);
				xor_(ecx, ecx);
			}
			else
			{
				mov(eax, int32(current_address));
				mov(ecx, eax);
			}
			jmp(intrinsics_.adel, T_NEAR);
		}
		// Epilog for store ops, as we need to test afterwards to see if they may have altered JIT memory.
		//if (!terminate_instruction)
		{
			// increment pc
			// 49 FF C0
			// inc r8
			inc(rdi);
		}
		if (!terminate_instruction)
		{
			// TODO is this right? Will the program counter be correct?
			if (alters_memory && !store_handled)
			{
				// Presently stores are all subroutine calls, which save pc.

				// 8A 42 7F 84 C0 74 10 49 FF C0 4C 01 42 7F 41 C1 E0 02 44 01 42 7F C3 
				// 8A 42 7F 84 C0 74 08 49 FF C0 44 01 42 7F C3 
				// mov al, byte [rdx + 0x7F]
				// test al, al
				// je no_flush
				// inc r8
				// add qword [rdx + 0x7F], r8 ; ic_offset
				// shl r8d, 2
				// add dword [rdx + 0x7F], r8d ; pc_offset
				// ret
				// no_flush:
				test(ebx, processor::flag::jit_mem_flush);
				jnz(intrinsics_.store_flush, T_NEAR);
			}
			// Handle compact branch.
			if (compact_branch && compact_branch_suffix_required)
			{
				// 8A 42 7F 84 C0 74 6A 48 FF 42 7F C6 42 7F 00 8B 42 7F B9 FF FF FF 7F 39 C8 72 2B 3D FF FF FF 7F 77 24 29 C8 48 B9 FF FF FF FF FF FF FF 7F 8B 04 01 48 B9 FF FF FF FF FF FF FF 7F 48 01 C8 45 31 C0 48 31 C9 FF E0 52 48 89 C2 48 83 EC 20 48 B8 FF FF FF FF FF FF FF 7F 48 B9 FF FF FF FF FF FF FF 7F FF D0 48 83 C4 20 5A 45 31 C0 48 31 C9 FF E0 
				// mov al, byte [rdx + 0x7F]			  ; load [pcc_offset] (program counter changed)
				// test al, al								 ; is pcc_offset 0?
				// je no_change								; if so, jump past this routine
				// inc qword [rdx + 0x7F]				  ; increment the [ic_offset] instruction counter
				// mov byte [rdx + 0x7F], 0				; set [pcc_offset] to 0
				// mov eax, dword [rdx + 0x7F]			; load [pc_offset] (program counter), zero extended, set by the compact branch instruction
				//												 ; check first if this offset is within the current chunk.
				// mov ecx, 0x7FFFFFFF					  ; load the [chunk base]
				// cmp eax, ecx								; is the address below chunk base?
				// jb not_within							  ; if so, skip to 'not_within'
				// cmp eax, 0x7FFFFFFF					  ; is the address above [chunk last]? (chunk base + chunk size - 1) (because of 0x1'0000'0000)
				// ja not_within							  ; if so, skip to 'not within'
				//												 ; if we are jumping internally, we can hardcode the offset
				// sub eax, ecx								; subtract chunk base from our address, giving us an offset within the chunk.
				//												 ; this offset is 4-byte aligned, as are all instructions. Luckily, so is 'chunk_offset'.
				// mov rcx, qword 0x7FFFFFFFFFFFFFFF	; load chunk_offset (uint32[])
				// mov eax, dword [rcx + rax]			 ; load chunk_offset[rax] (rax vs eax doesn't matter here, the value is 32-bit zero-extended)
				// mov rcx, qword 0x7FFFFFFFFFFFFFFF	; load chunk data pointer [pointer fixed up after generation of chunk]
				// add qword rax, rcx						; add the actual offset to the chunk data pointer, which is our jump target
				// xor r8d, r8d								; clear r8d (program counter delta)
				// xor rcx, rcx								; zero rcx, as our JIT guarantees that it is 0 at the start of instructions.
				// jmp rax									  ; jump to the local jump target
				// not_within:								 ; otherwise, we are jumping to a remove jump target
				// 
				// push rdx									 ; push rdx [stores 'processor' pointer + 128, a value we must retain]
				// mov rdx, rax								; move rax to rdx, which is the second argument in Win64 ABI
				// sub rsp, 32								 ; push the home space required by the Win64 ABI
				// mov rax, 0x7FFFFFFFFFFFFFFF			; load the address of [jit1::get_instruction]
				// mov rcx, 0x7FFFFFFFFFFFFFFF			; load the [this] pointer into rcx, which is the first argument in Win64 ABI
				// call rax									 ; call [jit1::get_instruction]
				// add rsp, 32								 ; pop the home space
				// pop rdx									  ; restore rdx
				// xor r8d, r8d								; clear r8d (program counter delta)
				// xor rcx, rcx								; zero rcx, as our JIT guarantees that it is 0 at the start of instructions.
				// jmp rax									  ; jump to the remote jump target
				// no_change:								  ; otherwise, the subroutine (unencoded) compact branch did not trigger a branch.
				const Xbyak::Label no_change;
				const Xbyak::Label not_within;

				test(ebx, processor::flag::pc_changed);
				jz(no_change);
				and_(ebx, ~processor::flag::pc_changed);
				mov(eax, dword[rbp + pc_offset]);

				mov(ecx, eax);
				and_(ecx, ~(jit1::ChunkSize - 1));
				cmp(ecx, base_address);
				jne(not_within);
				and_(eax, (jit1::ChunkSize - 1));

				mov(rcx, uint64(chunk_offset.data()));
				mov(eax, dword[rcx + rax]);
				mov(rcx, intrinsics_.chunk_start);
				add(rax, rcx);
				jmp(rax);
				L(not_within);

				mov(rdx, rax);
				mov(rax, uint64(jit1_get_instruction));
				mov(rcx, uint64(&jit_));
				call(rax);
				jmp(rax);
				L(no_change);
			}
			// exception checking epilog
			if (can_except && !exceptions_handled)
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
			if (possible_after_delaybranch)
			{
				handle_delay_branch(chunk, chunk_offset, current_address - 4, prev_instruction, *prev_instruction_info_ptr);
			}
		}
#if JIT_INSTRUCTION_SEPARATE
		nop(8, false);
#endif

		start_address += 4;
	}

	const auto patch_preprolog = [&](auto address) -> Xbyak::Label
	{
		// If execution gets past the chunk, we jump to the next chunk.
		// Start with a set of nops so that we have somewhere to write patch code.
		auto patch = L(); // patch should be 12 bytes. Enough to copy an 8B pointer to rax, and then to jump to it.
		chunk.m_patches.push_back({ uint32(getSize()), 0 });
		auto &patch_pair = chunk.m_patches.back();
		uint32 &patch_target = patch_pair.target;

		// patch no-op
		if (address == nullptr) {
			db(0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x00);
		}
		else {
			static constexpr uint16 patch_prefix = 0xB848;
			static constexpr uint16 patch_suffix = 0xE0FF;
			dw(patch_prefix);
			dq(uint64(address));
			dw(patch_suffix);
		}

		mov(rcx, int64(&patch_target));
		mov(dword[rcx], edx);

		return patch;
	};

	const auto patch_prolog = [&]()
	{
		auto &patch_pair = chunk.m_patches.back();
		uint32 &patch_target = patch_pair.target;
		mov(rcx, int64(&patch_target));
		mov(dword[rcx], edx);
	};

	const auto patch_epilog = [&](const Xbyak::Label &patch)
	{
		static constexpr uint16 patch_prefix = 0xB848;
		static constexpr uint16 patch_suffix = 0xE0FF;

		mov(rcx, patch);
		mov(word[rcx], int16_t(patch_prefix));
		mov(qword[rcx + 2], rax);
		mov(word[rcx + 10], int16_t(patch_suffix));
	};

	const auto patch = patch_preprolog(jit_.fetch_instruction(uint32(last_address + 1)));

	mov(edx, uint32(last_address + 1));

	patch_prolog();

	mov(dword[rbp + pc_offset], edx);
	mov(rax, uint64(jit1_get_instruction));
	mov(rcx, uint64(&jit_));
	call(rax);

	patch_epilog(patch);

	jmp(rax);

	{
		const Xbyak::Label intrinsic_ex;
		const Xbyak::Label save;

		L(intrinsics_.intrinsic_start);

		bool exception_label_used = false;

		if (intrinsics_.ri.used) {
			L(intrinsics_.ri);
			mov(rax, uint64(RI_Exception));
			jmp(intrinsic_ex);
			exception_label_used = true;
		}

		if (intrinsics_.adel.used) {
			L(intrinsics_.adel);
			mov(rax, uint64(AdEL_Exception));
			jmp(intrinsic_ex);
			exception_label_used = true;
		}

		if (intrinsics_.ades.used) {
			L(intrinsics_.ades);
			mov(rax, uint64(AdES_Exception));
			jmp(intrinsic_ex);
			exception_label_used = true;
		}

		if (intrinsics_.ov.used) {
			L(intrinsics_.ov);
			mov(rax, uint64(OV_Exception));
			jmp(intrinsic_ex);
			exception_label_used = true;
		}

		if (intrinsics_.tr.used) {
			L(intrinsics_.tr);
			mov(rax, uint64(TR_Exception));
			jmp(intrinsic_ex);
			exception_label_used = true;
		}

		if (exception_label_used) {
			L(intrinsic_ex);
			call(save);
			mov(rdx, rbp);
			add(rdx, -128);
			add(rsp, 40);
			jmp(rax);
		}

		if (intrinsics_.store_flush.used) {
			L(intrinsics_.store_flush);
			jmp(intrinsics_.save_return);
		}

		if (intrinsics_.check_ex.used) {
			L(intrinsics_.check_ex);
			jmp(intrinsics_.save_return);
		}

		if (intrinsics_.save_return_eax_pc.used) {
			L(intrinsics_.save_return_eax_pc);
			mov(dword[rbp + pc_offset], eax);
			jmp(intrinsics_.save_return);
		}

		L(save);
		mov(qword[rbp + ic_offset], rdi);		// save instruction count
		mov(dword[rbp + flags_offset], ebx);
		mov(dword[rbp + dbt_offset], esi);  // set it in the interpreter
		mov(dword[rbp + instructions::GPRegister<>{Jit1_CodeGen::mips_fp}.get_offset()], r15d);
		ret();

		if (intrinsics_.save_return.used) {
			L(intrinsics_.save_return);
			call(save);
			add(rsp, 40);
			ret();
		}

		if (jit_.processor_.collect_stats_) {
			// dispatch a stat call.
			if (intrinsics_.stats.used) {
				L(intrinsics_.stats);
				mov(rax, uint64(collect_stats));
				mov(rdx, rbp);
				add(rdx, -128);
				sub(rsp, 40);
				call(rax);
				add(rsp, 40);
				ret();
			}
		}
}

	xassert(!hasUndefinedLabel());
	ready();
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
		std::sort(jit_.chunks_.begin(), jit_.chunks_.end());
	}
}

void jit1::populate_chunk(ChunkOffset & __restrict chunk_offset, Chunk & __restrict chunk, const uint32 start_address, const bool update)
{
	if (!chunk.m_data)
	{
		chunk.m_datasize = MaxChunkRealSize;
		chunk.m_data = (uint8 *)allocate_executable_chunk<MaxChunkRealSize>();
	}
	Jit1_CodeGen cg{ *this, (uint8 *)chunk.m_data, jit1::MaxChunkRealSize };
	cg.write_chunk(chunk_offset, chunk, start_address, update);
}

// TODO may also need to update the _following_ chunk if we are altering the last instruction of this chunk.
bool jit1::memory_touched(const uint32 address)
{
	// Presume this is a sorted vector by address. We want to find a chunk that contains this address, if there is one.
	// TODO replace this with log n binary search.

	const uint32 mapped_address = address & ~(ChunkSize - 1);

	if (Chunk * __restrict dirty_chunk = get_chunk(address))
	{
		const uint32 adjusted_address = address & ~((1 << 2) - 1);
		if (mapped_address == (current_executing_chunk_address_& ~(ChunkSize - 1)))
		{
			flush_address_ = adjusted_address;
			processor_.set_flags(processor::flag::jit_mem_flush);
			flush_chunk_ = dirty_chunk;
			return true;
		}
		else
		{
			populate_chunk(*(dirty_chunk->m_chunk_offset), *dirty_chunk, adjusted_address, true);
			if (
				const auto next_address = adjusted_address + 4u;
				next_address == dirty_chunk->m_offset + ChunkSize
			)
			{
				// At the end of the chunk. Also update next chunk if it exists.
				if (memory_touched(next_address) && next_address == mapped_address)
				{
					return true;
				}
			}
		}
	}
	return false;
	/*
	for (Chunk *chunk : m_Chunks)
	{
		if (chunk->m_offset == mapped_address)
		{
			const uint32 adjusted_address = address & ~((1 << 2) - 1);
			if (mapped_address == (m_CurrentExecutingChunkAddress& ~(ChunkSize - 1)))
			{
				m_FlushAddress = adjusted_address;
				processor_.m_jit_meta = 1;
				m_FlushChunk = chunk;
			}
			else
			{
				populate_chunk(*(chunk->m_chunk_offset), *chunk, adjusted_address, true);
				if (adjusted_address + 4 == chunk->m_offset + ChunkSize)
				{
					// At the end of the chunk. Also update next chunk if it exists.
					memory_touched(adjusted_address + 4);
				}
			}
			break;
		}
	}
	*/
}

namespace
{
	struct JitScopeGuard final
	{
		processor& processor_;

		JitScopeGuard(processor& in_processor) : processor_(in_processor)
		{
			processor_.in_jit = true;
		}

		~JitScopeGuard()
		{
			processor_.in_jit = false;
		}
	};
}

void jit1::execute_instruction(uint32 address)
{
	current_executing_chunk_address_ = address;

	auto&& processor = processor_;
	const auto guest = processor.get_guest_system();

	// RCX, mov'd from the machine code	// RDX		 // R8
	auto instruction = get_instruction(address);
	__pragma(pack(1)) struct
	{
		uintptr coprocessor1;
		uint32 flags;
		uint32 delay_branch_target;
		uint64 target_instruction;
		uint32 frame_pointer;
	} parameter_pack = {
		.coprocessor1 = uintptr(processor.get_coprocessor(1)),
		.flags = uint32(processor.flags_),
		.delay_branch_target = processor.branch_target_,
		.target_instruction = processor.target_instructions_,
		.frame_pointer = processor.registers_[30]
	};

	const uintptr result = [&] {
		JitScopeGuard guard{processor};
		return jit1_springboard(uintptr(instruction), uintptr(&processor), processor.instruction_count_, uintptr(&parameter_pack), 0, 0);
	}();
	if _unlikely(guest->is_execution_complete()) [[unlikely]]
	{
		if _likely(guest->is_execution_success()) [[likely]] {
			throw ExecutionCompleteException();
		}
		else {
			throw ExecutionFailException();
		}
	}
	if _unlikely(processor.get_flags(processor::flag::jit_mem_flush)) [[unlikely]]
	{
		populate_chunk(*flush_chunk_->m_chunk_offset, *flush_chunk_, flush_address_, true);
		const auto next_flush_address = flush_address_ + 4;
		if (next_flush_address == flush_chunk_->m_offset + ChunkSize)
		{
			memory_touched(next_flush_address);
		}
	}
	if (processor.get_flags(processor::flag::trapped_exception))
	{
		// there was an exception.
		processor.clear_flags(processor::flag::trapped_exception);
		throw processor.trapped_exception_;
	}
}

jit1::jit_instructionexec_t jit1::get_instruction(const uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);

	Chunk * __restrict chunk;
	ChunkOffset * __restrict chunk_offset;

	if _likely(last_chunk_address_ == mapped_address) [[likely]]
	{
		chunk = last_chunk_;
		chunk_offset = last_chunk_offset_;
	}
	else
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
		auto & __restrict cml2 = (*jit_map_)[address >> (32 - RemainingLog2)];
		auto & __restrict cml1 = cml2[(address >> (32 - RemainingLog2 - 8)) & 0xFF];

		const bool chunk_exists = cml1.contains((address >> (32 - RemainingLog2 - 16)) & 0xFF);
		auto&& __restrict chunk_data = cml1[(address >> (32 - RemainingLog2 - 16)) & 0xFF];
		chunk = chunk_data.chunk;
		chunk_offset = chunk_data.offset;

		if (!chunk_exists) {
			populate_chunk(*chunk_offset, *chunk, mapped_address, false);
		}

		last_chunk_ = chunk;
		last_chunk_offset_ = chunk_offset;
		last_chunk_address_ = mapped_address;
	}

	const uint32 address_offset = (address - mapped_address) / 4u;

	const auto & __restrict chunk_instruction = chunk->m_data + (*chunk_offset)[address_offset];

	const auto result = jit_instructionexec_t(chunk_instruction);

	lookup_cache_.set(address, result);

	return result;
}

jit1::jit_instructionexec_t jit1::fetch_instruction(const uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	Chunk* __restrict chunk;
	ChunkOffset* __restrict chunk_offset;

	if _likely(last_chunk_address_ == mapped_address) [[likely]]
	{
		chunk = last_chunk_;
		chunk_offset = last_chunk_offset_;
	}
	else
	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */
		auto& __restrict cml2 = (*jit_map_)[address >> (32 - RemainingLog2)];
		auto& __restrict cml1 = cml2[(address >> (32 - RemainingLog2 - 8)) & 0xFF];

		if (!cml1.contains((address >> (32 - RemainingLog2 - 16)) & 0xFF)) {
			return nullptr;
		}

		auto&& __restrict chunk_data = cml1[(address >> (32 - RemainingLog2 - 16)) & 0xFF];
		chunk = chunk_data.chunk;
		chunk_offset = chunk_data.offset;

		last_chunk_ = chunk;
		last_chunk_offset_ = chunk_offset;
		last_chunk_address_ = mapped_address;
	}

	const auto& __restrict chunk_instruction = chunk->m_data + (*chunk_offset)[address_offset];

	return jit_instructionexec_t(chunk_instruction);
}

jit1::Chunk * jit1::get_chunk(uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	if _likely(last_chunk_address_ == mapped_address) [[likely]]
	{
		return last_chunk_;
	}
	else
	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */

		const uint32 jitmap_idx = address >> (32 - RemainingLog2);
		if (!jit_map_->contains(jitmap_idx))
		{
			return nullptr;
		}
		auto & __restrict cml2 = (*jit_map_)[jitmap_idx];

		const uint32 cml2_idx = (address >> (32 - RemainingLog2 - 8)) & 0xFF;
		if (!cml2.contains(cml2_idx))
		{
			return nullptr;
		}
		auto & __restrict cml1 = cml2[cml2_idx];

		const uint32 cml1_idx = (address >> (32 - RemainingLog2 - 16)) & 0xFF;

		return cml1.contains(cml1_idx) ?
			cml1[cml1_idx].chunk :
			nullptr;
	}
}

_nothrow jit1::chunk_data::~chunk_data() noexcept {
	m_chunkoffset_allocator.release(offset);

	release();
}

_nothrow void jit1::chunk_data::release() noexcept {
	if (chunk) {
		if (chunk->m_data)
		{
			free_executable_chunk(chunk->m_data);
		}
		delete chunk;
		chunk = nullptr;
	}
}
