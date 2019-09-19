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

extern jit1::jit_instructionexec_t jit1_get_instruction(jit1 * __restrict _this, uint32 address);
static constexpr uint32 MaxShortJumpLookAhead = 2;

#define IS_INSTRUCTION(instr, ref) \
	[&]() -> bool { \
		extern const mips::instructions::InstructionInfo StaticProc_ ## ref; \
		return &StaticProc_ ## ref == &instr; \
	}()

bool Jit1_CodeGen::write_delay_branch(bool &terminate_instruction, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   const uint32 this_address = address;

   const uint32 chunk_begin = this_address & ~(jit1::ChunkSize - 1);
   const uint32 chunk_last = chunk_begin + (jit1::ChunkSize - 1);
   const uint32 next_chunk = chunk_last + 1;
   const uint32 this_offset = (this_address - chunk_begin) / 4u;

   static const int8 flags_offset = sbyte_assert(offset_of(&processor::m_flags) - 128);
   static const int8 dbt_offset =  sbyte_assert(offset_of(&processor::m_branch_target) - 128);
   static const int8 pc_offset = sbyte_assert(offset_of(&processor::m_program_counter) - 128);
   static const int8 ic_offset = sbyte_assert(offset_of(&processor::m_instruction_count) - 128);
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);
   static const int8 r31 = gp_offset + (31 * 4);

   const auto no_jump = GetUniqueLabel();

   if (IS_INSTRUCTION(instruction_info, COP1_BC1EQZ_v))
   {
      static const int16 fp_offset = sword_assert(offset_of(&coprocessor1::m_registers) - 128);
      const instructions::FPRegister<16, 5> ft(instruction, (mips::coprocessor1 & __restrict)*m_jit.m_processor.get_coprocessor(1));
      const int16 ft_offset = sword_assert(fp_offset + (8 * ft.get_register()));

      const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();

      uint32 target_address = address + 4 + offset;

      test(dword[r12 + ft_offset], 1); // ZF set to 1 if [ft] & 1 == 0
      jnz(no_jump);
      mov(esi, int32(target_address));
      or_(ebx, processor::flag_bit(processor::flag::branch_delay));
      L(no_jump);
      or_(ebx, processor::flag_bit(processor::flag::no_cti));
   }
   else if (IS_INSTRUCTION(instruction_info, COP1_BC1NEZ_v))
   {
      static const int16 fp_offset = sword_assert(offset_of(&coprocessor1::m_registers) - 128);
      const instructions::FPRegister<16, 5> ft(instruction, (mips::coprocessor1 & __restrict)*m_jit.m_processor.get_coprocessor(1));
      const int16 ft_offset = sword_assert(fp_offset + (8 * ft.get_register()));

      const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();

      const uint32 target_address = address + 4 + offset;

      test(dword[r12 + ft_offset], 1); // ZF set to 1 if [ft] & 1 == 0
      jz(no_jump);
      mov(esi, int32(target_address));
      or_(ebx, processor::flag_bit(processor::flag::branch_delay));
      L(no_jump);
      or_(ebx, processor::flag_bit(processor::flag::no_cti));
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BAL))
   {
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

      const uint32 target_address = address + 4 + immediate;
      const uint32 link_address = address + 8;

      mov(dword[rbp + r31], int32(link_address));
      mov(esi, int32(target_address));
      or_(ebx, processor::flag_bit(processor::flag::branch_delay));
      or_(ebx, processor::flag_bit(processor::flag::no_cti));
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BEQ))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
      const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

      const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
      const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

      const uint32 target_address = address + 4 + immediate;

      if (rs.get_register() == rt.get_register())
      {
         mov(esi, int32(target_address)); // 0 == 0
         or_(ebx, processor::flag_bit(processor::flag::branch_delay) | processor::flag_bit(processor::flag::no_cti));
      }
      else
      {
         if (rs.get_register() == 0)
         {
            cmp(get_register_op32(rt), 0);
         }
         else if (rt.get_register() == 0)
         {
            cmp(get_register_op32(rs), 0);
         }
         else
         {
            mov(eax, get_register_op32(rs));
            cmp(eax, get_register_op32(rt));
         }
         jne(no_jump);
         mov(esi, int32(target_address));
         or_(ebx, processor::flag_bit(processor::flag::branch_delay));
         L(no_jump);
         or_(ebx, processor::flag_bit(processor::flag::no_cti));
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BGEZ))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

      const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));

      const uint32 target_address = address + 4 + immediate;

      if (rs.get_register() == 0) // 0 >= 0
      {
         mov(esi, int32(target_address));
         or_(ebx, processor::flag_bit(processor::flag::branch_delay) | processor::flag_bit(processor::flag::no_cti));
      }
      else
      {
         cmp(get_register_op32(rs), 0);
         jl(no_jump);
         mov(esi, int32(target_address));
         or_(ebx, processor::flag_bit(processor::flag::branch_delay));
         L(no_jump);
         or_(ebx, processor::flag_bit(processor::flag::no_cti));
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BGTZ))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

      const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));

      const uint32 target_address = address + 4 + immediate;

      if (rs.get_register() == 0) // 0 !> 0
      {
         //nop
      }
      else
      {
         cmp(get_register_op32(rs), 0);
         jle(no_jump);
         mov(esi, int32(target_address));
         or_(ebx, processor::flag_bit(processor::flag::branch_delay));
         L(no_jump);
      }
      or_(ebx, processor::flag_bit(processor::flag::no_cti));
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BLEZ))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

      const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));

      const uint32 target_address = address + 4 + immediate;

      if (rs.get_register() == 0) // 0 <= 0
      {
         mov(esi, int32(target_address));
         or_(ebx, processor::flag_bit(processor::flag::branch_delay) | processor::flag_bit(processor::flag::no_cti));
      }
      else
      {
         cmp(get_register_op32(rs), 0);
         jg(no_jump);
         mov(esi, int32(target_address));
         or_(ebx, processor::flag_bit(processor::flag::branch_delay));
         L(no_jump);
         or_(ebx, processor::flag_bit(processor::flag::no_cti));
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BLTZ))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

      const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));

      const uint32 target_address = address + 4 + immediate;

      if (rs.get_register() == 0) // 0 !< 0
      {
         //nop
      }
      else
      {
         cmp(get_register_op32(rs), 0);
         jge(no_jump);
         mov(esi, int32(target_address));
         or_(ebx, processor::flag_bit(processor::flag::branch_delay));
         L(no_jump);
      }
      or_(ebx, processor::flag_bit(processor::flag::no_cti));
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BNE))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
      const instructions::GPRegister<16, 5> rt(instruction, m_jit.m_processor);
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();

      const int8 rs_offset = sbyte_assert(gp_offset + (4 * rs.get_register()));
      const int8 rt_offset = sbyte_assert(gp_offset + (4 * rt.get_register()));

      const uint32 target_address = address + 4 + immediate;

      if (rs.get_register() == rt.get_register())
      {
         //nop
      }
      else
      {
         if (rs.get_register() == 0)
         {
            cmp(get_register_op32(rt), 0);
         }
         else if (rt.get_register() == 0)
         {
            cmp(get_register_op32(rs), 0);
         }
         else
         {
            mov(eax, get_register_op32(rs));
            cmp(eax, get_register_op32(rt));
         }
         je(no_jump);
         mov(esi, int32(target_address));
         or_(ebx, processor::flag_bit(processor::flag::branch_delay));
         L(no_jump);
      }
      or_(ebx, processor::flag_bit(processor::flag::no_cti));
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_J))
   {
      const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();

      const uint32 target_address = (address & instructions::HighBits(4)) | instr_index;

      mov(esi, int32(target_address));
      or_(ebx, processor::flag_bit(processor::flag::branch_delay) | processor::flag_bit(processor::flag::no_cti));
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_JAL))
   {
      const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();

      const uint32 target_address = (address & instructions::HighBits(4)) | instr_index;

      const uint32 link_address = address + 8;

      mov(dword[rbp + r31], int32(link_address));
      mov(esi, int32(target_address));
      or_(ebx, processor::flag_bit(processor::flag::branch_delay) | processor::flag_bit(processor::flag::no_cti));
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_JALR))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
      const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

      const uint32 link_address = address + 8;

      if (rd.get_register() != 0)
      {
         mov(dword[rbp + (int64(gp_offset) + int64(rd.get_register()) * 4)], int32(link_address));
      }
      mov(esi, dword[rbp + (gp_offset + int64(rs.get_register()) * 4)]);
      or_(ebx, processor::flag_bit(processor::flag::branch_delay) | processor::flag_bit(processor::flag::no_cti));
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_JR))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);
      const instructions::GPRegister<11, 5> rd(instruction, m_jit.m_processor);

      mov(esi, dword[rbp + (int64(gp_offset) + int64(rs.get_register()) * 4)]);
      or_(ebx, processor::flag_bit(processor::flag::branch_delay) | processor::flag_bit(processor::flag::no_cti));
   }
   else
   {
      assert(false);
      insert_procedure_ecx(address, uint64(instruction_info.Proc), instruction, instruction_info);
      return true;
   }

   return false;
}

// enum class branch_type : uint32
// {
//    near_branch = 0,        // Branches within this chunk         
//    far_branch,             // Branches outside this chunk
//    indeterminate           // Branches to an unknown location
// };
void Jit1_CodeGen::handle_delay_branch(jit1::Chunk & __restrict chunk, jit1::ChunkOffset & __restrict chunk_offset, uint32 address, instruction_t instruction, const mips::instructions::InstructionInfo & __restrict instruction_info) __restrict
{
   branch_type branchtype;
   uint32 target_address = 0;

   const uint32 this_address = address + 4;

   const uint32 chunk_begin = this_address & ~(jit1::ChunkSize - 1);
   const uint32 chunk_last = chunk_begin + (jit1::ChunkSize - 1);
   const uint32 next_chunk = chunk_last + 1;
   const uint32 this_offset = (this_address - chunk_begin) / 4u;

   static const int8 flags_offset = sbyte_assert(offset_of(&processor::m_flags) - 128);
   static const int8 dbt_offset =  sbyte_assert(offset_of(&processor::m_branch_target) - 128);
   static const int8 pc_offset = sbyte_assert(offset_of(&processor::m_program_counter) - 128);
   static const int8 ic_offset = sbyte_assert(offset_of(&processor::m_instruction_count) - 128);
   static const int8 gp_offset = sbyte_assert(offset_of(&processor::m_registers) - 128);
   static const int8 r31 = gp_offset + (31 * 4);

   const auto patch_preprolog = [&]() -> std::string
   {
      // If execution gets past the chunk, we jump to the next chunk.
      // Start with a set of nops so that we have somewhere to write patch code.
      const std::string patch = GetUniqueLabel();
      L(patch); // patch should be 12 bytes. Enough to copy an 8B pointer to rax, and then to jump to it.
      chunk.m_patches.push_back({ uint32(getSize()), 0 });
      auto &patch_pair = chunk.m_patches.back();
      uint32 &patch_target = patch_pair.target;

      // no-op patch
      for (uint8 octet : { 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x00 }) db(octet);

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

   const auto patch_epilog = [&](const std::string &patch)
   {
      static constexpr uint16 patch_prefix = 0xB848;
      static constexpr uint16 patch_suffix = 0xE0FF;

      mov(rcx, patch.c_str());
      mov(word[rcx], int16_t(patch_prefix));
      mov(qword[rcx + 2], rax);
      mov(word[rcx + 10], int16_t(patch_suffix));
   };

   const auto safejmp = [&](const std::string &target_label, uint32 instruction_offset)
   {
      if ((instruction_offset <= this_offset && (chunk_offset[this_offset] - chunk_offset[instruction_offset]) <= 128) || (instruction_offset - this_offset) <= MaxShortJumpLookAhead)
      {
         jmp(target_label);
      }
      else
      {
         jmp(target_label, T_NEAR);
      }
   };

   if (IS_INSTRUCTION(instruction_info, COP1_BC1EQZ_v))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();
      target_address = address + 4 + offset;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, COP1_BC1NEZ_v))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const int32 offset = instructions::TinyInt<18>(instruction << 2U).sextend<int32>();
      target_address = address + 4 + offset;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BAL))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
      target_address = address + 4 + immediate;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BEQ))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
      target_address = address + 4 + immediate;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BGEZ))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const int32 immediate = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
      target_address = address + 4 + immediate;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BGTZ))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
      target_address = address + 4 + offset;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BLEZ))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
      target_address = address + 4 + offset;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BLTZ))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
      target_address = address + 4 + offset;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_BNE))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const int32 offset = instructions::TinyInt<18>(instruction << 2).sextend<int32>();
      target_address = address + 4 + offset;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_J))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();
      target_address =  (address & instructions::HighBits(4)) | instr_index;;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_JAL))
   {
      const coprocessor1 & __restrict cop1 = *(const coprocessor1 * __restrict)m_jit.m_processor.get_coprocessor(1);
      const uint32 instr_index = instructions::TinyInt<28>(instruction << 2).zextend<uint32>();
      target_address =  (address & instructions::HighBits(4)) | instr_index;;

      if (chunk_begin <= target_address && chunk_last >= target_address)
      {
         branchtype = branch_type::near_branch;
      }
      else
      {
         branchtype = branch_type::far_branch;
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_JALR))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);

      if (rs.get_register() != 0)
      {
         branchtype = branch_type::indeterminate;
      }
      else
      {
         target_address = 0;

         if (chunk_begin <= target_address && chunk_last >= target_address)
         {
            branchtype = branch_type::near_branch;
         }
         else
         {
            branchtype = branch_type::far_branch;
         }
      }
   }
   else if (IS_INSTRUCTION(instruction_info, PROC_JR))
   {
      const instructions::GPRegister<21, 5> rs(instruction, m_jit.m_processor);

      if (rs.get_register() != 0)
      {
         branchtype = branch_type::indeterminate;
      }
      else
      {
         target_address = 0;

         if (chunk_begin <= target_address && chunk_last >= target_address)
         {
            branchtype = branch_type::near_branch;
         }
         else
         {
            branchtype = branch_type::far_branch;
         }
      }
   }
   else
   {
      assert(false);
      branchtype = branch_type::indeterminate_unhandled;
   }

   switch (branchtype)
   {
      case branch_type::near_branch:                  // Branches within this chunk
      {
         const auto no_branch = GetUniqueLabel();
         test(ebx, processor::flag_bit(processor::flag::branch_delay));
         je(no_branch);
         and_(ebx, ~processor::flag_bit(processor::flag::branch_delay));
         // what is the offset of the target address?
         const uint32 target_offset = (target_address - chunk_begin) / 4u;
         const auto target_label = GetIndexLabel(target_offset);
         xor_(esi, esi);
         safejmp(target_label, target_offset);
         L(no_branch);
      } break;
      case branch_type::far_branch:                   // Branches outside this chunk
      {
         const auto no_branch = GetUniqueLabel();
         test(ebx, processor::flag_bit(processor::flag::branch_delay));
         je(no_branch);
         xor_(esi, esi);
         and_(ebx, ~processor::flag_bit(processor::flag::branch_delay));
         const auto patch = patch_preprolog();

         mov(edx, int32(target_address));

         patch_prolog();

         mov(rax, uint64(jit1_get_instruction));
         mov(rcx, uint64(&m_jit));
         call(rax);
         patch_epilog(patch);
         jmp(rax);

         L(no_branch);
      } break;
      case branch_type::indeterminate:                // Branches to an unknown location
      {
         const auto no_branch = GetUniqueLabel();
         const auto not_within = GetUniqueLabel();

         test(ebx, processor::flag_bit(processor::flag::branch_delay));
         je(no_branch);
         and_(ebx, ~processor::flag_bit(processor::flag::branch_delay));
         mov(eax, esi);
         xor_(esi, esi);

         mov(ecx, eax);
         and_(ecx, ~(jit1::ChunkSize - 1));
         cmp(ecx, chunk_begin);
         jne(not_within);
         and_(eax, (jit1::ChunkSize - 1));

         mov(rcx, uint64(chunk_offset.data()));
         mov(eax, dword[rcx + rax]);
         mov(rcx, "chunk_start");
         add(rax, rcx);
         jmp(rax);
         L(not_within);
         mov(rdx, rax);
         mov(rax, uint64(jit1_get_instruction));
         mov(rcx, uint64(&m_jit));
         call(rax);
         jmp(rax);
         L(no_branch);
      } break;
      case branch_type::near_branch_unhandled:        // Branches within this chunk, use pc state   
      {
         const auto no_branch = GetUniqueLabel();
         test(ebx, processor::flag_bit(processor::flag::branch_delay));
         jz(no_branch);
         and_(ebx, ~processor::flag_bit(processor::flag::branch_delay));
         mov(eax, int32(target_address));
         mov(dword[rbp + pc_offset], eax);

         // what is the offset of the target address?
         const uint32 target_offset = (target_address - chunk_begin) / 4u;
         const auto target_label = GetIndexLabel(target_offset);

         safejmp(target_label, target_offset);
         L(no_branch);
      } break;
      case branch_type::far_branch_unhandled:         // Branches outside this chunk, use pc state
      {
         const auto no_branch = GetUniqueLabel();
         test(ebx, processor::flag_bit(processor::flag::branch_delay));
         jz(no_branch);
         and_(ebx, ~processor::flag_bit(processor::flag::branch_delay));
         mov(eax, int32(target_address));
         mov(dword[rbp + pc_offset], eax);

         mov(rdx, rax);
         mov(rax, uint64(jit1_get_instruction));
         mov(rcx, uint64(&m_jit));
         call(rax);
         jmp(rax);

         L(no_branch);
      } break;
      case branch_type::indeterminate_unhandled:      // Branches to an unknown location, use pc state
      {
         const auto no_branch = GetUniqueLabel();
         const auto not_within = GetUniqueLabel();

         test(ebx, processor::flag_bit(processor::flag::branch_delay));
         jz(no_branch);
         and_(ebx, ~processor::flag_bit(processor::flag::branch_delay));
         mov(eax, dword[rbp + dbt_offset]);
         mov(dword[rbp + pc_offset], eax);

         mov(ecx, eax);
         and_(ecx, ~(jit1::ChunkSize - 1));
         cmp(ecx, chunk_begin);
         jne(not_within);
         and_(eax, (jit1::ChunkSize - 1));

         mov(rcx, uint64(chunk_offset.data()));
         mov(eax, dword[rcx + rax]);
         mov(rcx, "chunk_start");
         add(rax, rcx);
         jmp(rax);
         L(not_within);
         mov(rdx, rax);
         mov(rax, uint64(jit1_get_instruction));
         mov(rcx, uint64(&m_jit));
         call(rax);
         jmp(rax);
         L(no_branch);
      } break;
   }
}
