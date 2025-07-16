#include "pch.hpp"

#include "jit1.hpp"
#include "../../processor.hpp"
#include "coprocessor/coprocessor1/coprocessor1.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "instructions/coprocessor1_support.hpp"
#include <cassert>
#include "codegen.hpp"

using namespace mips;

void Jit1_CodeGen::insert_procedure_ecx(uint32 address, uint64 procedure, uint32 _ecx, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	static const int8 flags_offset = value_assert<int8>(offsetof(processor, flags_) - 128);
	static const int8 pc_offset =  value_assert<int8>(offsetof(processor, program_counter_) - 128);

	if (procedure <= 0xFFFFFFFF)
	{
		set(eax, uint32(procedure));
	}
	else
	{
		set(rax, uint64(procedure));
	}

	mov(dword[rbp + pc_offset], int32(address));
	mov(dword[rbp + flags_offset], ebx);
	set(ecx, _ecx);
	lea(rdx, qword[rbp - 128]);
	call(rax);
	//mov(dword[rbp + instructions::GPRegister<>{0}.get_offset(true)], 0); //clear register
	mov(ebx, dword[rbp + flags_offset]);
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
		std::ignore = mov_ex(op_rd, op_rt, eax);
	}
	else if (rt.is_zero())
	{
		// just move rs to rd
		// 8B 42 DD 89 42 EE 
		// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		std::ignore = mov_ex(op_rd, op_rs, eax);
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
		std::ignore = mov_ex(op_rd, op_rt, eax);
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
		else if (immediate >= 0 && immediate <= 128)
		{
			// using 'sub' for add means we can go to 128
			// 8B 42 DD 83 E8 79 89 42 EE 
			// mov dword eax, [rdx + 0xDD]		  ; DD = 'rs' offset
			// sub dword eax, 0x79					 ; 79 = immediate
			// mov dword [rdx + 0xEE], eax		  ; EE = 'rt' offset
			sub(op_rt.if_reg(eax), int8(-immediate));
		}
		else
		{
			// this is a legitimate addiu
			// using 'add' with sub means we can go to -128
			// 8B 42 DD 83 C0 79 89 42 EE 
			// mov dword eax, [rdx + 0xDD]		  ; DD = 'rs' offset
			// add dword eax, 0x79					 ; 79 = immediate
			// mov dword [rdx + 0xEE], eax		  ; EE = 'rt' offset
			// 8B 42 DD 05 FF FF FF FF 89 42 EE 
			// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
			// add dword eax, 0xFFFFFFFF		 ; FFFF = 16-bit immediate value
			// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
			add(op_rt.if_reg(eax), immediate);
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
			else if (immediate >= 0 && immediate <= 128)
			{
				// using 'sub' for add means we can go to 128
				// 83 6A EE FF 
				// sub dword [rdx + 0xEE], 0xFF		 ; EE = 'rt' offset
				sub(eax, int8(-immediate));
			}
			else
			{
				// using 'add' with sub means we can go to -128
				// 83 42 EE FF
				// add dword [rdx + 0xEE], 0xFF		 ; EE = 'rt' offset
				// We are just adding immediate to 'rt'.
				// 81 42 EE FF FF FF FF 
				// add dword [rdx + 0xEE], 0xFFFFFFFF		 ; EE = 'rt' offset | FFFF = 16-bit immediate value
				add(eax, immediate);
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
		else if (immediate >= 0 && immediate <= 128)
		{
			// using 'sub' for add means we can go to 128
			// 8B 42 DD 83 E8 79 89 42 EE 
			// mov dword eax, [rdx + 0xDD]		  ; DD = 'rs' offset
			// sub dword eax, 0x79					 ; 79 = immediate
			// mov dword [rdx + 0xEE], eax		  ; EE = 'rt' offset
			sub(eax, int8(-immediate));
		}
		else
		{
			// this is a legitimate addiu
			// using 'add' with sub means we can go to -128
			// 8B 42 DD 83 C0 79 89 42 EE 
			// mov dword eax, [rdx + 0xDD]		  ; DD = 'rs' offset
			// add dword eax, 0x79					 ; 79 = immediate
			// mov dword [rdx + 0xEE], eax		  ; EE = 'rt' offset
			// 8B 42 DD 05 FF FF FF FF 89 42 EE 
			// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
			// add dword eax, 0xFFFFFFFF		 ; FFFF = 16-bit immediate value
			// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
			add(eax, immediate);
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
		std::ignore = mov_ex(op_rd, op_rs, eax);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		// move [rt] to [rd]
		std::ignore = mov_ex(op_rd, op_rt, eax);
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
		std::ignore = mov_ex(op_rd, op_rs, eax);
		return except_result::none;
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		// move [rt] to [rd]
		std::ignore = mov_ex(op_rd, op_rt, eax);
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
		std::ignore = mov_ex(op_rt, op_rs, eax);
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
		set(get_register_op32(rd), 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(get_register_op32(rd), 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(ecx, get_register_op32(rt));
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, get_register_op32(rd));
		xor_(edx, edx);
		idiv(ecx);
		L(divzero);
		mov(get_register_op32(rd), eax);
	}
	else if (rs == rt) [[unlikely]]
	{
		// move [rt] to [rd]
		cmp(get_register_op32(rt), 0);
		je(divzero);
		set(get_register_op32(rd), 1);
		// TODO should we throw an exception?
		L(divzero);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(ecx, get_register_op32(rt));
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, get_register_op32(rs));
		xor_(edx, edx);
		idiv(ecx);
		L(divzero);
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_DIVU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs/* rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

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
		set(get_register_op32(rd), 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(get_register_op32(rd), 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(ecx, get_register_op32(rt));
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, get_register_op32(rd));
		xor_(edx, edx);
		div(ecx);
		L(divzero);
		mov(get_register_op32(rd), eax);
	}
	else if (rs == rt) [[unlikely]]
	{
		// move [rt] to [rd]
		cmp(get_register_op32(rt), 0);
		je(divzero);
		set(get_register_op32(rd), 1);
		// TODO should we throw an exception?
		L(divzero);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(ecx, get_register_op32(rt));
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, get_register_op32(rs));
		xor_(edx, edx);
		div(ecx);
		L(divzero);
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_MOD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

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
		set(get_register_op32(rd), 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(get_register_op32(rd), 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(ecx, get_register_op32(rt));
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, get_register_op32(rd));
		xor_(edx, edx);
		idiv(ecx);
		L(divzero);
		mov(get_register_op32(rd), eax);
	}
	else if (rs == rt) [[unlikely]]
	{
		// move [rt] to [rd]
		cmp(get_register_op32(rt), 0);
		je(divzero);
		set(get_register_op32(rd), 0);
		// TODO should we throw an exception?
		L(divzero);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(ecx, get_register_op32(rt));
		xor_(eax, eax);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, get_register_op32(rs));
		xor_(edx, edx);
		idiv(ecx);
		L(divzero);
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_MODU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

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
		set(get_register_op32(rd), 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		set(get_register_op32(rd), 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(ecx, get_register_op32(rt));
		xor_(edx, edx);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, get_register_op32(rd));
		xor_(edx, edx);
		div(ecx);
		L(divzero);
		mov(get_register_op32(rd), edx);
	}
	else if (rs == rt) [[unlikely]]
	{
		// move [rt] to [rd]
		cmp(get_register_op32(rt), 0);
		je(divzero);
		set(get_register_op32(rd), 0);
		// TODO should we throw an exception?
		L(divzero);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(ecx, get_register_op32(rt));
		xor_(edx, edx);
		test(ecx, ecx);
		jz(divzero);
		mov(eax, get_register_op32(rs));
		xor_(edx, edx);
		div(ecx);
		L(divzero);
		mov(get_register_op32(rd), edx);
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
		std::ignore = mov_ex(op_rd, op_rt, eax);
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		std::ignore = mov_ex(op_rd, op_rs, eax);
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

void Jit1_CodeGen::write_PROC_SLL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt <<< sa

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
		// move rt to rd
		std::ignore = mov_ex(op_rd, op_rt, eax);
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

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rt.is_zero()) [[unlikely]]
	{
		// set rd to 0.
		set(get_register_op32(rd), 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		// move rt to rd
		std::ignore = mov_ex(get_register_op32(rd), get_register_op32(rt), eax);
	}
	else if (rs == rt)
	{
		mov(ecx, get_register_op32(rs));
		mov(eax, ecx);
		and_(ecx, int32(instructions::Bits(5)));
		shr(eax, cl);
		mov(get_register_op32(rd), eax);
	}
	else
	{
		mov(ecx, get_register_op32(rs));
		mov(eax, get_register_op32(rt));
		and_(ecx, int32(instructions::Bits(5)));
		shr(eax, cl);
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_SYNC(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// do nothing
}


Jit1_CodeGen::except_result Jit1_CodeGen::write_PROC_RDHWR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	static const int8 uv_offset =  value_assert<int8>(offsetof(processor, user_value_) - 128);

	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const uint32 selector = instructions::TinyInt<3>(instruction >> 6).zextend<uint32>();
	const uint32 reg_number = rd.get_register();

	if (selector == 0) [[likely]] {
		switch (reg_number) {
		case 29:
			std::ignore = mov_ex(get_register_op32(rt), dword[rbp + uv_offset], eax);
			return except_result::none;
		case 1:
			set(get_register_op32(rt), 0x100);
			return except_result::none;
		default: [[unlikely]]
			break;
		}
	}

	mov(ecx, int32(address));
	jmp(intrinsics_.ri, T_NEAR);
	return except_result::always_throw;
}

void Jit1_CodeGen::write_PROC_EXT(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info)
{
	// TODO : BMI1

	// rt = rs[msbd+lsb...lsb]
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	auto&& rs_reg = get_register_op32(rs);
	auto&& rt_reg = get_register_op32(rt);

	const uint32 msbd = instructions::TinyInt<5>(instruction >> 11).zextend<uint32>();
	const uint32 lsb =  instructions::TinyInt<5>(instruction >> 6).zextend<uint32>();

	if (rt.is_zero()) [[unlikely]] {
		// nop
	}
	else {
		if (lsb + msbd > 31) [[unlikely]] {
			// Result is unpredictable, just push -1.
			mov(rt_reg, -1);
		}
		else if (rs.is_zero()) [[unlikely]] {
			// The operation would just return 0.
			mov(rt_reg, 0);
		}
		else {
			// Equivalent logic to ProcInstructionDef::EXT
			const uint32_t mask = ((1U << (msbd + 1)) - 1) & (std::numeric_limits<uint32>::max() >> lsb);
			xassert(mask != 0 || (lsb == 0 && msbd == 31));
			if (mask == 0) [[unlikely]] {
				mov(rt_reg, 0);
			}
			else {
				mov(eax, rs_reg);
				if (lsb > 0) {
					shr(eax, lsb);
				}
				and_(eax, mask);
				mov(rt_reg, eax);
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

	auto&& rs_reg = get_register_op32(rs);
	auto&& rt_reg = get_register_op32(rt);

	const uint32 msb = instructions::TinyInt<5>(instruction >> 11).zextend<uint32>();
	const uint32 lsb = instructions::TinyInt<5>(instruction >> 6).zextend<uint32>();

	if (rt.is_zero()) [[unlikely]] {
		// nop
	}
	else if (lsb > msb) [[unlikely]] {
		// Result is unpredictable, just push -1.
		mov(rt_reg, -1);
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
				mov(rt_reg, 0);
			}
			else
			{
				and_(rt_reg, inverse_mask);
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
				and_(rt_reg, mask);
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
			xassert(lsb != 0);
			xassert(size_mask != std::numeric_limits<uint32>::max());

			// TODO : this can be one fewer instruction depending on the various values, as you can shift the masks and then use `lea`.
			mov(eax, rs_reg);
			mov(edx, rt_reg);
			and_(eax, size_mask);
			and_(edx, inverse_mask);
			shl(eax, static_cast<int>(lsb));
			or_(edx, eax);
			mov(rt_reg, edx);
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

	auto&& rs_reg = get_register_op32(rs);
	auto&& rt_reg = get_register_op32(rt);
	auto&& rd_reg = get_register_op32(rd);

	if (rd.is_zero()) [[unlikely]]
	{
		// nop
	}
	else if (rs.is_zero() && rt.is_zero()) [[unlikely]]
	{
		mov(rd_reg, 0);
	}
	else if (rs.is_zero()) [[unlikely]]
	{
		mov(eax, rt_reg);
		mov(rd_reg, eax);
	}
	else if (rs == rd)
	{
		if (rt.is_zero())
		{
			// rd = signed( ([rd] << (sa + 1)) )
			// TODO : is SAL correct?
			sal(rd_reg, sa);
		}
		else
		{
			// signed( rs <<= (sa + 1); rs += rt; )
			sal(rs_reg, sa);
			add(rs_reg, rt_reg);
		}
	}
	else
	{
		mov(eax, rs_reg);
		// TODO : is SAL correct?
		sal(eax, sa);
		// TODO : sign extension
		if (rt == rd)
		{
			add(rd_reg, eax);
		}
		else
		{
			if (!rt.is_zero()) [[likely]]
			{
				add(eax, rt_reg);
			}

			mov(rd_reg, eax);
		}
	}
}
