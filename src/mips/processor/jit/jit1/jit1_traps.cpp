#include "pch.hpp"

#include "jit1.hpp"
#include "../../processor.hpp"
#include "coprocessor/coprocessor1/coprocessor1.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "instructions/coprocessor1_support.hpp"
#include <cassert>
#include "codegen.hpp"
#include "mips/system.hpp"

using namespace mips;

// TODO consider implementing code properly.

void Jit1_CodeGen::write_PROC_TEQ(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

   // tr ? [rs] == [rt]
   mov(eax, int32(code));
   if (rs.get_register() == rt.get_register())
   {
      jmp("intrinsic_tr_ex", T_NEAR);
   }
   else
   {
      mov(ecx, get_register_op32(rs));
      cmp(ecx, get_register_op32(rt));
      je("intrinsic_tr_ex", T_NEAR);
   }
}

void Jit1_CodeGen::write_PROC_TGE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

   // tr ? [rs] >= [rt]
   mov(eax, int32(code));
   if (rs.get_register() == rt.get_register())
   {
      jmp("intrinsic_tr_ex", T_NEAR);
   }
   else
   {
      mov(ecx, get_register_op32(rs));
      cmp(ecx, get_register_op32(rt));
      jge("intrinsic_tr_ex", T_NEAR);
   }
}

void Jit1_CodeGen::write_PROC_TGEU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

   // tr ? [rs] >= [rt]
   mov(eax, int32(code));
   if (rs.get_register() == rt.get_register())
   {
      jmp("intrinsic_tr_ex", T_NEAR);
   }
   else
   {
      mov(ecx, get_register_op32(rs));
      cmp(ecx, get_register_op32(rt));
      jae("intrinsic_tr_ex", T_NEAR);
   }
}

void Jit1_CodeGen::write_PROC_TLT(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

   // tr ? [rs] < [rt]
   mov(eax, int32(code));
   if (rs.get_register() == rt.get_register())
   {
      // nop
   }
   else
   {
      mov(ecx, get_register_op32(rs));
      cmp(ecx, get_register_op32(rt));
      jl("intrinsic_tr_ex", T_NEAR);
   }
}

void Jit1_CodeGen::write_PROC_TLTU(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

   // tr ? [rs] < [rt]
   mov(eax, int32(code));
   if (rs.get_register() == rt.get_register())
   {
      // nop
   }
   else if (rt.get_register() == 0)
   {
      // nop (impossible - rs !< 0
   }
   else
   {
      mov(ecx, get_register_op32(rs));
      cmp(ecx, get_register_op32(rt));
      jl("intrinsic_tr_ex", T_NEAR);
   }
}

void Jit1_CodeGen::write_PROC_TNE(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);

   const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
   const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);

   const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
   const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

   const uint32 code = instructions::TinyInt<10>(instruction >> 6).zextend<uint32>();

   // tr ? [rs] != [rt]
   mov(eax, int32(code));
   if (rs.get_register() == rt.get_register())
   {
      // nop
   }
   else
   {
      mov(ecx, get_register_op32(rs));
      cmp(ecx, get_register_op32(rt));
      jne("intrinsic_tr_ex", T_NEAR);
   }
}

// syscall
namespace
{
   static uint32 do_syscall(uint32 code, processor * __restrict proc)
   {
      const CPU_Exception sys_ex{ CPU_Exception::Type::Sys, proc->get_program_counter(), code };
      return proc->get_guest_system()->handle_exception(sys_ex);
   }
}

void Jit1_CodeGen::write_PROC_SYSCALL(jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   static const int8 pc_offset = sbyte_assert(offset_of(&processor::m_program_counter) - 128);
   static const int8 ic_offset = sbyte_assert(offset_of(&processor::m_instruction_count) - 128);
   static const int8 flags_offset = sbyte_assert(offset_of(&processor::m_flags) - 128);

   const uint32 code = instructions::TinyInt<20>(instruction >> 6).zextend<uint32>();

   // tr ? [rs] != [rt]
   mov(rax, uint64(do_syscall));
   mov(qword[rbp + ic_offset], rdi);
   mov(dword[rbp + pc_offset], int32(address));
   mov(dword[rbp + flags_offset], ebx);
   mov(ecx, int32(code));
   mov(rdx, rbp);
   add(rdx, -128);
   call(rax);
   mov(ebx, dword[rbp + flags_offset]);
   test(eax, eax);
   jnz("save_return", T_NEAR);
}

