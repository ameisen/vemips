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

void Jit1_CodeGen::insert_procedure_ecx(uint32 address, uint64 procedure, uint32 _ecx, const mips::instructions::InstructionInfo & __restrict instruction_info)
{
   static const int8 flags_offset = sbyte_assert(offset_of(&processor::m_flags) - 128);
   static const int8 pc_offset =  sbyte_assert(offset_of(&processor::m_program_counter) - 128);
   static const int8 ic_offset =  sbyte_assert(offset_of(&processor::m_instruction_count) - 128);
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   if (procedure <= 0xFFFFFFFF)
   {
      mov(eax, uint32(procedure));
      mov(dword[rbp + pc_offset], int32(address));
      mov(dword[rbp + flags_offset], ebx);
      mov(ecx, _ecx);
      mov(rdx, rbp);
      add(rdx, -128);
      call(rax);
      mov(dword[rbp + gp_offset], 0); // clear zero register.
      mov(ebx, dword[rbp + flags_offset]);
   }
   else
   {
      mov(rax, uint64(procedure));
      mov(dword[rbp + pc_offset], int32(address));
      mov(dword[rbp + flags_offset], ebx);
      mov(ecx, _ecx);
      mov(rdx, rbp);
      add(rdx, -128);
      call(rax);
      mov(dword[rbp + gp_offset], 0); //clear register
      mov(ebx, dword[rbp + flags_offset]);
   }
}

void Jit1_CodeGen::write_PROC_SUBU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   // [rd] = [rs] - [rt]

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == rt.get_register())
   {
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0)
   {
      mov(eax, get_register_op32(rt));
      neg(eax);
      mov(get_register_op32(rd), eax);
   }
   else if (rt.get_register() == 0)
   {
      mov(eax, get_register_op32(rs));
      mov(get_register_op32(rd), eax);
   }
   else if (rd.get_register() == rs.get_register())
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

void Jit1_CodeGen::write_PROC_SUB(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   // [rd] = [rs] - [rt]

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == rt.get_register())
   {
		 mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0)
   {
      // check if 'rt' is zero or not. If it's zero, we write zero to rd.
      // otherwise, we throw OV
		 cmp(get_register_op32(rt), 0);
      jne("intrinsic_ov_ex", T_NEAR);
      mov(get_register_op32(rd), 0);
   }
   else if (rt.get_register() == 0)
   {
      mov(eax, get_register_op32(rs));
      mov(get_register_op32(rd), eax);
   }
   else
   {
      const auto no_ov = GetUniqueLabel();
      mov(eax, get_register_op32(rs));
      sub(eax, get_register_op32(rt));
      jo("intrinsic_ov_ex", T_NEAR);
      mov(get_register_op32(rd), eax);
   }
}

void Jit1_CodeGen::write_PROC_OR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs | rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == 0 && rt.get_register() == 0)
   {
      // set rd to 0
      // 89 4A EE
      // mov dword [rdx + 0xEE], ecx ; EE = 'rd' offset
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0 || rt.get_register() == rs.get_register())
   {
      // just move rt to rd
      // 8B 42 DD 89 42 EE 
      // mov dword eax, [rdx + 0xDD] ; DD = 'rt' offset
      // mov dword [rdx + 0xEE], eax ; EE = 'rd' offset
      mov(eax, get_register_op32(rt));
      mov(get_register_op32(rd), eax);
   }
   else if (rt.get_register() == 0)
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

void Jit1_CodeGen::write_PROC_NOR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs | rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == 0 && rt.get_register() == 0)
   {
      // set rd to all ones
      mov(get_register_op32(rd), int32(-1));
   }
   else if (rs.get_register() == 0 || rt.get_register() == rs.get_register())
   {
      // just move ~rt to rd
      mov(eax, get_register_op32(rt));
      not_(eax);
      mov(get_register_op32(rd), eax);
   }
   else if (rt.get_register() == 0)
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

void Jit1_CodeGen::write_PROC_AND(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs | rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == 0 || rt.get_register() == 0)
   {
      // set rd to 0
      // 89 4A EE
      // mov dword [rdx + 0xEE], ecx ; EE = 'rd' offset
      mov(get_register_op32(rd), 0);
   }
   else if (rt.get_register() == rs.get_register())
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

void Jit1_CodeGen::write_PROC_ORI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs | rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const uint32 immediate = instructions::TinyInt<16>(instruction).zextend<uint32>();

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   if (rt.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == 0)
   {
      // set rt to offset
      if (immediate == 0)
      {
         mov(get_register_op32(rt), int8(0));
      }
      else
      {
         mov(get_register_op32(rt), int32(immediate));
      }
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
		 mov(eax, int32(immediate));
		 or_(eax, get_register_op32(rs));
      mov(get_register_op32(rt), eax);
   }
}

void Jit1_CodeGen::write_PROC_ANDI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs | rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const uint32 immediate = instructions::TinyInt<16>(instruction).zextend<uint32>();

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   if (rt.get_register() == 0)
   {
      // nop
   }
	 else if (rs.get_register() == 0 || immediate == 0) {
		 mov(get_register_op32(rt), 0);
	 }
   else
   {
      // actually perform AND
      mov(eax, int32(immediate));
      and_(eax, get_register_op32(rs));
      mov(get_register_op32(rt), eax);
   }
}

// TODO optimize
void Jit1_CodeGen::write_PROC_SELEQZ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs | rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      // select always hits.
      mov(eax, get_register_op32(rs));
      mov(get_register_op32(rd), eax);
   }
   else
   {
      if (rt_offset == rs_offset)
      {
         mov(ecx, get_register_op32(rt));
         cmp(ecx, 1); // Set carry flag if zero
         sbb(eax, eax);                  // if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
         and_(eax, ecx);
         mov(get_register_op32(rd), eax);
      }
      else
      {
         cmp(get_register_op32(rt), 1); // Set carry flag if zero
         sbb(eax, eax);                  // if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
         and_(eax, get_register_op32(rs));
         mov(get_register_op32(rd), eax);
      }
   }
}

// TODO optimize
void Jit1_CodeGen::write_PROC_SELNEZ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs | rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      // select never hits.
      mov(get_register_op32(rd), 0);
   }
   else
   {
      if (rt_offset == rs_offset)
      {
         mov(ecx, get_register_op32(rt));
         cmp(ecx, 1);                     // Set carry flag if zero
         cmc();
         sbb(eax, eax);                  // if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
         and_(eax, ecx);
         mov(get_register_op32(rd), eax);
      }
      else
      {
         cmp(get_register_op32(rt), 1); // Set carry flag if zero
         cmc();
         sbb(eax, eax);                  // if carry flag is zero, sets 0, otherwise sets 0xFFFFFFFF
         and_(eax, get_register_op32(rs));
         mov(get_register_op32(rd), eax);
      }
   }
}

void Jit1_CodeGen::write_PROC_MOVE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   if (rt.get_register() == 0 || rt.get_register() == rs.get_register())
   {
      // nop
   }
   else if (rs.get_register() == 0)
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

void Jit1_CodeGen::write_PROC_ADDIU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rt = rs + immediate
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const int32 immediate = instructions::TinyInt<16>(instruction).sextend<int32>();

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   if (rt.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == 0)
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
         // mov dword [rdx + 0xEE], 0xFFFFFFFF       ; EE = 'rt' offset | FFFF = 16-bit immediate value
         mov(get_register_op32(rt), immediate);
      }
   }
   else if (rs.get_register() == rt.get_register())
   {
      if (immediate == 0)
      {
         // nop
      }
      else if (immediate == 1)
      {
         // FF 42 EE 
         // inc dword [rdx + 0xEE]       ; EE = 'rt' offset
         inc(get_register_op32(rt));
      }
      else if (immediate == -1)
      {
         // FF 4A EE 
         // dec dword [rdx + 0xEE]       ; EE = 'rt' offset
         dec(get_register_op32(rt));
      }
      else if (immediate >= 0 && immediate <= 128)
      {
         // using 'sub' for add means we can go to 128
         // 83 6A EE FF 
         // sub dword [rdx + 0xEE], 0xFF       ; EE = 'rt' offset
         sub(get_register_op32(rt), int8(-immediate));
      }
      else if (immediate < 0 && -immediate <= 128 )
      {
         // using 'add' with sub means we can go to -128
         // 83 42 EE FF
         // add dword [rdx + 0xEE], 0xFF       ; EE = 'rt' offset
         add(get_register_op32(rt), int8(immediate));
      }
      else
      {
         // We are just adding immediate to 'rt'.
         // 81 42 EE FF FF FF FF 
         // add dword [rdx + 0xEE], 0xFFFFFFFF       ; EE = 'rt' offset | FFFF = 16-bit immediate value
         add(get_register_op32(rt), immediate);
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
   else if (immediate == 1)
   {
      // increment
      // 8B 42 DD FF C0 89 42 EE 
      // mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
      // inc eax
      // mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
      mov(eax, get_register_op32(rs));
      inc(eax);
      mov(get_register_op32(rt), eax);
   }
   else if (immediate == -1)
   {
		 // decrement
		 // 8B 42 DD FF C8 89 42 EE  
		 // mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
		 // dec eax
		 // mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
		 mov(eax, get_register_op32(rs));
		 dec(eax);
		 mov(get_register_op32(rt), eax);
   }
   else if (immediate >= 0 && immediate <= 128)
   {
      // using 'sub' for add means we can go to 128
      // 8B 42 DD 83 E8 79 89 42 EE 
      // mov dword eax, [rdx + 0xDD]        ; DD = 'rs' offset
      // sub dword eax, 0x79                ; 79 = immediate
      // mov dword [rdx + 0xEE], eax        ; EE = 'rt' offset
      mov(eax, get_register_op32(rs));
      sub(eax, int8(-immediate));
      mov(get_register_op32(rt), eax);
   }
   else if (immediate < 0 && -immediate <= 128)
   {
      // using 'add' with sub means we can go to -128
      // 8B 42 DD 83 C0 79 89 42 EE 
      // mov dword eax, [rdx + 0xDD]        ; DD = 'rs' offset
      // add dword eax, 0x79                ; 79 = immediate
      // mov dword [rdx + 0xEE], eax        ; EE = 'rt' offset
      mov(eax, get_register_op32(rs));
      add(eax, int8(immediate));
      mov(get_register_op32(rt), eax);
   }
   else
   {
      // this is a legitimate addiu
      // 8B 42 DD 05 FF FF FF FF 89 42 EE 
      // mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
      // add dword eax, 0xFFFFFFFF       ; FFFF = 16-bit immediate value
      // mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
      mov(eax, get_register_op32(rs));
      add(eax, immediate);
      mov(get_register_op32(rt), eax);
   }
}

void Jit1_CodeGen::write_PROC_ADDI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rt = rs + immediate
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const int32 immediate = instructions::TinyInt<16>(instruction).sextend<int32>();

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   if (rt.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == 0)
   {
      if (immediate < 0)
      {
         jmp("intrinsic_ov_ex", T_NEAR);
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
         // mov dword [rdx + 0xEE], 0xFFFFFFFF       ; EE = 'rt' offset | FFFF = 16-bit immediate value
         mov(get_register_op32(rt), immediate);
      }
   }
   else if (rs.get_register() == rt.get_register())
   {
      if (immediate == 0)
      {
         // nop
      }
      else if (immediate == 1)
      {
         mov(eax, get_register_op32(rt));
         inc(eax);
         jo("intrinsic_ov_ex", T_NEAR);
         mov(get_register_op32(rt), eax);
      }
      else if (immediate == -1)
      {
         mov(eax, get_register_op32(rt));
         dec(eax);
         jo("intrinsic_ov_ex", T_NEAR);
         mov(get_register_op32(rt), eax);
      }
      else if (immediate >= 0 && immediate <= 128)
      {
         // using 'sub' for add means we can go to 128
         // 83 6A EE FF 
         // sub dword [rdx + 0xEE], 0xFF       ; EE = 'rt' offset
         mov(eax, get_register_op32(rt));
         sub(eax, int8(-immediate));
         jo("intrinsic_ov_ex", T_NEAR);
         mov(get_register_op32(rt), eax);
      }
      else if (immediate < 0 && -immediate <= 128 )
      {
         // using 'add' with sub means we can go to -128
         // 83 42 EE FF
         // add dword [rdx + 0xEE], 0xFF       ; EE = 'rt' offset
         mov(eax, get_register_op32(rt));
         add(eax, int8(immediate));
         jo("intrinsic_ov_ex", T_NEAR);
         mov(get_register_op32(rt), eax);
      }
      else
      {
         // We are just adding immediate to 'rt'.
         // 81 42 EE FF FF FF FF 
         // add dword [rdx + 0xEE], 0xFFFFFFFF       ; EE = 'rt' offset | FFFF = 16-bit immediate value
         mov(eax, get_register_op32(rt));
         add(eax, immediate);
         jo("intrinsic_ov_ex", T_NEAR);
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
   else if (immediate == 1)
   {
      // increment
      // 8B 42 DD FF C0 89 42 EE 
      // mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
      // inc eax
      // mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
      mov(eax, get_register_op32(rs));
      inc(eax);
      jo("intrinsic_ov_ex", T_NEAR);
      mov(get_register_op32(rt), eax);
   }
   else if (immediate == -1)
   {
      // decrement
      // 8B 42 DD FF C8 89 42 EE  
      // mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
      // dec eax
      // mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
      mov(eax, get_register_op32(rs));
      dec(eax);
      jo("intrinsic_ov_ex", T_NEAR);
      mov(get_register_op32(rt), eax);
   }
   else if (immediate >= 0 && immediate <= 128)
   {
      // using 'sub' for add means we can go to 128
      // 8B 42 DD 83 E8 79 89 42 EE 
      // mov dword eax, [rdx + 0xDD]        ; DD = 'rs' offset
      // sub dword eax, 0x79                ; 79 = immediate
      // mov dword [rdx + 0xEE], eax        ; EE = 'rt' offset
      mov(eax, get_register_op32(rs));
      sub(eax, int8(-immediate));
      jo("intrinsic_ov_ex", T_NEAR);
      mov(get_register_op32(rt), eax);
   }
   else if (immediate < 0 && -immediate <= 128)
   {
      // using 'add' with sub means we can go to -128
      // 8B 42 DD 83 C0 79 89 42 EE 
      // mov dword eax, [rdx + 0xDD]        ; DD = 'rs' offset
      // add dword eax, 0x79                ; 79 = immediate
      // mov dword [rdx + 0xEE], eax        ; EE = 'rt' offset
      mov(eax, get_register_op32(rs));
      add(eax, int8(immediate));
      jo("intrinsic_ov_ex", T_NEAR);
      mov(get_register_op32(rt), eax);
   }
   else
   {
      // this is a legitimate addiu
      // 8B 42 DD 05 FF FF FF FF 89 42 EE 
      // mov dword eax, [rdx + 0xDD] ; DD = 'rs' offset
      // add dword eax, 0xFFFFFFFF       ; FFFF = 16-bit immediate value
      // mov dword [rdx + 0xEE], eax ; EE = 'rt' offset
      mov(eax, get_register_op32(rs));
      add(eax, immediate);
      jo("intrinsic_ov_ex", T_NEAR);
      mov(get_register_op32(rt), eax);
   }
}

void Jit1_CodeGen::write_PROC_ADDU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs + rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0 && rd.get_register() == 0)
   {
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rt.get_register() == 0)
   {
      // move [rs] to [rd]
      mov(eax, get_register_op32(rs));
      mov(get_register_op32(rd), eax);
   }
   else if (rs.get_register() == 0)
   {
      // move [rt] to [rd]
      mov(eax, get_register_op32(rt));
      mov(get_register_op32(rd), eax);
   }
   else if (rs.get_register() == rd.get_register())
   {
      // move [rt] to [rd]
      mov(eax, get_register_op32(rt));
      add(get_register_op32(rd), eax);
   }
   else
   {
      // add [rs] and [rt] to [rd]
      mov(eax, get_register_op32(rs));
			if (rs.get_register() == rt.get_register()) {
				add(eax, eax);
			}
			else {
				add(eax, get_register_op32(rt));
			}
      mov(get_register_op32(rd), eax);
   }
}

void Jit1_CodeGen::write_PROC_ADD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs + rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0 && rd.get_register() == 0)
   {
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rt.get_register() == 0)
   {
      // move [rs] to [rd]
      mov(eax, get_register_op32(rs));
      mov(get_register_op32(rd), eax);
   }
   else if (rs.get_register() == 0)
   {
      // move [rt] to [rd]
      mov(eax, get_register_op32(rt));
      mov(get_register_op32(rd), eax);
   }
   else
   {
      // add [rs] and [rt] to [rd]
      mov(eax, get_register_op32(rs));
			if (rs.get_register() == rt.get_register()) {
				add(eax, eax);
			}
			else {
				add(eax, get_register_op32(rt));
			}
      jo("intrinsic_ov_ex", T_NEAR);
      mov(get_register_op32(rd), eax);
   }
}

void Jit1_CodeGen::write_PROC_AUI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs + rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   int32 immediate = instructions::TinyInt<32>(instruction << 16).sextend<int32>();

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));


   if (rt.get_register() == 0)
   {
      // nop
   }
   else if (immediate == 0)
   {
      // move [rs] to [rt]
      mov(eax, get_register_op32(rs));
      mov(get_register_op32(rt), eax);
   }
   else if (immediate < 0)
   {
      if (immediate >= -128)
      {
         mov(eax, get_register_op32(rs));
         add(eax, int8(immediate));
         mov(get_register_op32(rt), eax);
      }
      else
      {
         mov(eax, get_register_op32(rs));
         add(eax, immediate);
         mov(get_register_op32(rt), eax);
      }
   }
   else
   {
      immediate = -immediate;
      if (immediate >= -128)
      {
         mov(eax, get_register_op32(rs));
         sub(eax, int8(immediate));
         mov(get_register_op32(rt), eax);
      }
      else
      {
         mov(eax, get_register_op32(rs));
         sub(eax, immediate);
         mov(get_register_op32(rt), eax);
      }
   }
}

void Jit1_CodeGen::write_PROC_SLT(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0 || rt.get_register() == rs.get_register())
   {
      mov(get_register_op32(rd), 0);
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

void Jit1_CodeGen::write_PROC_SLTU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0 || rt.get_register() == rs.get_register())
   {
      mov(get_register_op32(rd), 0);
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

void Jit1_CodeGen::write_PROC_SLTI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

   const int32 immediate = instructions::TinyInt<16>(instruction).sextend<int32>();

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   if (rt.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == 0 && immediate == 0)
   {
      mov(get_register_op32(rt), 0);
   }
   else if (rs.get_register() == 0 && immediate > 0)
   {
      mov(get_register_op32(rt), 1);
   }
   else
   {
      xor_(ecx, ecx);
      cmp(get_register_op32(rs), immediate);
      setl(cl);
      mov(get_register_op32(rt), ecx);
   }
}

void Jit1_CodeGen::write_PROC_SLTIU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

   const uint32 immediate = uint32(instructions::TinyInt<16>(instruction).sextend<int32>()); // intended

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   if (rt.get_register() == 0)
   {
      // nop
   }
   else if (immediate == 0)
   {
      mov(get_register_op32(rt), 0);
   }
   else if (rs.get_register() == 0 && immediate > 0)
   {
      mov(get_register_op32(rt), 1);
   }
   else
   {
      xor_(ecx, ecx);
      cmp(get_register_op32(rs), immediate);
      setb(cl);
      mov(get_register_op32(rt), ecx);
   }
}

void Jit1_CodeGen::write_COP1_MFC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   static const int16 fp_offset = sword_assert(offset_of(&coprocessor1::m_registers) - 128);
   const instructions::FPRegister<11, 5> fs(instruction, (mips::coprocessor1 & __restrict)*m_jit.m_processor.get_coprocessor(1));
   const int16 fs_offset = sword_assert(fp_offset + (8 * fs.get_register()));

   if (rt.get_register() == 0)
   {
      // nop
   }
   else
   {
      mov(eax, dword[r12 + fs_offset]);
      mov(get_register_op32(rt), eax);
   }
}

void Jit1_CodeGen::write_COP1_MTC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   static const int16 fp_offset = sword_assert(offset_of(&coprocessor1::m_registers) - 128);
   const instructions::FPRegister<11, 5> fs(instruction, (mips::coprocessor1 & __restrict)*m_jit.m_processor.get_coprocessor(1));
   const int16 fs_offset = sword_assert(fp_offset + (8 * fs.get_register()));

   if (rt.get_register() == 0)
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

void Jit1_CodeGen::write_COP1_MFHC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   static const int16 fp_offset = sword_assert(offset_of(&coprocessor1::m_registers) - 128);
   const instructions::FPRegister<11, 5> fs(instruction, (mips::coprocessor1 & __restrict)*m_jit.m_processor.get_coprocessor(1));
   const int16 fs_offset = sword_assert(fp_offset + (8 * fs.get_register()));

   if (rt.get_register() == 0)
   {
      // nop
   }
   else
   {
      mov(eax, dword[r12 + (fs_offset + 4)]);
      mov(get_register_op32(rt), eax);
   }
}

void Jit1_CodeGen::write_COP1_MTHC1(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   static const int16 fp_offset = sword_assert(offset_of(&coprocessor1::m_registers) - 128);
   const instructions::FPRegister<11, 5> fs(instruction, (mips::coprocessor1 & __restrict)*m_jit.m_processor.get_coprocessor(1));
   const int16 fs_offset = sword_assert(fp_offset + (8 * fs.get_register()));

   if (rt.get_register() == 0)
   {
      // write 0 to [fs]
      mov(dword[r12 + fs_offset + 4], 0);
   }
   else
   {
      mov(eax, get_register_op32(rt));
      mov(dword[r12 + (fs_offset + 4)], eax);
   }
}

void Jit1_CodeGen::write_COP1_SEL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int16 fp_offset = sword_assert(offset_of(&coprocessor1::m_registers) - 128);

   const instructions::FPRegister<16, 5> ft{instruction, (mips::coprocessor1 & __restrict)*m_jit.m_processor.get_coprocessor(1)};
   const instructions::FPRegister<11, 5> fs{instruction, (mips::coprocessor1 & __restrict)*m_jit.m_processor.get_coprocessor(1)};
   const instructions::FPRegister<6, 5> fd{instruction, (mips::coprocessor1 & __restrict)*m_jit.m_processor.get_coprocessor(1)};

   const int16 ft_offset = sword_assert(fp_offset + (8 * ft.get_register()));
   const int16 fs_offset = sword_assert(fp_offset + (8 * fs.get_register()));
   const int16 fd_offset = sword_assert(fp_offset + (8 * fd.get_register()));

   if (instruction_info.Type == 'f')
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

void Jit1_CodeGen::write_PROC_MUL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs * rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0 || rd.get_register() == 0)
   {
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == rd.get_register())
   {
      // move [rt] to [rd]
      mov(eax, get_register_op32(rt));
      imul(get_register_op32(rd));
      mov(get_register_op32(rd), eax);
   }
	 else if (rt.get_register() == rd.get_register())
	 {
		 // move [rt] to [rd]
		 mov(eax, get_register_op32(rs));
		 imul(get_register_op32(rd));
		 mov(get_register_op32(rd), eax);
	 }
   else
   {
      // add [rs] and [rt] to [rd]
      mov(eax, get_register_op32(rs));
			if (rs.get_register() == rt.get_register()) {
				imul(eax);
			}
			else {
				imul(get_register_op32(rt));
			}
      mov(get_register_op32(rd), eax);
   }
}

void Jit1_CodeGen::write_PROC_MULU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs * rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0 || rd.get_register() == 0)
   {
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
	 else if (rs.get_register() == rd.get_register())
	 {
		 // move [rt] to [rd]
		 mov(eax, get_register_op32(rt));
		 mul(get_register_op32(rd));
		 mov(get_register_op32(rd), eax);
	 }
	 else if (rt.get_register() == rd.get_register())
	 {
		 // move [rt] to [rd]
		 mov(eax, get_register_op32(rs));
		 mul(get_register_op32(rd));
		 mov(get_register_op32(rd), eax);
	 }
   else
   {
      // add [rs] and [rt] to [rd]
      mov(eax, get_register_op32(rs));
			if (rs.get_register() == rt.get_register()) {
				mul(eax);
			}
			else {
				mul(get_register_op32(rt));
			}
      mov(get_register_op32(rd), eax);
   }
}

void Jit1_CodeGen::write_PROC_MUH(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs * rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0 || rd.get_register() == 0)
   {
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == rd.get_register())
   {
      // move [rt] to [rd]
      mov(eax, get_register_op32(rt));
      imul(get_register_op32(rd));
      mov(get_register_op32(rd), edx);
   }
	 else if (rt.get_register() == rd.get_register())
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
			if (rs.get_register() == rt.get_register()) {
				imul(eax);
			}
			else {
				imul(get_register_op32(rt));
			}
      mov(get_register_op32(rd), edx);
   }
}

void Jit1_CodeGen::write_PROC_MUHU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs * rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0 || rd.get_register() == 0)
   {
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == rd.get_register())
   {
      // move [rt] to [rd]
      mov(eax, get_register_op32(rt));
      mul(get_register_op32(rd));
      mov(get_register_op32(rd), edx);
   }
	 else if (rt.get_register() == rd.get_register())
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
			if (rs.get_register() == rt.get_register()) {
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

void Jit1_CodeGen::write_PROC_DIV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs * rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   const auto divzero = GetUniqueLabel();
   // [rd] = [rs] / [rt]

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      // divzero
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0)
   {
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == rd.get_register())
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
	 else if (rs.get_register() == rt.get_register())
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

void Jit1_CodeGen::write_PROC_DIVU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs * rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   const auto divzero = GetUniqueLabel();
   // [rd] = [rs] / [rt]

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      // divzero
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0)
   {
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == rd.get_register())
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
	 else if (rs.get_register() == rt.get_register())
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

void Jit1_CodeGen::write_PROC_MOD(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs * rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   const auto divzero = GetUniqueLabel();
   // [rd] = [rs] / [rt]

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      // divzero
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0)
   {
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == rd.get_register())
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
	 else if (rs.get_register() == rt.get_register())
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

void Jit1_CodeGen::write_PROC_MODU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs * rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   const auto divzero = GetUniqueLabel();
   // [rd] = [rs] / [rt]

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      // divzero
      // set [rd] to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0)
   {
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == rd.get_register())
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
	 else if (rs.get_register() == rt.get_register())
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

void Jit1_CodeGen::write_PROC_XOR(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs | rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == rt.get_register())
   {
      // set rd to 0
      // 89 4A EE
      // mov dword [rdx + 0xEE], ecx ; EE = 'rd' offset
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0)
   {
      mov(eax, get_register_op32(rt));
      mov(get_register_op32(rd), eax);
   }
   else if (rt.get_register() == 0)
   {
      mov(eax, get_register_op32(rs));
      mov(get_register_op32(rd), eax);
   }
   else
   {
		 if (rd.get_register() == rt.get_register()) {
			 mov(eax, get_register_op32(rs));
			 xor_(get_register_op32(rd), eax);
		 }
		 else if (rd.get_register() == rs.get_register()) {
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

void Jit1_CodeGen::write_PROC_XORI(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   // rd = rs | rt
   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const uint32 offset = instructions::TinyInt<16>(instruction).zextend<uint32>();

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   if (rt.get_register() == 0)
   {
      // nop
   }
   else if (rs.get_register() == 0)
   {
      // set rt to offset
      if (offset == 0)
      {
         mov(get_register_op32(rt), int8(0));
      }
      else
      {
         mov(get_register_op32(rt), int32(offset));
      }
   }
   else if (offset == 0)
   {
		 if (rs.get_register() != rt.get_register()) {
			 // just move rs to rt
			 mov(eax, get_register_op32(rs));
			 mov(get_register_op32(rt), eax);
		 }
   }
   else
   {
      // actually perform OR
		 if (rt.get_register() == rs.get_register()) {
			 xor_(get_register_op32(rt), int32(offset));
		 }
		 else {
			 mov(eax, get_register_op32(rs));
			 xor_(eax, int32(offset));
			 mov(get_register_op32(rt), eax);
		 }
   }
}

void Jit1_CodeGen::write_PROC_SEB(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      mov(get_register_op32(rd), int8(0));
   }
   else
   {
      movsx(eax, byte[rbp + rt_offset]);
      mov(get_register_op32(rd), eax);
   }
}

void Jit1_CodeGen::write_PROC_SEH(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      mov(get_register_op32(rd), int16(0));
   }
   else
   {
      movsx(eax, word[rbp + rt_offset]);
      mov(get_register_op32(rd), eax);
   }
}

void Jit1_CodeGen::write_PROC_SLL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);
   const int8 sa = (int8)instructions::TinyInt<5>(instruction >> 6).zextend<uint32>();

   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      mov(get_register_op32(rd), int16(0));
   }
   else if (rd.get_register() == rt.get_register())
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
      if (sa == 0)
      {
         // move
         mov(eax, get_register_op32(rt));
         mov(get_register_op32(rd), eax);
      }
      else
      {
         mov(eax, get_register_op32(rt));
         shl(eax, sa);
         mov(get_register_op32(rd), eax);
      }
   }
}

void Jit1_CodeGen::write_PROC_SRL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);
   const int8 sa = (int8)instructions::TinyInt<5>(instruction >> 6).zextend<uint32>();

   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      mov(get_register_op32(rd), int16(0));
   }
   else if (rd.get_register() == rt.get_register())
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
      if (sa == 0)
      {
         // move
         mov(eax, get_register_op32(rt));
         mov(get_register_op32(rd), eax);
      }
      else
      {
         mov(eax, get_register_op32(rt));
         shr(eax, sa);
         mov(get_register_op32(rd), eax);
      }
   }
}

void Jit1_CodeGen::write_PROC_SRA(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);
   const int8 sa = (int8)instructions::TinyInt<5>(instruction >> 6).zextend<uint32>();

   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));
   const int8 rd_offset = sbyte_assert(gp_offset + (4 * rd.get_register()));

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      mov(get_register_op32(rd), int16(0));
   }
   else if (rd.get_register() == rt.get_register())
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
      if (sa == 0)
      {
         // move
         mov(eax, get_register_op32(rt));
         mov(get_register_op32(rd), eax);
      }
      else
      {
         mov(eax, get_register_op32(rt));
         sar(eax, sa);
         mov(get_register_op32(rd), eax);
      }
   }
}

void Jit1_CodeGen::write_PROC_SLLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      // set rd to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0)
   {
      // move rt to rd
      mov(eax, get_register_op32(rt));
      mov(get_register_op32(rd), eax);
   }
   else if (rs.get_register() == rt.get_register())
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

void Jit1_CodeGen::write_PROC_SRLV(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
   const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

   if (rd.get_register() == 0)
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      // set rd to 0.
      mov(get_register_op32(rd), 0);
   }
   else if (rs.get_register() == 0)
   {
      // move rt to rd
      mov(eax, get_register_op32(rt));
      mov(get_register_op32(rd), eax);
   }
   else if (rs.get_register() == rt.get_register())
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

void Jit1_CodeGen::write_PROC_SYNC(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   // do nothing
}


void Jit1_CodeGen::write_PROC_RDHWR(jit1::ChunkOffset & __restrict chunk_offset, bool& terminate_instruction, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
	static const int8 uv_offset =  sbyte_assert(offset_of(&processor::m_user_value) - 128);

	const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
	const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);
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
	jmp("intrinsic_ri_ex", T_NEAR);
}

void Jit1_CodeGen::write_PROC_EXT(jit1::ChunkOffset& __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo& __restrict instruction_info) __restrict
{
	const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
	const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

	auto& rs_reg = get_register_op32(rs);
	auto& rt_reg = get_register_op32(rt);

	const uint32 msbd = instructions::TinyInt<5>(instruction >> 11).zextend<uint32>();
	const uint32 lsb = instructions::TinyInt<5>(instruction >> 6).zextend<uint32>();

	if (rt.get_register() == 0) {
		// nop
	}
	else {
		if (lsb + msbd > 31) {
			// Result is unpredictable, just push -1.
			mov(rt_reg, -1);
		}
		else if (rs.get_register() == 0) {
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
