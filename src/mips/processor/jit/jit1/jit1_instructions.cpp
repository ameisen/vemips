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

	if (procedure <= 0xFFFFFFFF) {
		mov(eax, uint32(procedure));
		mov(dword[rbp + pc_offset], int32(address));
		mov(dword[rbp + flags_offset], ebx);
		mov(ecx, _ecx);
		mov(rdx, rbp);
		add(rdx, -128);
		call(rax);
		mov(dword[rbp + instructions::GPRegister<>{0}.get_offset(true)], 0); // clear zero register.
		mov(ebx, dword[rbp + flags_offset]);
	}
	else {
		mov(rax, uint64(procedure));
		mov(dword[rbp + pc_offset], int32(address));
		mov(dword[rbp + flags_offset], ebx);
		mov(ecx, _ecx);
		mov(rdx, rbp);
		add(rdx, -128);
		call(rax);
		mov(dword[rbp + instructions::GPRegister<>{0}.get_offset(true)], 0); //clear register
		mov(ebx, dword[rbp + flags_offset]);
	}
}

void Jit1_CodeGen::write_PROC_SUBU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) {
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	// [rd] = [rs] - [rt]

	if (rd.is_zero())
	{
		// nop
	}
	else if (rs == rt)
	{
		mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero())
	{
		mov(eax, get_register_op32(rt));
		neg(eax);
		mov(get_register_op32(rd), eax);
	}
	else if (rt.is_zero())
	{
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rd), eax);
	}
	else if (rd == rs)
	{
		mov(eax, get_register_op32(rt));
		sub(get_register_op32(rd), eax);
	}
	else
	{
		mov(eax, get_register_op32(rs));
		sub(eax, get_register_op32(rt));
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_SUB(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	// [rd] = [rs] - [rt]

	if (rd.is_zero())
	{
		// nop
	}
	else if (rs == rt)
	{
		 mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero())
	{
		// check if 'rt' is zero or not. If it's zero, we write zero to rd.
		// otherwise, we throw OV
		cmp(get_register_op32(rt), 0);
		jne(intrinsics_.ov, T_NEAR);
		mov(get_register_op32(rd), 0);
	}
	else if (rt.is_zero())
	{
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rd), eax);
	}
	else
	{
		mov(eax, get_register_op32(rs));
		sub(eax, get_register_op32(rt));
		jo(intrinsics_.ov, T_NEAR);
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_OR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs | rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rs.is_zero() && rt.is_zero())
	{
		// set rd to 0
		// 89 4A EE
		// mov dword [rdx + 0xEE], ecx ; EE = 'rd' offset
		mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero() || rt == rs)
	{
		// just move rt to rd
		// 8B 42 DD 89 42 EE 
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		mov(eax, get_register_op32(rt));
		mov(get_register_op32(rd), eax);
	}
	else if (rt.is_zero())
	{
		// just move rs to rd
		// 8B 42 DD 89 42 EE 
		// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rd), eax);
	}
	else
	{
		// actually perform OR
		// 8B 42 DD 0B 42 FF 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// or dword eax, [rdx + 0xFF] ; FF = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		mov(eax, get_register_op32(rt));
		or_(eax, get_register_op32(rs));
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_NOR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs \/ rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rs.is_zero() && rt.is_zero())
	{
		// set rd to all ones
		mov(get_register_op32(rd), int32(-1));
	}
	else if (rs.is_zero() || rt == rs)
	{
		// just move ~rt to rd
		mov(eax, get_register_op32(rt));
		not_(eax);
		mov(get_register_op32(rd), eax);
	}
	else if (rt.is_zero())
	{
		// just move ~rs to rd
		mov(eax, get_register_op32(rs));
		not_(eax);
		mov(get_register_op32(rd), eax);
	}
	else
	{
		// actually perform OR
		// 8B 42 DD 0B 42 FF 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// or dword eax, [rdx + 0xFF] ; FF = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		mov(eax, get_register_op32(rt));
		or_(eax, get_register_op32(rs));
		not_(eax);
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_AND(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs & rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rs.is_zero() || rt.is_zero())
	{
		// set rd to 0
		// 89 4A EE
		// mov dword [rdx + 0xEE], ecx ; EE = 'rd' offset
		mov(get_register_op32(rd), 0);
	}
	else if (rt == rs)
	{
		// just move rt to rd
		// 8B 42 DD 89 42 EE 
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		mov(eax, get_register_op32(rt));
		mov(get_register_op32(rd), eax);
	}
	else
	{
		// actually perform OR
		// 8B 42 DD 0B 42 FF 89 42 EE
		// mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
		// or dword eax, [rdx + 0xFF] ; FF = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
		mov(eax, get_register_op32(rt));
		and_(eax, get_register_op32(rs));
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_ORI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs | zimm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const uint32 immediate = instructions::TinyInt<16>(instruction).zextend<uint32>();

	if (rt.is_zero())
	{
		// nop
	}
	else if (rs.is_zero())
	{
		// set rt to immediate
		mov(get_register_op32(rt), immediate);
	}
	else if (immediate == 0)
	{
		// just move rs to rt
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rt), eax);
	}
	else
	{
		// actually perform OR
		mov(eax, immediate);
		or_(eax, get_register_op32(rs));
		mov(get_register_op32(rt), eax);
	}
}

void Jit1_CodeGen::write_PROC_ANDI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs & zimm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const uint32 immediate = instructions::TinyInt<16>(instruction).zextend<uint32>();

	if (rt.is_zero())
	{
		// nop
	}
	 else if (rs.is_zero() || immediate == 0) {
		 mov(get_register_op32(rt), 0);
	 }
	else
	{
		// actually perform AND
		mov(eax, immediate);
		and_(eax, get_register_op32(rs));
		mov(get_register_op32(rt), eax);
	}
}

// TODO optimize
void Jit1_CodeGen::write_PROC_SELEQZ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt ? 0 : rs
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero())
	{
		// select always hits.
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rd), eax);
	}
	else
	{
		if (rt == rs)
		{
			mov(ecx, get_register_op32(rt));
			cmp(ecx, 1); // Set carry flag if zero
			sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
			and_(eax, ecx);
			mov(get_register_op32(rd), eax);
		}
		else
		{
			cmp(get_register_op32(rt), 1); // Set carry flag if zero
			sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
			and_(eax, get_register_op32(rs));
			mov(get_register_op32(rd), eax);
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

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero())
	{
		// select never hits.
		mov(get_register_op32(rd), 0);
	}
	else
	{
		if (rt == rs)
		{
			mov(ecx, get_register_op32(rt));
			cmp(ecx, 1);							// Set carry flag if zero
			cmc();
			sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
			and_(eax, ecx);
			mov(get_register_op32(rd), eax);
		}
		else
		{
			cmp(get_register_op32(rt), 1); // Set carry flag if zero
			cmc();
			sbb(eax, eax);						// if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
			and_(eax, get_register_op32(rs));
			mov(get_register_op32(rd), eax);
		}
	}
}

void Jit1_CodeGen::write_PROC_MOVE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	if (rt.is_zero() || rt == rs)
	{
		// nop
	}
	else if (rs.is_zero())
	{
		// This just sets rt to 0.
		// 89 4A EE 
		// mov dword [rdx + 0xEE], ecx ; EE = 'rt' offset
		mov(get_register_op32(rt), 0);
	}
	else
	{
		// We are just moving rs to rt
		// 8B 42 DD 89 42 EE 
		// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rt), eax);
	}
}

void Jit1_CodeGen::write_PROC_ADDIU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs + immediate
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const int32 immediate = instructions::TinyInt<16>(instruction).sextend<int32>();

	if (rt.is_zero())
	{
		// nop
	}
	else if (rs.is_zero())
	{
		if (immediate == 0)
		{
			// This just sets rt to 0.
			// 89 4A EE 
			// mov dword [rdx + 0xEE], ecx ; EE = 'rt' offset
			mov(get_register_op32(rt), 0);
		}
		// // // // // // TODO 128/-128
		else
		{
			// This just sets rt to the immediate value.
			// C7 42 EE FF FF FF FF 
			// mov dword [rdx + 0xEE], 0xFFFFFFFF		 ; EE = 'rt' offset | FFFF = 16-bit immediate value
			mov(get_register_op32(rt), immediate);
		}
	}
	else if (rs == rt)
	{
		if (immediate == 0)
		{
			// nop
		}
		else if (immediate == 1)
		{
			// FF 42 EE 
			// inc dword [rdx + 0xEE]		 ; EE = 'rt' offset
			inc(get_register_op32(rt));
		}
		else if (immediate == -1)
		{
			// FF 4A EE 
			// dec dword [rdx + 0xEE]		 ; EE = 'rt' offset
			dec(get_register_op32(rt));
		}
		else if (immediate >= 0 && immediate <= 128)
		{
			// using 'sub' for add means we can go to 128
			// 83 6A EE FF 
			// sub dword [rdx + 0xEE], 0xFF		 ; EE = 'rt' offset
			sub(get_register_op32(rt), int8(-immediate));
		}
		else
		{
			// We are just adding immediate to 'rt'.
			// 83 42 EE FF
			// add dword [rdx + 0xEE], 0xFF		 ; EE = 'rt' offset
			// 81 42 EE FF FF FF FF 
			// add dword [rdx + 0xEE], 0xFFFFFFFF		 ; EE = 'rt' offset | FFFF = 16-bit immediate value
			add(get_register_op32(rt), immediate);
		}
	}
	else
	{
		mov(eax, get_register_op32(rs));

		if (immediate == 0)
		{
			// We are just moving rs to rt
			// 8B 42 DD 89 42 EE 
			// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
			// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
		}
		else if (immediate == 1)
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

		mov(get_register_op32(rt), eax);
	}
}

void Jit1_CodeGen::write_PROC_ADDI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs + immediate
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const int32 immediate = instructions::TinyInt<16>(instruction).sextend<int32>();

	if (rt.is_zero())
	{
		// nop
	}
	else if (rs.is_zero())
	{
		if (immediate < 0)
		{
			jmp(intrinsics_.ov, T_NEAR);
		}
		else if (immediate == 0)
		{
			// This just sets rt to 0.
			// 89 4A EE 
			// mov dword [rdx + 0xEE], ecx ; EE = 'rt' offset
			mov(get_register_op32(rt), 0);
		}
		// // // // // // TODO 128/-128
		else
		{
			// This just sets rt to the immediate value.
			// C7 42 EE FF FF FF FF 
			// mov dword [rdx + 0xEE], 0xFFFFFFFF		 ; EE = 'rt' offset | FFFF = 16-bit immediate value
			mov(get_register_op32(rt), immediate);
		}
	}
	else if (rs == rt)
	{
		if (immediate == 0)
		{
			// nop
		}
		else
		{
			mov(eax, get_register_op32(rt));

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
			mov(get_register_op32(rt), eax);
		}
	}
	else if (immediate == 0)
	{
		// We are just moving rs to rt
		// 8B 42 DD 89 42 EE 
		// mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
		// mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rt), eax);
	}
	else
	{
		mov(eax, get_register_op32(rs));

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
		mov(get_register_op32(rt), eax);
	}
}

void Jit1_CodeGen::write_PROC_ADDU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs + rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero() && rd.is_zero())
	{
		// set [rd] to 0.
		mov(get_register_op32(rd), 0);
	}
	else if (rt.is_zero())
	{
		// move [rs] to [rd]
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rd), eax);
	}
	else if (rs.is_zero())
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rt));
		mov(get_register_op32(rd), eax);
	}
	else if (rs == rd)
	{
		// add [rt] to [rd]
		mov(eax, get_register_op32(rt));
		add(get_register_op32(rd), eax);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(eax, get_register_op32(rs));
		if (rs == rt) {
			add(eax, eax);
		}
		else {
			add(eax, get_register_op32(rt));
		}
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_ADD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs + rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero() && rd.is_zero())
	{
		// set [rd] to 0.
		mov(get_register_op32(rd), 0);
	}
	else if (rt.is_zero())
	{
		// move [rs] to [rd]
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rd), eax);
	}
	else if (rs.is_zero())
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rt));
		mov(get_register_op32(rd), eax);
	}
	/*
	else if (rs == rd)
	{
		// add [rt] to [rd]
		mov(eax, get_register_op32(rt));
		add(get_register_op32(rd), eax);
		jo(intrinsics_.ov, T_NEAR);
	}
	*/
	else
	{
		// add [rs] and [rt] to [rd]
		mov(eax, get_register_op32(rs));
		if (rs == rt) {
			add(eax, eax);
		}
		else {
			add(eax, get_register_op32(rt));
		}
		jo(intrinsics_.ov, T_NEAR);
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_AUI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs + simm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	int32 immediate = instructions::TinyInt<32>(instruction << 16).sextend<int32>();

	if (rt.is_zero())
	{
		// nop
	}
	else if (rs.is_zero())
	{
		mov(get_register_op32(rt), immediate);
	}
	else if (immediate == 0)
	{
		// move [rs] to [rt]
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rt), eax);
	}
	else if (immediate < 0)
	{
		mov(eax, get_register_op32(rs));
		add(eax, immediate);
		mov(get_register_op32(rt), eax);
	}
	else
	{
		immediate = -immediate;
		mov(eax, get_register_op32(rs));
		sub(eax, immediate);
		mov(get_register_op32(rt), eax);
	}
}

void Jit1_CodeGen::write_PROC_SLT(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs < rt

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt == rs)
	{
		// rd == x < x
		mov(get_register_op32(rd), 0);
	}
	else
	{
		if (rt.is_zero())
		{
			// rd = rs < 0
			xor_(ecx, ecx);
			cmp(get_register_op32(rs), 0);
			setl(cl);
			mov(get_register_op32(rd), ecx);
		}
		else if (rs.is_zero())
		{
			// rd = 0 < rt
			// rd = rt > 0
			xor_(ecx, ecx);
			cmp(get_register_op32(rt), 0);
			setg(cl);
			mov(get_register_op32(rd), ecx);
		}
		else
		{
			mov(eax, get_register_op32(rs));
			xor_(ecx, ecx);
			cmp(eax, get_register_op32(rt));
			setl(cl);
			mov(get_register_op32(rd), ecx);
		}
	}
}

void Jit1_CodeGen::write_PROC_SLTU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs < rt

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero() || rt == rs)
	{
		// rd = rs < 0
		// rd = x < x
		mov(get_register_op32(rd), 0);
	}
	else
	{
		if (rs.is_zero())
		{
			// rd = 0 < rt
			// rd = rt > 0
			// rd = rd != 0
			xor_(ecx, ecx);
			cmp(get_register_op32(rd), 0);
			setz(cl);
			mov(get_register_op32(rd), ecx);
		}
		else
		{
			mov(eax, get_register_op32(rs));
			xor_(ecx, ecx);
			cmp(eax, get_register_op32(rt));
			setb(cl);
			mov(get_register_op32(rd), ecx);
		}
	}
}

void Jit1_CodeGen::write_PROC_SLTI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs < simm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const int32 immediate = instructions::TinyInt<16>(instruction).sextend<int32>();

	if (rt.is_zero())
	{
		// nop
	}
	if (rs.is_zero())
	{
		// rt = 0 < 0|1...
		mov(get_register_op32(rt), (immediate > 0) ? 1 : 0);
	}
	else
	{
		xor_(ecx, ecx);
		cmp(get_register_op32(rs), immediate);
		setl(cl);
		mov(get_register_op32(rt), ecx);
	}
}

void Jit1_CodeGen::write_PROC_SLTIU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs < zimm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const uint32 immediate = uint32(instructions::TinyInt<16>(instruction).sextend<int32>()); // intended

	if (rt.is_zero())
	{
		// nop
	}
	else if (immediate == 0)
	{
		// rt = rs < 0
		mov(get_register_op32(rt), 0);
	}
	else if (rs.is_zero())
	{
		// rt = 0 < imm
		// imm > 0
		mov(get_register_op32(rt), (immediate > 0) ? 1 : 0);
	}
	else
	{
		xor_(ecx, ecx);
		cmp(get_register_op32(rs), immediate);
		setb(cl);
		mov(get_register_op32(rt), ecx);
	}
}

void Jit1_CodeGen::write_COP1_MFC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const instructions::FPRegister<11, 5> fs(instruction, *jit_.processor_.get_fpu_coprocessor());
	const int16 fs_offset = fs.get_offset();

	if (rt.is_zero())
	{
		// nop
	}
	else
	{
		mov(eax, dword[r12 + fs_offset]);
		mov(get_register_op32(rt), eax);
	}
}

void Jit1_CodeGen::write_COP1_MTC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const instructions::FPRegister<11, 5> fs(instruction, jit_.processor_.get_fpu_coprocessor());
	const int16 fs_offset = fs.get_offset();

	if (rt.is_zero())
	{
		// write 0 to [fs]
		mov(dword[r12 + fs_offset], 0);
	}
	else
	{
		mov(eax, get_register_op32(rt));
		mov(dword[r12 + fs_offset], eax);
	}
}

void Jit1_CodeGen::write_COP1_MFHC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const instructions::FPRegister<11, 5> fs(instruction, jit_.processor_.get_fpu_coprocessor());
	const int16 fs_offset = fs.get_offset();

	if (rt.is_zero())
	{
		// nop
	}
	else
	{
		mov(eax, dword[r12 + (fs_offset + 4)]);
		mov(get_register_op32(rt), eax);
	}
}

void Jit1_CodeGen::write_COP1_MTHC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	const instructions::FPRegister<11, 5> fs(instruction, jit_.processor_.get_fpu_coprocessor());
	const int16 fs_offset = fs.get_offset();

	if (rt.is_zero())
	{
		// write 0 to [fs]
		mov(dword[r12 + (fs_offset + 4)], 0);
	}
	else
	{
		mov(eax, get_register_op32(rt));
		mov(dword[r12 + (fs_offset + 4)], eax);
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

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero() || rs.is_zero()) // fixed bug
	{
		// set [rd] to 0.
		mov(get_register_op32(rd), 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rt));
		imul(get_register_op32(rd));
		mov(get_register_op32(rd), eax);
	}
	else if (rt == rd)
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rs));
		imul(get_register_op32(rd));
		mov(get_register_op32(rd), eax);
	}
	else
	{
		// mul [rs] and [rt] to [rd]
		mov(eax, get_register_op32(rs));
		if (rs == rt) {
			imul(eax);
		}
		else {
			imul(get_register_op32(rt));
		}
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_MULU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero() || rs.is_zero()) // fixed bug
	{
		// set [rd] to 0.
		mov(get_register_op32(rd), 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rt));
		mul(get_register_op32(rd));
		mov(get_register_op32(rd), eax);
	}
	else if (rt == rd)
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rs));
		mul(get_register_op32(rd));
		mov(get_register_op32(rd), eax);
	}
	else
	{
		// mul [rs] and [rt] to [rd]
		mov(eax, get_register_op32(rs));
		if (rs == rt) {
			mul(eax);
		}
		else {
			mul(get_register_op32(rt));
		}
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_MUH(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero() || rs.is_zero()) // fixed bug
	{
		// set [rd] to 0.
		mov(get_register_op32(rd), 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rt));
		imul(get_register_op32(rd));
		mov(get_register_op32(rd), edx);
	}
	else if (rt == rd)
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rs));
		imul(get_register_op32(rd));
		mov(get_register_op32(rd), edx);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(eax, get_register_op32(rs));
		if (rs == rt) {
			imul(eax);
		}
		else {
			imul(get_register_op32(rt));
		}
		mov(get_register_op32(rd), edx);
	}
}

void Jit1_CodeGen::write_PROC_MUHU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero() || rs.is_zero()) // fixed bug
	{
		// set [rd] to 0.
		mov(get_register_op32(rd), 0);
	}
	else if (rs == rd)
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rt));
		mul(get_register_op32(rd));
		mov(get_register_op32(rd), edx);
	}
	else if (rt == rd)
	{
		// move [rt] to [rd]
		mov(eax, get_register_op32(rs));
		mul(get_register_op32(rd));
		mov(get_register_op32(rd), edx);
	}
	else
	{
		// add [rs] and [rt] to [rd]
		mov(eax, get_register_op32(rs));
		if (rs == rt) {
			mul(eax);
		}
		else {
			mul(get_register_op32(rt));
		}
		mov(get_register_op32(rd), edx);
	}
}

// TODO consider scanning forward for further DIV/MOD instructions using these parameters.
// Values might be able to be cached.

void Jit1_CodeGen::write_PROC_DIV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	const Xbyak::Label divzero;
	// [rd] = [rs] / [rt]

	if (rd.is_zero())
	{
		// TODO should we throw an exception?
		// nop
	}
	else if (rt.is_zero())
	{
		// divzero
		// set [rd] to 0.
		// TODO should we throw an exception?
		mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero())
	{
		mov(get_register_op32(rd), 0);
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
	else if (rs == rt)
	{
		// move [rt] to [rd]
		cmp(get_register_op32(rt), 0);
		je(divzero);
		mov(get_register_op32(rd), 1);
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
	// rd = rs * rt
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	const Xbyak::Label divzero;
	// [rd] = [rs] / [rt]

	if (rd.is_zero())
	{
		// TODO should we throw an exception?
		// nop
	}
	else if (rt.is_zero())
	{
		// divzero
		// set [rd] to 0.
		// TODO should we throw an exception?
		mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero())
	{
		mov(get_register_op32(rd), 0);
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
	else if (rs == rt)
	{
		// move [rt] to [rd]
		cmp(get_register_op32(rt), 0);
		je(divzero);
		mov(get_register_op32(rd), 1);
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

	if (rd.is_zero())
	{
		// TODO should we throw an exception?
		// nop
	}
	else if (rt.is_zero())
	{
		// divzero
		// set [rd] to 0.
		// TODO should we throw an exception?
		mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero())
	{
		mov(get_register_op32(rd), 0);
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
	else if (rs == rt)
	{
		// move [rt] to [rd]
		cmp(get_register_op32(rt), 0);
		je(divzero);
		mov(get_register_op32(rd), 0);
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

	if (rd.is_zero())
	{
		// TODO should we throw an exception?
		// nop
	}
	else if (rt.is_zero())
	{
		// divzero
		// set [rd] to 0.
		// TODO should we throw an exception?
		mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero())
	{
		mov(get_register_op32(rd), 0);
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
	else if (rs == rt)
	{
		// move [rt] to [rd]
		cmp(get_register_op32(rt), 0);
		je(divzero);
		mov(get_register_op32(rd), 0);
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

	if (rd.is_zero())
	{
		// nop
	}
	else if (rs == rt)
	{
		// set rd to 0
		// 89 4A EE
		// mov dword [rdx + 0xEE], ecx ; EE = 'rd' offset
		mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero())
	{
		mov(eax, get_register_op32(rt));
		mov(get_register_op32(rd), eax);
	}
	else if (rt.is_zero())
	{
		mov(eax, get_register_op32(rs));
		mov(get_register_op32(rd), eax);
	}
	else
	{
		 if (rd == rt) {
			 mov(eax, get_register_op32(rs));
			 xor_(get_register_op32(rd), eax);
		 }
		 else if (rd == rs) {
			 mov(eax, get_register_op32(rt));
			 xor_(get_register_op32(rd), eax);
		 }
		 else {
			 mov(eax, get_register_op32(rt));
			 xor_(eax, get_register_op32(rs));
			 mov(get_register_op32(rd), eax);
		 }
	}
}

void Jit1_CodeGen::write_PROC_XORI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rt = rs | zimm
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const uint32 immediate = instructions::TinyInt<16>(instruction).zextend<uint32>();

	if (rt.is_zero())
	{
		// nop
	}
	else if (rs.is_zero())
	{
		// set rt to offset
		mov(get_register_op32(rt), immediate);
	}
	else if (immediate == 0)
	{
		 if (rs != rt) {
			 // just move rs to rt
			 mov(eax, get_register_op32(rs));
			 mov(get_register_op32(rt), eax);
		 }
		 else
		 {
			 // nop
		 }
	}
	else
	{
		// actually perform OR
		 if (rt == rs) {
			 xor_(get_register_op32(rt), immediate);
		 }
		 else {
			 mov(eax, get_register_op32(rs));
			 xor_(eax, immediate);
			 mov(get_register_op32(rt), eax);
		 }
	}
}

void Jit1_CodeGen::write_PROC_SEB(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = sbyte(rt)
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero())
	{
		mov(get_register_op32(rd), 0);
	}
	else
	{
		movsx(eax, get_register_op8(rt));
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_SEH(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = sword(rt)
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero())
	{
		mov(get_register_op32(rd), 0);
	}
	else
	{
		movsx(eax, get_register_op8(rt));
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_SLL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt <<< sa

	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const int8 sa = instructions::TinyInt<5>(instruction >> 6).zextend<int8>();

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero())
	{
		mov(get_register_op32(rd), 0);
	}
	else if (rd == rt)
	{
		if (sa == 0)
		{
			// nop
		}
		else
		{
			shl(get_register_op32(rd), sa);
		}
	}
	else
	{
		mov(eax, get_register_op32(rt));

		if (sa == 0)
		{
			// move
			// TODO : global func
		}
		else
		{
			shl(eax, sa);
		}

		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_SRL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt >>> sa

	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const int8 sa = instructions::TinyInt<5>(instruction >> 6).zextend<int8>();

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero())
	{
		mov(get_register_op32(rd), 0);
	}
	else if (rd == rt)
	{
		if (sa == 0)
		{
			// nop
		}
		else
		{
			shr(get_register_op32(rd), sa);
		}
	}
	else
	{
		mov(eax, get_register_op32(rt));

		if (sa == 0)
		{
			// move
		}
		else
		{
			shr(eax, sa);
		}

		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_SRA(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt >> sa

	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const int8 sa = instructions::TinyInt<5>(instruction >> 6).zextend<int8>();

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero())
	{
		mov(get_register_op32(rd), 0);
	}
	else if (rd == rt)
	{
		if (sa == 0)
		{
			// nop
		}
		else
		{
			sar(get_register_op32(rd), sa);
		}
	}
	else
	{
		mov(eax, get_register_op32(rt));

		if (sa == 0)
		{
			// move
		}
		else
		{
			sar(eax, sa);
		}

		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_SLLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt <<< rs

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero())
	{
		// set rd to 0.
		mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero())
	{
		// move rt to rd
		mov(eax, get_register_op32(rt));
		mov(get_register_op32(rd), eax);
	}
	else if (rs == rt)
	{
		mov(ecx, get_register_op32(rs));
		mov(eax, ecx);
		and_(ecx, int32(instructions::Bits(5)));
		shl(eax, cl);
		mov(get_register_op32(rd), eax);
	}
	else
	{
		mov(ecx, get_register_op32(rs));
		mov(eax, get_register_op32(rt));
		and_(ecx, int32(instructions::Bits(5)));
		shl(eax, cl);
		mov(get_register_op32(rd), eax);
	}
}

void Jit1_CodeGen::write_PROC_SRLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	// rd = rt >>> rs

	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);

	if (rd.is_zero())
	{
		// nop
	}
	else if (rt.is_zero())
	{
		// set rd to 0.
		mov(get_register_op32(rd), 0);
	}
	else if (rs.is_zero())
	{
		// move rt to rd
		mov(eax, get_register_op32(rt));
		mov(get_register_op32(rd), eax);
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


void Jit1_CodeGen::write_PROC_RDHWR(jit1::ChunkOffset & __restrict chunk_offset, bool& terminate_instruction, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
	static const int8 uv_offset =  value_assert<int8>(offsetof(processor, user_value_) - 128);

	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);
	const instructions::GPRegister<11, 5> rd(instruction, jit_.processor_);
	const uint32 selector = instructions::TinyInt<3>(instruction >> 6).zextend<uint32>();
	const uint32 reg_number = rd.get_register();

	if (selector == 0) {
		switch (reg_number) {
		case 29:
			mov(eax, dword[rbp + uv_offset]);
			mov(get_register_op32(rt), eax);
			return;
		case 1:
			mov(get_register_op32(rt), 0x100);
			return;
		}
	}
	terminate_instruction = true;
	mov(eax, int32(address));
	jmp(intrinsics_.ri, T_NEAR);
}

void Jit1_CodeGen::write_PROC_EXT(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info)
{
	const instructions::GPRegister<21, 5> rs(instruction, jit_.processor_);
	const instructions::GPRegister<16, 5> rt(instruction, jit_.processor_);

	auto& rs_reg = get_register_op32(rs);
	auto& rt_reg = get_register_op32(rt);

	const uint32 msbd = instructions::TinyInt<5>(instruction >> 11).zextend<uint32>();
	const uint32 lsb = instructions::TinyInt<5>(instruction >> 6).zextend<uint32>();

	if (rt.is_zero()) {
		// nop
	}
	else {
		if (lsb + msbd > 31) {
			// Result is unpredictable, just push -1.
			mov(rt_reg, -1);
		}
		else if (rs.is_zero()) {
			// The operation would just return 0.
			mov(rt_reg, 0);
		}
		else {
			// Equivalent logic to ProcInstructionDef::EXT
			const uint32_t mask = ((1U << (msbd + 1)) - 1) & (uint32_t(-1) >> lsb); // Optimization, mainly, to detect if the mask would always be zero.
			if (mask == 0) {
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
