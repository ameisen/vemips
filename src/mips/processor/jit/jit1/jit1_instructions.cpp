#include "pch.hpp"

#include "jit1.hpp"
#include "../../processor.hpp"
#include "coprocessor/coprocessor1/coprocessor1.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "instructions/coprocessor1_support.hpp"
#include <cassert>
#include "codegen.hpp"
#include "instructions/instructions_support.hpp"
#include "platform/platform.hpp"

using namespace mips;

void Jit1_CodeGen::insert_procedure(const uint32 address, const void* const procedure, const uint32 argument0) {

	mov(dword[rbp + offsets.pc], address);
	mov(dword[rbp + offsets.flags], ebx);
	set(ecx, argument0);
	lea(rdx, qword[rbp - 128]);
	std::ignore = call_ex<true>(procedure, rax);
	//mov(dword[rbp + instructions::GPRegister<>{0}.get_offset(true)], 0); //clear register
	mov(ebx, dword[rbp + offsets.flags]);
}

void Jit1_CodeGen::insert_procedure(const uint32 address, const void* const procedure, const Xbyak::Operand& argument0) {

	std::ignore = mov_ex(ecx, argument0);
	mov(dword[rbp + offsets.pc], address);
	mov(dword[rbp + offsets.flags], ebx);
	lea(rdx, qword[rbp - 128]);
	std::ignore = call_ex<true>(procedure, rax);
	//mov(dword[rbp + instructions::GPRegister<>{0}.get_offset(true)], 0); //clear register
	mov(ebx, dword[rbp + offsets.flags]);
}

void Jit1_CodeGen::insert_procedure_hazard(const uint32 address [[maybe_unused]], const void* const procedure, const uint32 argument0) {
	const Xbyak::Label return_label;

	set(rax, reinterpret_cast<uintptr>(procedure));
	mov(rdx, return_label);
	set(ecx, address);
	set(r9, argument0);
	std::ignore = call_ex<true>(jit_.call_instruction_hazard_ptr_, r8);
	L(return_label);
	//mov(dword[rbp + instructions::GPRegister<>{0}.get_offset(true)], 0); //clear register
	mov(ebx, dword[rbp + offsets.flags]);
}

void Jit1_CodeGen::insert_procedure_hazard(const uint32 address [[maybe_unused]], const void* const procedure, const Xbyak::Operand& argument0) {
	const Xbyak::Label return_label;

	std::ignore = mov_ex(r9, argument0);
	set(rax, reinterpret_cast<uintptr>(procedure));
	mov(rdx, return_label);
	set(ecx, address);
	std::ignore = call_ex<true>(jit_.call_instruction_hazard_ptr_, r8);

	L(return_label);
	//mov(dword[rbp + instructions::GPRegister<>{0}.get_offset(true)], 0); //clear register
	mov(ebx, dword[rbp + offsets.flags]);
}

Jit1_CodeGen::except_result Jit1_CodeGen::insert_procedure_check_hazard(const uint32 address, const mips::instructions::InstructionInfo& __restrict instruction_info, const uint32 argument0)
{
	if (instructions::HasAnyFlags(instruction_info.OpFlags, instructions::OpFlags::InstructionHazard)) [[unlikely]]
	{
		mov(eax, int32(address));
		or_(ebx, processor::flag::instruction_hazard);
		jmp(intrinsics_.save_return_eax_pc, T_NEAR);

		return except_result::always_exits;
	}
	else
	{
		insert_procedure(address, std::bit_cast<void*>(instruction_info.Proc), argument0);
		if (instructions::HasAnyFlags(instruction_info.OpFlags, instructions::OpFlags::Throws | instructions::OpFlags::Signals_All))
		{
			return except_result::can_except;
		}
		else
		{
			return except_result::none;
		}
	}
}

bool Jit1_CodeGen::interpret_if_hazard(const uint32 address, const mips::instructions::InstructionInfo& __restrict instruction_info)
{
	if (instructions::HasAnyFlags(instruction_info.OpFlags, instructions::OpFlags::InstructionHazard)) [[unlikely]]
	{
		mov(eax, address);
		or_(ebx, processor::flag::instruction_hazard);
		jmp(intrinsics_.save_return_eax_pc, T_NEAR);
		return true;
	}
	else
	{
		return false;
	}
}

namespace
{
	_nothrow static void ClearExecutionHazards(const uint32, processor & __restrict processor) noexcept
	{
		//const uint32 chunk_size = jit1::get_static_chunk_size();
		//const uint32 current_address = processor.get_program_counter();
		//const uint32 chunk_base = current_address & (~(chunk_size - 1));

		try
		{
			processor.clear_execution_hazards();
		}
		catch (...)
		{
			xunreachable("unexpected exception in `ClearExecutionHazards`");
		}
	}

	_nothrow static void* ClearInstructionHazards(const uint32 target_address, processor & __restrict processor) noexcept
	{
		const uint32 chunk_size = jit1::get_static_chunk_size();
		const uint32 current_address = processor.get_program_counter();
		const uint32 chunk_base = current_address & (~(chunk_size - 1));

		try
		{
			processor.clear_execution_hazards();
			if (processor.clear_instruction_hazards({{chunk_base, chunk_size}}))
			{
				const uint32 next_address = target_address;

				const auto* const return_chunk = (*std::get_if<jit1* __restrict>(&processor.jit_))->get_chunk(next_address);
				const uint32 instruction_offset = (next_address - return_chunk->m_offset) / sizeof(uint32);
				const uint32 chunk_offset = (*(return_chunk->m_chunk_offset))[instruction_offset];

				return return_chunk->m_data + chunk_offset;
			}

			return nullptr;
		}
		catch (...)
		{
			xunreachable("unexpected exception in `ClearInstructionHazards`");
		}
	}

	_nothrow static void SyncInstruction(const uint32 target_address, processor & __restrict processor) noexcept
	{
		try
		{
			//processor.mem_poke<char>(target_address);
			processor.invalidate_instruction_cache(target_address);
		}
		catch (...)
		{
			xunreachable("unexpected exception in `SyncInstruction`");
		}
	}
}

void Jit1_CodeGen::intrinsic_clear_execution_hazards(const uint32 address)
{
	// suboptimal
	insert_procedure(address, std::bit_cast<void*>(&ClearExecutionHazards), 0U);
}

void Jit1_CodeGen::intrinsic_clear_instruction_hazards(const uint32 address, const uint32 target_address)
{
	// suboptimal
	insert_procedure_hazard(address, std::bit_cast<void*>(&ClearInstructionHazards), target_address);
}

void Jit1_CodeGen::intrinsic_clear_instruction_hazards(const uint32 address, const Xbyak::Reg& target_address_reg)
{
	// suboptimal
	insert_procedure_hazard(address, std::bit_cast<void*>(&ClearInstructionHazards), target_address_reg);
}

void Jit1_CodeGen::write_PROC_SUBU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	// [rd] = [rs] - [rt]

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs == rt) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rs.is_zero())
	{
		if (rt == rd)
		{
			neg(op_rd);
		}
		else
		{
			if (op_rd.isREG())
			{
				mov(op_rd, op_rt);
				neg(op_rd);
			}
			else
			{
				mov(eax, op_rt);
				neg(eax);
				mov(op_rd, eax);
			}
		}
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		std::ignore = mov_ex(op_rd, op_rs, eax);
	}
	else if (rd == rs)
	{
		if (op_rd.isREG() || op_rt.isREG())
		{
			sub(op_rd, op_rt);
		}
		else
		{
			mov(eax, op_rt);
			sub(op_rd, eax);
		}
	}
	else
	{
		if (op_rd.isREG())
		{
			mov(op_rd, op_rs);
			sub(op_rd, op_rt);
		}
		else
		{
			mov(eax, op_rs);
			sub(eax, op_rt);
			mov(op_rd, eax);
		}
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_SUB(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	// [rd] = [rs] - [rt]

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
		return except_result::none;
	}
	else if (rs == rt) [[unlikely]]
	{
		set(op_rd, 0);
		return except_result::none;
	}
	else if (rs.is_zero())
	{
		mov(eax, op_rt);
		neg(eax);
		jo(intrinsics_.ov, T_NEAR);
		mov(op_rd, eax);
		return except_result::can_throw;
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		std::ignore = mov_ex(op_rd, op_rs, eax);
		return except_result::none;
	}
	else
	{
		mov(eax, op_rs);
		sub(eax, op_rt);
		jo(intrinsics_.ov, T_NEAR);
		mov(op_rd, eax);
		return except_result::can_throw;
	}
}

void Jit1_CodeGen::write_PROC_OR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs | rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero() && rt.is_zero()) [[unlikely]]
	{
		// set rd to 0
		// 89 4A EE
		// mov dword [rdx + 0xEE], ecx ; EE = 'rd' offset
		set(op_rd, 0);
	}
	else if (rs.is_zero() || rt == rs) [[unlikely]]
	{
		// just move rt to rd
		// 8B 42 DD 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		if (rd != rt)
		{
			std::ignore = mov_ex(op_rd, op_rt, eax);
		}
	}
	else if (rt.is_zero())
	{
		// just move rs to rd
		// 8B 42 DD 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		if (rd != rs)
		{
			std::ignore = mov_ex(op_rd, op_rs, eax);
		}
	}
	else
	{
		// actually perform OR
		// 8B 42 DD 0B 42 FF 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// or dword eax, [rdx + 0xFF] ; FF = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		auto&& tmp = op_rd.if_reg(eax);

		mov(tmp, op_rt);
		or_(tmp, op_rs);
		if (!op_rd.isREG())
		{
			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_NOR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs \/ rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero() && rt.is_zero()) [[unlikely]]
	{
		// set rd to all ones
		set(op_rd, int32(-1));
	}
	else if (rd == rs && rd == rt) [[unlikely]]
	{
		not_(op_rd);
	}
	else if (rs.is_zero() || rt == rs)
	{
		if (op_rd.isREG())
		{
			mov(op_rd, op_rt);
			not_(op_rd);
		}
		else
		{
			mov(eax, op_rt);
			not_(eax);
			mov(op_rd, eax);
		}
	}
	else if (rt.is_zero())
	{
		if (op_rd.isREG())
		{
			mov(op_rd, op_rs);
			not_(op_rd);
		}
		else
		{
			mov(eax, op_rs);
			not_(eax);
			mov(op_rd, eax);
		}
	}
	else
	{
		// actually perform OR
		// 8B 42 DD 0B 42 FF 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// or dword eax, [rdx + 0xFF] ; FF = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		auto&& tmp = op_rd.if_reg(eax);

		mov(tmp, op_rt);
		or_(tmp, op_rs);
		not_(tmp);
		if (!op_rd.isREG())
		{
			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_AND(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs & rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero() || rt.is_zero()) [[unlikely]]
	{
		// set rd to 0
		// 89 4A EE
		// mov dword [rdx + 0xEE], ecx ; EE = 'rd' offset
		set(op_rd, 0);
	}
	else if (rt == rs) [[unlikely]]
	{
		// just move rt to rd
		// 8B 42 DD 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		if (rd != rt)
		{
			std::ignore = mov_ex(op_rd, op_rt, eax);
		}
	}
	else
	{
		// actually perform OR
		// 8B 42 DD 0B 42 FF 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// or dword eax, [rdx + 0xFF] ; FF = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		auto&& tmp = op_rd.if_reg(eax);

		mov(tmp, op_rt);
		and_(tmp, op_rs);
		if (!op_rd.isREG())
		{
			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_ORI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs | zimm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const uint32 immediate = instructions::TinyInt<16>(instruction).zextend<uint32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		// set rt to immediate
		set(op_rt, immediate);
	}
	else if (immediate == 0) [[unlikely]]
	{
		if (rt == rs) [[unlikely]]
		{
			// nop
		}
		else
		{
			std::ignore = mov_ex(op_rt, op_rs, eax);
			// just move rs to rt
		}
	}
	else if (rt == rs)
	{
		or_(op_rt, immediate);
	}
	else
	{
		// actually perform OR
		if (op_rt.isREG())
		{
			mov(op_rt, op_rs);
			or_(op_rt, immediate);
		}
		else
		{
			mov(eax, immediate);
			or_(eax, get_register_op32(rs));
			mov(get_register_op32(rt), eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_ANDI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs & zimm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const uint32 immediate = instructions::TinyInt<16>(instruction).zextend<uint32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero() || immediate == 0) [[unlikely]]
	{
		set(op_rt, 0);
	}
	else if (rt == rs)
	{
		and_(op_rt, immediate);
	}
	else
	{
		// actually perform AND
		if (op_rt.isREG())
		{
			mov(op_rt, op_rs);
			and_(op_rt, immediate);
		}
		else
		{
			mov(eax, immediate);
			and_(eax, op_rs);
			mov(op_rt, eax);
		}
	}
}

// TODO optimize
void Jit1_CodeGen::write_PROC_SELEQZ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt ? 0 : rs
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// select always hits.
		std::ignore = mov_ex(op_rd, op_rs, eax);
	}
	else
	{
		if (rt == rs)
		{
			if (rd == rt || !op_rt.isREG())
			{
				mov(ecx, op_rt);
				cmp(ecx, 1); // Set carry flag if zero
				if (op_rd.isREG())
				{
					sbb(op_rd, op_rd);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(op_rd, ecx);
				}
				else
				{
					sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(eax, ecx);
					mov(op_rd, eax);
				}
			}
			else if (!op_rt.isREG())
			{
				mov(ecx, op_rt);
				cmp(ecx, 1); // Set carry flag if zero
				if (op_rd.isREG())
				{
					sbb(op_rd, op_rd);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(op_rd, ecx);
				}
				else
				{
					sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(eax, ecx);
					mov(op_rd, eax);
				}
			}
			else
			{
				cmp(op_rt, 1); // Set carry flag if zero
				if (op_rd.isREG())
				{
					sbb(op_rd, op_rd);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(op_rd, op_rt);
				}
				else
				{
					sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(eax, op_rt);
					mov(op_rd, eax);
				}
			}
		}
		else
		{
			cmp(op_rt, 1); // Set carry flag if zero
			if (op_rd.isREG())
			{
				sbb(op_rd, op_rd);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
				and_(op_rd, op_rs);
			}
			else
			{
				sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
				and_(eax, op_rs);
				mov(op_rd, eax);
			}
		}
	}
}

// TODO optimize
void Jit1_CodeGen::write_PROC_SELNEZ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt ? rs : 0
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// select never hits.
		set(op_rd, 0);
	}
	else
	{
		if (rt == rs)
		{
			if (rd == rt || !op_rt.isREG())
			{
				mov(ecx, op_rt);
				cmp(ecx, 1);							// Set carry flag if zero
				cmc();
				if (op_rd.isREG())
				{
					sbb(op_rd, op_rd);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(op_rd, ecx);
				}
				else
				{
					sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(eax, ecx);
					mov(op_rd, eax);
				}
			}
			else if (!op_rt.isREG())
			{
				cmp(op_rt, 1);							// Set carry flag if zero
				cmc();
				if (op_rd.isREG())
				{
					sbb(op_rd, op_rd);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(op_rd, op_rt);
				}
				else
				{
					sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(eax, op_rt);
					mov(op_rd, eax);
				}
			}
			else
			{
				mov(ecx, op_rt);
				cmp(ecx, 1);							// Set carry flag if zero
				cmc();
				if (op_rd.isREG())
				{
					sbb(op_rd, op_rd);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(op_rd, ecx);
				}
				else
				{
					sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
					and_(eax, ecx);
					mov(op_rd, eax);
				}
			}
		}
		else
		{
			cmp(op_rt, 1); // Set carry flag if zero
			cmc();
			if (op_rd.isREG())
			{
				sbb(op_rd, op_rd);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
				and_(op_rd, op_rs);
			}
			else
			{
				sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
				and_(eax, op_rs);
				mov(op_rd, eax);
			}
		}
	}
}

void Jit1_CodeGen::write_PROC_MOVE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero() || rt == rs) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero())
	{
		// This just sets rt to 0.
		// 89 4A EE
		// mov dword [rdx + 0xEE], ecx ; EE = 'rt' offset
		set(op_rt, 0);
	}
	else
	{
		// We are just moving rs to rt
		// 8B 42 DD 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
		std::ignore = mov_ex(op_rt, op_rs, eax);
	}
}

void Jit1_CodeGen::write_PROC_ADDIU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs + immediate
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const int32 immediate = instructions::TinyInt<16>(instruction).sextend<int32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero())
	{
		set(op_rt, immediate);
	}
	else if (rs == rt)
	{
		if (immediate == 0) [[unlikely]]
		{
			// nop
		}
		else if (immediate == 1)
		{
			// FF 42 EE
			// inc dword [rdx + 0xEE]		 ; EE = 'rt' offset
			inc(op_rt);
		}
		else if (immediate == -1)
		{
			// FF 4A EE
			// dec dword [rdx + 0xEE]		 ; EE = 'rt' offset
			dec(op_rt);
		}
		else if (immediate >= 0 && immediate <= 128)
		{
			// using 'sub' for add means we can go to 128
			// 83 6A EE FF
			// sub dword [rdx + 0xEE], 0xFF		 ; EE = 'rt' offset
			sub(op_rt, int8(-immediate));
		}
		else
		{
			// We are just adding immediate to 'rt'.
			// 83 42 EE FF
			// add dword [rdx + 0xEE], 0xFF		 ; EE = 'rt' offset
			// 81 42 EE FF FF FF FF
			// add dword [rdx + 0xEE], 0xFFFFFFFF		 ; EE = 'rt' offset | FFFF = 16-bit immediate value
			add(op_rt, immediate);
		}
	}
	else
	{
		// rt = rs + immediate

		mov(op_rt.if_reg(eax), op_rs);

		if (immediate == 0) [[unlikely]]
		{
			// rt = rs
			// We are just moving rs to rt
			// 8B 42 DD 89 42 EE
			// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
			// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset

			// Add no instructions in this case
		}
		else if (immediate == 1)
		{
			// rt = rs + 1
			// increment
			// 8B 42 DD FF C0 89 42 EE
			// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
			// inc eax
			// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
			inc(op_rt.if_reg(eax));
		}
		else if (immediate == -1)
		{
			// rt = rs + -1
			// decrement
			// 8B 42 DD FF C8 89 42 EE
			// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
			// dec eax
			// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
			dec(op_rt.if_reg(eax));
		}
		else if (immediate < 0)
		{
			add(op_rt.if_reg(eax), immediate);
		}
		else
		{
			sub(op_rt.if_reg(eax), -immediate);
		}

		if (!op_rt.isREG())
		{
			mov(op_rt, eax);
		}
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_ADDI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs + immediate
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const int32 immediate = instructions::TinyInt<16>(instruction).sextend<int32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
		return except_result::none;
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		if (immediate == 0) [[unlikely]]
		{
			// This just sets rt to 0.
			// 89 4A EE
			// mov dword [rdx + 0xEE], ecx ; EE = 'rt' offset
			set(op_rt, 0);
			return except_result::none;
		}
		// // // // // // TODO 128/-128
		else
		{
			// This just sets rt to the immediate value.
			// C7 42 EE FF FF FF FF
			// mov dword [rdx + 0xEE], 0xFFFFFFFF		 ; EE = 'rt' offset | FFFF = 16-bit immediate value
			set(op_rt, immediate);
			return except_result::none;
		}
	}
	else if (rs == rt)
	{
		if (immediate == 0) [[unlikely]]
		{
			// nop
			return except_result::none;
		}
		else
		{
			mov(eax, op_rt);

			if (immediate == 1)
			{
				inc(eax);
			}
			else if (immediate == -1)
			{
				dec(eax);
			}
			else if (immediate < 0)
			{
				add(eax, immediate);
			}
			else
			{
				sub(eax, -immediate);
			}

			jo(intrinsics_.ov, T_NEAR);
			mov(op_rt, eax);
			return except_result::can_throw;
		}
	}
	else if (immediate == 0) [[unlikely]]
	{
		// We are just moving rs to rt
		// 8B 42 DD 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
		std::ignore = mov_ex(op_rt, op_rs, eax);
		return except_result::none;
	}
	else
	{
		mov(eax, op_rs);

		if (immediate == 1)
		{
			// increment
			// 8B 42 DD FF C0 89 42 EE
			// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
			// inc eax
			// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
			inc(eax);
		}
		else if (immediate == -1)
		{
			// decrement
			// 8B 42 DD FF C8 89 42 EE
			// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
			// dec eax
			// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
			dec(eax);
		}
		else if (immediate < 0)
		{
			add(eax, immediate);
		}
		else
		{
			sub(eax, -immediate);
		}

		jo(intrinsics_.ov, T_NEAR);
		mov(op_rt, eax);
		return except_result::can_throw;
	}
}

void Jit1_CodeGen::write_PROC_ADDU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs + rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero() && rd.is_zero()) [[unlikely]]
	{
		// set [rd] to 0.
		set(op_rd, 0);
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// move [rs] to [rd]
		if (rd != rs)
		{
			std::ignore = mov_ex(op_rd, op_rs, eax);
		}
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		// move [rt] to [rd]
		if (rd != rt)
		{
			std::ignore = mov_ex(op_rd, op_rt, eax);
		}
	}
	else if (rs == rd)
	{
		// add [rt] to [rd]
		if (op_rt.isREG() || op_rd.isREG())
		{
			add(op_rd, op_rt);
		}
		else
		{
			mov(eax, op_rt);
			add(op_rd, eax);
		}
	}
	else if (rt == rd)
	{
		// add [rs] to [rd]
		if (op_rs.isREG() || op_rd.isREG())
		{
			add(op_rd, op_rs);
		}
		else
		{
			mov(eax, op_rs);
			add(op_rd, eax);
		}
	}
	else if (op_rd.isREG())
	{
		// add [rs] and [rt] to [rd]
		mov(op_rd, op_rs);
		add(op_rd, op_rt);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(eax, op_rs);
		if (rs == rt) {
			add(eax, eax);
		}
		else {
			add(eax, op_rt);
		}
		mov(op_rd, eax);
	}
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_ADD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs + rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
		return except_result::none;
	}
	else if (rt.is_zero() && rd.is_zero()) [[unlikely]]
	{
		// set [rd] to 0.
		set(op_rd, 0);
		return except_result::none;
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// move [rs] to [rd]
		if (rd != rs)
		{
			std::ignore = mov_ex(op_rd, op_rs, eax);
		}
		return except_result::none;
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		// move [rt] to [rd]
		if (rd != rt)
		{
			std::ignore = mov_ex(op_rd, op_rt, eax);
		}
		return except_result::none;
	}
	else if (rs == rd)
	{
		const Xbyak::Label no_overflow;

		// add [rt] to [rd]
		mov(eax, op_rt);
		add(op_rd, eax);
		jno(no_overflow, T_SHORT);
		sub(op_rd, eax);
		jmp(intrinsics_.ov, T_NEAR);
		L(no_overflow);
		return except_result::can_throw;
	}
	else
	{
		// add [rs] and [rt] to [rd]
		// TODO : determine if adding directly to RD and rolling it back is faster. I doubt it.
		mov(eax, op_rs);
		if (rs == rt) {
			add(eax, eax);
		}
		else {
			add(eax, op_rt);
		}
		jo(intrinsics_.ov, T_NEAR);
		mov(op_rd, eax);
		return except_result::can_throw;
	}
}

void Jit1_CodeGen::write_PROC_AUI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs + simm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	int32 immediate = instructions::TinyInt<32>(instruction << 16).sextend<int32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero())
	{
		set(op_rt, immediate);
	}
	else if (immediate == 0) [[unlikely]]
	{
		// move [rs] to [rt]
		if (rt != rs)
		{
			std::ignore = mov_ex(op_rt, op_rs, eax);
		}
	}
	else if (immediate < 0)
	{
		if (op_rt.isREG())
		{
			mov(op_rt, op_rs);
			add(op_rt, immediate);
		}
		else
		{
			mov(eax, op_rs);
			add(eax, immediate);
			mov(op_rt, eax);
		}
	}
	else
	{
		immediate = -immediate;
		if (op_rt.isREG())
		{
			mov(op_rt, op_rs);
			sub(op_rt, immediate);
		}
		else
		{
			mov(eax, op_rs);
			sub(eax, immediate);
			mov(op_rt, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_SLT(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs < rt

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt == rs) [[unlikely]]
	{
		// rd == x < x
		set(op_rd, 0);
	}
	else
	{
		if (rt.is_zero())
		{
			// rd = rs < 0
			xor_(ecx, ecx);
			cmp_ex(op_rs, 0);
			setl(cl);
			mov(op_rd, ecx);
		}
		else if (rs.is_zero())
		{
			// rd = 0 < rt
			// rd = rt > 0
			xor_(ecx, ecx);
			cmp_ex(op_rt, 0);
			setg(cl);
			mov(op_rd, ecx);
		}
		else
		{
			if (!op_rs.isREG())
			{
				mov(eax, op_rs);
			}
			xor_(ecx, ecx);
			cmp(op_rs.if_reg(eax), op_rt);
			setl(cl);
			mov(op_rd, ecx);
		}
	}
}

void Jit1_CodeGen::write_PROC_SLTU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs < rt

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero() || rt == rs) [[unlikely]]
	{
		// rd = rs < 0
		// rd = x < x
		set(op_rd, 0);
	}
	else
	{
		if (rs.is_zero())
		{
			// rd = 0 < rt
			// rd = rt > 0
			// rd = rt != 0
			xor_(ecx, ecx);
			cmp_ex(op_rt, 0);
			setz(cl);
			mov(op_rd, ecx);
		}
		else
		{
			if (!op_rs.isREG())
			{
				mov(eax, op_rs);
			}
			xor_(ecx, ecx);
			cmp(op_rs.if_reg(eax), op_rt);
			setb(cl);
			mov(op_rd, ecx);
		}
	}
}

void Jit1_CodeGen::write_PROC_SLTI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs < simm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const int32 immediate = instructions::TinyInt<16>(instruction).sextend<int32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		// rt = 0 < 0|1...
		set(op_rt, (immediate > 0) ? 1 : 0);
	}
	else if (immediate == 0)
	{
		if (op_rt.isREG() || rt == rs)
		{
			if (rt != rs)
			{
				mov(op_rt, op_rs);
			}
			shr(op_rt, 31);
		}
		else
		{
			mov(ecx, op_rs);
			shr(ecx, 31);
			mov(op_rt, ecx);
		}
	}
	else if (immediate == 1)
	{
		if (rs == rt && op_rt.isREG())
		{
			auto&& reg_rt = op_rt.as_reg();
			test(reg_rt, reg_rt);
			setle(reg_rt.cvt8());
			movzx(reg_rt, reg_rt.cvt8());
		}
		else if (op_rs.isREG())
		{
			auto&& reg_rs = op_rs.as_reg();

			xor_(ecx, ecx);
			test(reg_rs, reg_rs);
			setle(cl);
			mov(op_rt, ecx);
		}
		else
		{
			mov(eax, op_rs);
			xor_(ecx, ecx);
			test(eax, eax);
			setle(cl);
			mov(op_rt, ecx);
		}
	}
	else
	{
		if (op_rt.isREG() && rt != rs)
		{
			xor_(op_rt, op_rt);
			cmp(op_rs, immediate);
			setl(op_rt);
		}
		else
		{
			xor_(ecx, ecx);
			cmp(op_rs, immediate);
			setl(cl);
			mov(op_rt, ecx);
		}
	}
}

void Jit1_CodeGen::write_PROC_SLTIU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs < zimm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const uint32 immediate = uint32(instructions::TinyInt<16>(instruction).sextend<int32>()); // intended

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (immediate == 0) [[unlikely]]
	{
		// rt = rs < 0
		set(op_rt, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		// rt = 0 < imm
		// imm > 0
		set(op_rt, (immediate > 0) ? 1 : 0);
	}
	else if (immediate == 1)
	{
		if (rs == rt && op_rt.isREG())
		{
			auto&& reg_rt = op_rt.as_reg();
			test(reg_rt, reg_rt);
			setz(reg_rt.cvt8());
			movzx(reg_rt, reg_rt.cvt8());
		}
		else if (op_rs.isREG())
		{
			auto&& reg_rs = op_rs.as_reg();

			xor_(ecx, ecx);
			test(reg_rs, reg_rs);
			setz(cl);
			mov(op_rt, ecx);
		}
		else
		{
			mov(eax, op_rs);
			xor_(ecx, ecx);
			test(eax, eax);
			setz(cl);
			mov(op_rt, ecx);
		}
	}
	else
	{
		if (op_rt.isREG() && rt != rs)
		{
			xor_(op_rt, op_rt);
			cmp(op_rs, immediate);
			setb(op_rt);
		}
		else
		{
			xor_(ecx, ecx);
			cmp(op_rs, immediate);
			setb(cl);
			mov(op_rt, ecx);
		}
	}
}

void Jit1_CodeGen::write_COP1_MFC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const instructions::FPRegister<11, 5> fs(instruction, jit_.processor_.get_fpu_coprocessor());
	const int16 fs_offset = fs.get_offset();

	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
	}
	else
	{
		std::ignore = mov_ex(op_rt, dword[r12 + fs_offset], eax);
	}
}

void Jit1_CodeGen::write_COP1_MTC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const instructions::FPRegister<11, 5> fs(instruction, jit_.processor_.get_fpu_coprocessor());
	const int16 fs_offset = fs.get_offset();

	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero())
	{
		// write 0 to [fs]
		set(dword[r12 + fs_offset], 0);
	}
	else
	{
		std::ignore = mov_ex(dword[r12 + fs_offset], op_rt, eax);
	}
}

void Jit1_CodeGen::write_COP1_MFHC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const instructions::FPRegister<11, 5> fs(instruction, jit_.processor_.get_fpu_coprocessor());
	const int16 fs_offset = fs.get_offset();

	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
	}
	else
	{
		std::ignore = mov_ex(op_rt, dword[r12 + fs_offset + 4], eax);
	}
}

void Jit1_CodeGen::write_COP1_MTHC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const instructions::FPRegister<11, 5> fs(instruction, jit_.processor_.get_fpu_coprocessor());
	const int16 fs_offset = fs.get_offset();

	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero())
	{
		// write 0 to [fs]
		set(dword[r12 + (fs_offset + 4)], 0);
	}
	else
	{
		std::ignore = mov_ex(dword[r12 + fs_offset + 4], op_rt, eax);
	}
}

void Jit1_CodeGen::write_COP1_SEL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	auto* coprocessor = jit_.processor_.get_fpu_coprocessor();

	const instructions::FPRegister<16, 5> ft{instruction, coprocessor};
	const instructions::FPRegister<11, 5> fs{instruction, coprocessor};
	const instructions::FPRegister<6, 5> fd{instruction, coprocessor};

	const int16 ft_offset = ft.get_offset();
	const int16 fs_offset = fs.get_offset();
	const int16 fd_offset = fd.get_offset();

	if (instruction_info.Type == mips::instructions::instruction_type::single_fp)
	{
		// float
		// fd = fd.bit[0] ? ft : fs
		mov(eax, dword[r12 + fd_offset]);
		xor_(ecx, ecx);
		and_(eax, 1);
		sub(ecx, eax);
		dec(eax);
		and_(ecx, dword[r12 + ft_offset]);
		and_(eax, dword[r12 + fs_offset]);
		or_(ecx, eax);
		mov(dword[r12 + fd_offset], ecx);
	}
	else
	{
		// double
		// fd = fd.bit[0] ? ft : fs
		mov(eax, dword[r12 + fd_offset]);
		xor_(ecx, ecx);
		and_(eax, 1);
		sub(rcx, rax);
		dec(rax);
		and_(rcx, qword[r12 + ft_offset]);
		and_(rax, qword[r12 + fs_offset]);
		or_(rcx, rax);
		mov(qword[r12 + fd_offset], rcx);
	}
}

void Jit1_CodeGen::write_PROC_MUL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero() || rs.is_zero()) [[unlikely]]
	{
		// set [rd] to 0.
		set(op_rd, 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		if (eax != op_rt)
		{
			mov(eax, op_rt);
		}
		imul(op_rd);
		mov(op_rd, eax);
	}
	else if (rt == rd)
	{
		// move [rt] to [rd]
		if (eax != op_rs)
		{
			mov(eax, op_rs);
		}
		imul(op_rd);
		mov(op_rd, eax);
	}
	else
	{
		// mul [rs] and [rt] to [rd]
		mov(eax, op_rs);
		if (rs == rt) {
			imul(eax);
		}
		else {
			imul(op_rt);
		}
		mov(op_rd, eax);
	}
}

void Jit1_CodeGen::write_PROC_MULU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero() || rs.is_zero()) [[unlikely]]
	{
		// set [rd] to 0.
		set(op_rd, 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		if (eax != op_rt)
		{
			mov(eax, op_rt);
		}
		mul(op_rd);
		mov(op_rd, eax);
	}
	else if (rt == rd)
	{
		// move [rt] to [rd]
		if (eax != op_rs)
		{
			mov(eax, op_rs);
		}
		mul(op_rd);
		mov(op_rd, eax);
	}
	else
	{
		// mul [rs] and [rt] to [rd]
		mov(eax, op_rs);
		if (rs == rt) {
			mul(eax);
		}
		else {
			mul(op_rt);
		}
		mov(op_rd, eax);
	}
}

void Jit1_CodeGen::write_PROC_MUH(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero() || rs.is_zero()) [[unlikely]]
	{
		// set [rd] to 0.
		set(op_rd, 0);
	}
	else if (rs == rd) // TODO: identical to `else`
	{
		// move [rt] to [rd]
		if (eax != op_rt)
		{
			mov(eax, op_rt);
		}
		imul(op_rd);
		mov(op_rd, edx);
	}
	else if (rt == rd) // TODO: identical to `else`
	{
		// move [rt] to [rd]
		if (eax != op_rs)
		{
			mov(eax, op_rs);
		}
		imul(op_rd);
		mov(op_rd, edx);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(eax, op_rs);
		if (rs == rt) {
			imul(eax);
		}
		else {
			imul(op_rt);
		}
		mov(op_rd, edx);
	}
}

void Jit1_CodeGen::write_PROC_MUHU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero() || rs.is_zero()) [[unlikely]]
	{
		// set [rd] to 0.
		set(op_rd, 0);
	}
	else if (rs == rd) // TODO: identical to `else`
	{
		// move [rt] to [rd]
		if (eax != op_rt)
		{
			mov(eax, op_rt);
		}
		mul(op_rd);
		mov(op_rd, edx);
	}
	else if (rt == rd) // TODO: identical to `else`
	{
		// move [rt] to [rd]
		if (eax != op_rs)
		{
			mov(eax, op_rs);
		}
		mul(op_rd);
		mov(op_rd, edx);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(eax, op_rs);
		if (rs == rt) {
			mul(eax);
		}
		else {
			mul(op_rt);
		}
		mov(op_rd, edx);
	}
}

// TODO consider scanning forward for further DIV/MOD instructions using these parameters.
// Values might be able to be cached.

void Jit1_CodeGen::write_PROC_DIV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs / rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	const Xbyak::Label divzero;
	// [rd] = [rs] / [rt]

	if (rd.is_zero()) [[unlikely]]
	{
		// TODO should we throw an exception?
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// divzero
		// set [rd] to 0.
		// TODO should we throw an exception?
		set(op_rd, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(ecx, op_rt);
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, op_rd);
		xor_(edx, edx);
		idiv(ecx);
		L(divzero);
		mov(op_rd, eax);
	}
	else if (rs == rt) [[unlikely]]
	{
		// TODO should we throw an exception?
		set(op_rd, 1);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(ecx, op_rt);
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, op_rs);
		xor_(edx, edx);
		idiv(ecx);
		L(divzero);
		mov(op_rd, eax);
	}
}

void Jit1_CodeGen::write_PROC_DIVU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs / rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	const Xbyak::Label divzero;
	// [rd] = [rs] / [rt]

	if (rd.is_zero()) [[unlikely]]
	{
		// TODO should we throw an exception?
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// divzero
		// set [rd] to 0.
		// TODO should we throw an exception?
		set(op_rd, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rs == rd)
	{
		mov(ecx, op_rt);
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, op_rd);
		xor_(edx, edx);
		div(ecx);
		L(divzero);
		mov(op_rd, eax);
	}
	else if (rs == rt) [[unlikely]]
	{
		// TODO should we throw an exception?
		set(op_rd, 1);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(ecx, op_rt);
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, op_rs);
		xor_(edx, edx);
		div(ecx);
		L(divzero);
		mov(op_rd, eax);
	}
}

void Jit1_CodeGen::write_PROC_MOD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs % rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	const Xbyak::Label divzero;
	// [rd] = [rs] / [rt]

	if (rd.is_zero()) [[unlikely]]
	{
		// TODO should we throw an exception?
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// divzero
		// set [rd] to 0.
		// TODO should we throw an exception?
		set(op_rd, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(ecx, op_rt);
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, op_rd);
		xor_(edx, edx);
		idiv(ecx);
		L(divzero);
		mov(op_rd, eax);
	}
	else if (rs == rt) [[unlikely]]
	{
		// TODO should we throw an exception?
		set(op_rd, 0);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(ecx, op_rt);
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, op_rs);
		xor_(edx, edx);
		idiv(ecx);
		L(divzero);
		mov(op_rd, eax);
	}
}

void Jit1_CodeGen::write_PROC_MODU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	const Xbyak::Label divzero;
	// [rd] = [rs] / [rt]

	if (rd.is_zero()) [[unlikely]]
	{
		// TODO should we throw an exception?
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// divzero
		// set [rd] to 0.
		// TODO should we throw an exception?
		set(op_rd, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(ecx, op_rt);
		xor_(edx, edx);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, op_rd);
		xor_(edx, edx);
		div(ecx);
		L(divzero);
		mov(op_rd, edx);
	}
	else if (rs == rt) [[unlikely]]
	{
		// TODO should we throw an exception?
		set(op_rd, 0);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(ecx, op_rt);
		xor_(edx, edx);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, op_rs);
		xor_(edx, edx);
		div(ecx);
		L(divzero);
		mov(op_rd, edx);
	}
}

void Jit1_CodeGen::write_PROC_XOR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs ^ rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs == rt) [[unlikely]]
	{
		// set rd to 0
		// 89 4A EE
		// mov dword [rdx + 0xEE], ecx ; EE = 'rd' offset
		set(op_rd, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		if (rd != rt)
		{
			std::ignore = mov_ex(op_rd, op_rt, eax);
		}
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		if (rd != rs)
		{
			std::ignore = mov_ex(op_rd, op_rs, eax);
		}
	}
	else
	{
		if (rd == rt) {
			if (op_rs.isREG() || op_rd.isREG())
			{
				xor_(op_rd, op_rs);
			}
			else
			{
				mov(eax, op_rs);
				xor_(op_rd, eax);
			}
		}
		else if (rd == rs) {
			if (op_rs.isREG() || op_rd.isREG())
			{
				xor_(op_rd, op_rt);
			}
			else
			{
				mov(eax, op_rt);
				xor_(op_rd, eax);
			}
		}
		else {
			if (op_rd.isREG())
			{
				mov(op_rd, op_rt);
				xor_(op_rd, op_rs);
			}
			else
			{
				mov(eax, op_rt);
				xor_(eax, op_rs);
				mov(op_rd, eax);
			}
		}
	}
}

void Jit1_CodeGen::write_PROC_XORI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs | zimm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const uint32 immediate = instructions::TinyInt<16>(instruction).zextend<uint32>();

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	if (rt.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		// set rt to offset
		set(op_rt, immediate);
	}
	else if (immediate == 0) [[unlikely]]
	{
		if (rs == rt) {
			//nop
		}
		else
		{
			// just move rs to rt
			std::ignore = mov_ex(op_rt, op_rs, eax);
		}
	}
	else
	{
		// actually perform OR
		if (rt == rs) {
			xor_(op_rt, immediate);
		}
		else if (op_rt.isREG())
		{
			mov(op_rt, op_rs);
			xor_(op_rt, immediate);
		}
		else
		{
			mov(eax, op_rs);
			xor_(eax, immediate);
			mov(op_rt, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_SEB(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = sbyte(rt)
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rt = get_register_op8(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else
	{
		if (op_rd.isREG())
		{
			movsx(op_rd.as_reg(), op_rt);
		}
		else
		{
			movsx(eax, op_rt);
			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_SEH(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = sword(rt)
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rt = get_register_op16(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else
	{
		if (op_rd.isREG())
		{
			movsx(op_rd.as_reg(), op_rt);
		}
		else
		{
			movsx(eax, op_rt);
			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_EHB(jit1::ChunkOffset & __restrict chunk_offset, const uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt <<< sa but hazard barrier instead

	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const int8 sa = instructions::TinyInt<5>(instruction >> 6).zextend<int8>();

	xassert(rt.is_zero() && rd.is_zero() && sa == 0b00011);

	if (jit_.processor_.handles_execution_hazards())
	{
		intrinsic_clear_execution_hazards(address);
	}
}

void Jit1_CodeGen::write_PROC_SLL(jit1::ChunkOffset & __restrict chunk_offset, const uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt <<< sa

	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const int8 sa = instructions::TinyInt<5>(instruction >> 6).zextend<int8>();

	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	// EHB should be handled by PROC_EHB
	xassert(!(rt.is_zero() && rd.is_zero() && sa == 0b00011));

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rd == rt)
	{
		if (sa == 0) [[unlikely]]
		{
			// nop
		}
		else
		{
			shl(op_rd, sa);
		}
	}
	else
	{
		auto&& tmp = op_rd.if_reg(eax);

		mov(tmp, op_rt);

		if (sa == 0) [[unlikely]]
		{
			// move
			// TODO : global func
		}
		else
		{
			shl(tmp, sa);
		}

		if (!op_rd.isREG())
		{
			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_SRL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt >>> sa

	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const int8 sa = instructions::TinyInt<5>(instruction >> 6).zextend<int8>();

	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rd == rt)
	{
		if (sa == 0) [[unlikely]]
		{
			// nop
		}
		else
		{
			shr(op_rd, sa);
		}
	}
	else
	{
		auto&& tmp = op_rd.if_reg(eax);

		mov(tmp, op_rt);

		if (sa == 0) [[unlikely]]
		{
			// move
		}
		else
		{
			shr(tmp, sa);
		}

		if (!op_rd.isREG())
		{
			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_SRA(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt >> sa

	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const int8 sa = instructions::TinyInt<5>(instruction >> 6).zextend<int8>();

	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rd == rt)
	{
		if (sa == 0) [[unlikely]]
		{
			// nop
		}
		else
		{
			sar(op_rd, sa);
		}
	}
	else
	{
		auto&& tmp = op_rd.if_reg(eax);

		mov(tmp, op_rt);

		if (sa == 0) [[unlikely]]
		{
			// move
		}
		else
		{
			sar(tmp, sa);
		}

		if (!op_rd.isREG())
		{
			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_SLLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt <<< rs

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// set rd to 0.
		set(op_rd, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		if (rd == rt) [[unlikely]]
		{
			// nop
		}
		else
		{
			// move rt to rd
			std::ignore = mov_ex(op_rd, op_rt, eax);
		}
	}
	else if (rs == rt)
	{
		if (rd == rs)
		{
			// rd == rs == rt
			mov(ecx, op_rd);
			and_(ecx, int32(instructions::Bits(5)));
			shl(op_rd, cl);
		}
		else
		{
			if (op_rd.isREG())
			{
				mov(ecx, op_rs);
				mov(op_rd, op_rs.if_reg(ecx));
				and_(ecx, int32(instructions::Bits(5)));
				shl(op_rd, cl);
			}
			else
			{
				mov(ecx, op_rs);
				mov(eax, op_rs.if_reg(ecx));
				and_(ecx, int32(instructions::Bits(5)));
				shl(eax, cl);
				mov(op_rd, eax);
			}
		}
	}
	else
	{
		if (rd == rt)
		{
			mov(ecx, op_rs);
			and_(ecx, int32(instructions::Bits(5)));
			shl(op_rd, cl);
		}
		else if (rs == rt)
		{
			if (op_rd.isREG())
			{
				mov(ecx, op_rs);
				mov(op_rd, op_rs.if_reg(ecx));
				and_(ecx, int32(instructions::Bits(5)));
				shl(op_rd, cl);
			}
			else
			{
				mov(ecx, op_rs);
				mov(eax, ecx);
				and_(ecx, int32(instructions::Bits(5)));
				shl(eax, cl);
				mov(op_rd, eax);
			}
		}
		else
		{
			mov(ecx, op_rs);
			if (op_rd.isREG())
			{
				mov(op_rd, op_rs.if_reg(ecx));
			}
			else
			{
				mov(eax, op_rt);
			}
			and_(ecx, int32(instructions::Bits(5)));
			if (op_rd.isREG())
			{
				shl(op_rd, cl);
			}
			else
			{
				shl(eax, cl);
				mov(op_rd, eax);
			}
		}
	}
}

void Jit1_CodeGen::write_PROC_SRLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt >>> rs
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// set rd to 0.
		set(op_rd, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		if (rd != rt)
		{

			// move rt to rd
			std::ignore = mov_ex(op_rd, op_rt, eax);
		}
	}
	else if (rs == rt)
	{
		mov(ecx, op_rs);
		mov(eax, ecx);
		and_(ecx, int32(instructions::Bits(5)));
		shr(eax, cl);
		mov(op_rd, eax);
	}
	else
	{
		mov(ecx, op_rs);
		mov(eax, op_rt);
		and_(ecx, int32(instructions::Bits(5)));
		shr(eax, cl);
		mov(op_rd, eax);
	}
}

void Jit1_CodeGen::write_PROC_SYNC(jit1::ChunkOffset & __restrict chunk_offset, const uint32 address, const instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const uint8 stype = instructions::TinyInt<5>(instruction >> 6).zextend<uint8>();

	// Don't defer to an expensive procedure if the interpreter doesn't handle this hazard to begin with.
	if (
		const mips::memory_hazards hazards = instructions::parse_sync_type(stype);
		!jit_.processor_.handles_memory_hazards(hazards)
	)
	{
		return;
	}

	const auto& this_processor = jit_.processor_;

	if (this_processor.collect_stats_)
	{
		// dispatch a stat call.
		set(rcx, intptr(instruction_info.Name));
		call(intrinsics_.emulated_stats);
	}

	// Defer to the emulator for this. In the end, we're going to call into host code anyway, so might as well let the emulator do it.
	insert_procedure(address, std::bit_cast<const void*>(instruction_info.Proc), instruction);
	// `SYNC` does not throw.
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_SYNCI(jit1::ChunkOffset & __restrict chunk_offset, const uint32 address, const instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<21, 5> base(instruction, jit_.processor_);
	const int32 offset = instructions::TinyInt<16>(instruction).sextend<int32>();

	if (base.is_zero())
	{
		insert_procedure(address, std::bit_cast<const void*>(&SyncInstruction), offset);
	}
	else
	{
		auto&& op_base = get_register_op32(base);

		if (offset != 0)
		{
			auto&& tmp = is_same(op_base, eax) ? ecx : eax;
			set(tmp, offset);
			add(tmp, op_base);
			insert_procedure(address, std::bit_cast<const void*>(&SyncInstruction), tmp);
		}
		else
		{
			insert_procedure(address, std::bit_cast<const void*>(&SyncInstruction), op_base);
		}
	}

	return except_result::none; // TODO : for now
}

Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_RDHWR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const uint32 selector = instructions::TinyInt<3>(instruction >> 6).zextend<uint32>();
	const uint32 reg_number = rd.get_register();

	auto&& op_rt = get_register_op32(rt);

	if (selector == 0) [[likely]] {
		switch (reg_number) {
		case 29:
			std::ignore = mov_ex(op_rt, dword[rbp + offsets.user_value], eax);
			return except_result::none;
		case 1:
			set(op_rt, 0x100);
			return except_result::none;
		default: [[unlikely]]
			break;
		}
	}

	mov(ecx, address);
	jmp(intrinsics_.ri, T_NEAR);
	return except_result::always_throw;
}

void Jit1_CodeGen::write_PROC_EXT(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info)
{
	// rt <= extract([rs, msbd, lsb])
	// msbd = size - 1
	// lsb = pos
	// TODO : BMI1

	// rt = rs[msbd+lsb...lsb]
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	const uint16 msbd = instructions::TinyInt<5>(instruction >> 11).zextend<uint16>();
	const uint16 lsb =  instructions::TinyInt<5>(instruction >> 6).zextend<uint16>();

	if (rt.is_zero()) [[unlikely]] {
		// nop
	}
	else {
		if (lsb + msbd > 31) [[unlikely]] {
			// Result is unpredictable, just push -1.
			set(op_rt, -1);
		}
		else if (rs.is_zero()) [[unlikely]] {
			// The operation would just return 0.
			set(op_rt, 0);
		}
		else {
			const uint32_t mask = ((1U << (msbd + 1)) - 1) & (std::numeric_limits<uint32>::max() >> lsb);

			if (platform::get_host_features().bmi1 && lsb > 0 && mask != 0)
			{
				struct bextr_operand final
				{
					uint16 start : 8;
					uint16 length : 8;
				};

				const bextr_operand operand = {
					.start = lsb,
					.length = static_cast<uint16>(msbd + 1U)
				};

				// TODO : TBM has immediate form, but xbyak doesn't appear to support it.
				mov(eax, std::bit_cast<uint16>(operand));
				bextr(eax, op_rs, eax);
				mov(op_rt, eax);
			}
			else
			{
				// Equivalent logic to ProcInstructionDef::EXT
				xassert(mask != 0 || (lsb == 0 && msbd == 31));
				if (mask == 0) [[unlikely]] {
					set(op_rt, 0);
				}
				else {
					mov(eax, op_rs);
					if (lsb > 0) {
						shr(eax, lsb);
					}
					and_(eax, mask);
					mov(op_rt, eax);
				}
			}
		}
	}
}

// TODO : test me
void Jit1_CodeGen::write_PROC_INS(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info)
{
	// TODO : BMI1

	// rt = rt[31..msb+1] || rs[msb-lsb..0] || rt[lsb-1..0]]
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);

	const uint32 msb = instructions::TinyInt<5>(instruction >> 11).zextend<uint32>();
	const uint32 lsb = instructions::TinyInt<5>(instruction >> 6).zextend<uint32>();

	if (rt.is_zero()) [[unlikely]] {
		// nop
	}
	else if (lsb > msb) [[unlikely]] {
		// Result is unpredictable, just push -1.
		set(op_rt, -1);
	}
	else [[likely]] {
		const uint32 size = msb - lsb + 1;
		const uint32 size_mask = mips::make_bitmask<uint32>(size);
		const uint32 inverse_mask = ~(size_mask << lsb);

		xassert(size > 0U);      // it should be impossible for size to be zero
		xassert(size_mask != 0); // likewise, the size_mask should not be able to be zero
		// inverse_mask can only be zero when lsb=0 and msb=31. In this situation, and only in this situation, size_mask can be ~0.
		xassert(inverse_mask != 0 || (size_mask == std::numeric_limits<uint32>::max() && lsb == 0 && msb == 31));
		xassert(size_mask != std::numeric_limits<uint32>::max() || inverse_mask == 0);
		xassert(inverse_mask != std::numeric_limits<uint32>::max());

		if (rs.is_zero()) {
			// rs == 0
			// size = msb - lsb + 1
			// tmp0 = rt & ~(mask(size) << lsb)
			// rt = tmp0

			if (inverse_mask == 0) [[unlikely]]
			{
				set(op_rt, 0);
			}
			else
			{
				and_(op_rt, inverse_mask);
			}
		}
		else if (rs == rt)
		{
			if (lsb == 0) [[unlikely]]
			{
				xassert(inverse_mask == std::numeric_limits<uint32>::max());
				// nop, do nothing
			}
			else
			{
				// size = msb - lsb + 1
				// mask = mask(size) | ~(mask(size) << lsb)
				// rt = rt & mask

				const uint32 mask = size_mask | inverse_mask;
				xassert(mask != 0);
				xassert(mask != std::numeric_limits<uint32>::max()); // this is only possible if lsb=0, in which case it's always true
				and_(op_rt, mask);
			}
		}
		else if (inverse_mask == 0) [[unlikely]]
		{
			xassert(lsb == 0 && msb == 31);
			xassert(size_mask == std::numeric_limits<uint32>::max());
			// input from [rt] will be zero
			// input from [rt] will be total, unshifted
			// ergo, this becomes a no-op.
		}
		else {
			// Equivalent logic to ProcInstructionDef::INS

			// size = msb - lsb + 1
			// eval = rs & mask(size)
			// tmp0 = rt & ~(mask(size) << lsb)
			// rt = tmp0 | (eval << lsb)

			// LSB == 0
			// size = msb + 1
			// eval = rs & mask(size)
			// tmp0 = rt & ~mask(size)
			// rt = tmp0 | eval

			// MSB == 0, LSB == 0
			// size = 1
			// eval = rs & 1
			// tmp0 = rt & ~1
			// rt = tmp0 | eval

			// rs == rt
			// size = msb - lsb + 1
			// eval = r & mask(size)
			// tmp0 = r & ~(mask(size) << lsb)
			// r = tmp0 | (eval << lsb)

			// if lsb == 0, then inverse_mask is 0 and mask is ~0, and other branches take precedence.
			/// xassert(lsb != 0);
			xassert(size_mask != std::numeric_limits<uint32>::max());

			// TODO : this can be one fewer instruction depending on the various values, as you can shift the masks and then use `lea`.
			mov(eax, op_rs);
			mov(edx, op_rt);
			and_(eax, size_mask);
			and_(edx, inverse_mask);
			if (lsb != 0)
			{
				shl(eax, static_cast<int>(lsb));
			}
			or_(edx, eax);
			mov(op_rt, edx);
		}
	}
}

void Jit1_CodeGen::write_PROC_LSA(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info)
{
	// rd = signed( ([rs] << (sa + 1)) + [rt] )

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const uint32 sa = instructions::TinyInt<2>(instruction >> 6).zextend<uint32>() + 1U;

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rt = get_register_op32(rt);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero() && rt.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		if (rd != rt)
		{
			std::ignore = mov_ex(op_rd, op_rt, eax);
		}
	}
	else if (rs == rd)
	{
		if (rt.is_zero())
		{
			// rd = signed( ([rd] << (sa + 1)) )
			// TODO : is SAL correct?
			sal(op_rd, sa);
		}
		else
		{
			// signed( rs <<= (sa + 1); rs += rt; )
			sal(op_rs, sa);
			add(op_rs, op_rt);
		}
	}
	else
	{
		mov(eax, op_rs);
		// TODO : is SAL correct?
		sal(eax, sa);
		// TODO : sign extension
		if (rt == rd)
		{
			add(op_rd, eax);
		}
		else
		{
			if (!rt.is_zero()) [[likely]]
			{
				add(eax, op_rt);
			}

			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_CLZ(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info)
{
	// rd <= count leading zeroes in rs

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(op_rd, 32);
	}
	else if (rs == rd && op_rs.isREG())
	{
		if (platform::get_host_features().bmi1)
		{
			tzcnt(op_rd.as_reg(), op_rd);
		}
		else
		{
			auto&& tmp = is_same(op_rd, eax) ? edx : eax;
			mov(tmp, 32);
			if (!is_same(op_rd, ecx))
			{
				mov(ecx, op_rd);
			}
			rep(); bsf(eax, op_rd);
			if (!is_same(op_rd, eax))
			{
				mov(op_rd, eax);
			}
		}
	}
	else
	{
		if (platform::get_host_features().bmi1)
		{
			tzcnt(ecx, op_rs);
			mov(op_rd, ecx);
		}
		else
		{
			if (!op_rs.isREG() || !is_same(op_rs, ecx))
			{
				mov(ecx, op_rs);
			}
			mov(eax, 32);
			rep(); bsf(eax, ecx);
			mov(op_rd, eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_CLO(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info)
{
	// rd <= count leading ones in rs

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	auto&& op_rs = get_register_op32(rs);
	auto&& op_rd = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(op_rd, 0);
	}
	else if (rs == rd && op_rs.isREG())
	{
		not_(op_rd);
		if (platform::get_host_features().bmi1)
		{
			tzcnt(op_rd.as_reg(), op_rd);
		}
		else
		{
			auto&& tmp = is_same(op_rd, eax) ? edx : eax;
			mov(tmp, 32);
			if (!is_same(op_rd, ecx))
			{
				mov(ecx, op_rd);
			}
			{
				rep(); bsf(eax, op_rd);
				if (!is_same(op_rd, eax))
				{
					mov(op_rd, eax);
				}
			}
		}
	}
	else
	{
		mov(ecx, op_rs);
		not_(ecx);

		if (platform::get_host_features().bmi1)
		{
			tzcnt(ecx, ecx);
			mov(op_rd, ecx);
		}
		else
		{
			mov(eax, 32);
			rep(); bsf(eax, ecx);
			mov(op_rd, eax);
		}
	}
}
