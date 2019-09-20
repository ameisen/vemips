#include "pch.hpp"

#include "jit1.hpp"
#include "instructions/instructions.hpp"
#include "instructions/instructions_common.hpp"
#include "../../processor.hpp"
#include <cassert>
#include "codegen.hpp"

#define NOMINMAX 1
#include <Windows.h>

#include <mutex>

#include "system.hpp"

using namespace mips;

#define JIT_INSTRUCTION_SEPARATE 0
#define JIT_INSERT_IDENTIFIERS 0

#define IS_INSTRUCTION(instr, ref) \
	[&]() -> bool { \
		extern const mips::instructions::InstructionInfo StaticProc_ ## ref; \
		return &StaticProc_ ## ref == instr; \
	}()


// this can and should be global to all JITs.
namespace
{
   extern "C" uint64 jit1_springboard(uint64 instruction, uint64 processor, uint64 pc_runner, uint64, uint64, uint64);
   extern "C" void jit1_drop_signal();
   static char * g_global_exec_data = nullptr;
   static uint64 g_ex_return = 0;
}

namespace
{
   __forceinline void * allocate_executable(size_t size)
   {
      void * __restrict data = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
      return data;
   }

   __forceinline void free_executable(void *ptr)
   {
      VirtualFree(ptr, 0, MEM_RELEASE);
   }
}

__forceinline jit1::MapLevel1::~MapLevel1()
{
   for (auto *ptr : m_Data)
   {
      if (ptr)
      {
         free_executable(ptr);
      }
   }
}

__forceinline jit1::Chunk & __restrict jit1::MapLevel1::operator [] (uint32 idx) __restrict
{
   Chunk * __restrict result = m_Data[idx];
   if (!result)
   {
      result = (Chunk * __restrict)allocate_executable(sizeof(Chunk));
      new (result) Chunk;
      m_Data[idx] = result;

      m_DataOffsets[idx] = new ChunkOffset;
      memset(m_DataOffsets[idx]->data(), 0, m_DataOffsets[idx]->size() * sizeof(uint32));
   }
   return *result;
}

__forceinline jit1::ChunkOffset & __restrict jit1::MapLevel1::get_offsets(uint32 idx) __restrict
{
   return *m_DataOffsets[idx];
}

jit1::jit_instructionexec_t jit1_get_instruction(jit1 * __restrict _this, uint32 address)
{
   return _this->get_instruction(address);
}

jit1::jit_instructionexec_t jit1_fetch_instruction(jit1* __restrict _this, uint32 address)
{
	return _this->fetch_instruction(address);
}

namespace
{
   void RI_Exception (instruction_t, processor & __restrict _processor)
   {
      _processor.set_trapped_exception({ CPU_Exception::Type::RI, _processor.get_program_counter() });
   }

   void OV_Exception (uint32 code, processor & __restrict _processor)
   {
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

jit1::jit1(processor & __restrict _processor) : m_processor(_processor)
{
   // get around missing symbol due to __forceinline
   volatile const bool RunNever = false;
   if (RunNever)
   {
      execute_instruction(0);
   }

   if (!g_global_exec_data)
   {
      g_global_exec_data = (char *)allocate_executable(0x1000);

      Jit1_CodeGen cg{ *this, (uint8 *)g_global_exec_data, 0x1000 };
      {
         g_ex_return = uint64(g_global_exec_data + cg.getSize());

         static const int8 flags_offset = sbyte_assert(offset_of(&processor::m_flags) - 128);
         static const int8 pc_offset =  sbyte_assert(offset_of(&processor::m_program_counter) - 128);
         static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);
         static const int8 dbt_offset =  sbyte_assert(offset_of(&processor::m_branch_target) - 128);
         static const int8 ic_offset =  sbyte_assert(offset_of(&processor::m_instruction_count) - 128);

         cg.mov(cg.qword[cg.rbp + ic_offset], cg.rdi);      // save instruction count
         cg.mov(cg.dword[cg.rbp + flags_offset], cg.ebx);
         cg.mov(cg.dword[cg.rbp + dbt_offset], cg.esi);  // set it in the interpreter
         cg.mov(cg.dword[cg.rbp + (int64(gp_offset) + (Jit1_CodeGen::mips_fp * 4))], cg.r15d);
         cg.mov(cg.rax, int64(jit1_drop_signal));
         cg.jmp(cg.rax);
      }

      cg.ready();
   }
}

jit1::~jit1()
{
}


namespace
{
   // // RCX, mov'd from the machine code   // RDX       // R8
   uint64 JumpFunction(instruction_t i, processor & __restrict p, uint64 f, uint64 rspv)
   {
      const mips::instructions::instructionexec_t executable = mips::instructions::instructionexec_t(f);
      executable(i, p);
      return rspv + 4;
   }
}

struct bytes
{
   const uint8 * __restrict m_data;
   const size_t m_datasize;

   template <size_t N>
   bytes(const uint8(&data) [N]) :
      m_data(data),
      m_datasize(N)
   {
   }
};

struct base_fixup
{
   uint32 offset;
   uint64 chunk_address = 0;

   base_fixup(uint32 _offset) :
      offset(_offset) {}
};
template <typename T>
struct fixup : public base_fixup
{
   fixup(uint32 _offset) : base_fixup(_offset) {}
};


struct vector_wrapper
{
   std::vector<uint8> &m_data;
   std::vector<base_fixup> &m_fixups;

   template <typename T>
   vector_wrapper & operator << (const T &value) __restrict;

   template <typename U>
   vector_wrapper & operator << (const fixup<U> &value) __restrict;
};


template <typename T>
vector_wrapper & vector_wrapper::operator << (const T &value) __restrict
{
   size_t sz = m_data.size();
   m_data.resize(m_data.size() + sizeof(T));
   *(T *)(m_data.data() + sz) = value;
   return *this;
};

template<>
vector_wrapper & vector_wrapper::operator << <bytes> (const bytes &value) __restrict
{
   size_t sz = m_data.size();
   m_data.resize(m_data.size() + value.m_datasize);
   memcpy(m_data.data() + sz, value.m_data, value.m_datasize);
   return *this;
};

template <>
vector_wrapper & vector_wrapper::operator << <uint8> (const uint8 &value) __restrict
{
   m_data.push_back(value);
   return *this;
};

using instruction_pack = std::pair<uint8[32], size_t>;

template <>
vector_wrapper & vector_wrapper::operator << <instruction_pack> (const instruction_pack &value) __restrict
{
   size_t sz = m_data.size();
   m_data.resize(m_data.size() + value.second);
   memcpy(m_data.data() + sz, value.first, value.second);
   return *this;
};

template <typename U>
vector_wrapper & vector_wrapper::operator << (const fixup<U> &value) __restrict
{
   fixup<U> _value = value;
   _value.chunk_address = m_data.size();
   m_fixups.push_back(_value);
   *this << U(0);
   return *this;
}

void collect_stats(const char * __restrict instruction, processor & __restrict processor)
{
   processor.add_stat(instruction);
}

namespace
{
   static uint32 should_debug_break(processor * __restrict proc)
   {
      return proc->get_guest_system()->get_debugger()->should_interrupt_execution();
   }
}

void Jit1_CodeGen::write_chunk(jit1::ChunkOffset & __restrict chunk_offset, jit1::Chunk & __restrict chunk, uint32 start_address, bool update) __restrict
{
   assert((start_address % 4) == 0);
   const uint32 base_address = start_address & ~(jit1::ChunkSize - 1);
   const uint32 last_address = base_address + (jit1::ChunkSize - 1);

   static const int8 flags_offset = sbyte_assert(offset_of(&processor::m_flags) - 128);
   static const int8 pc_offset =  sbyte_assert(offset_of(&processor::m_program_counter) - 128);
   static const int8 gp_offset =  sbyte_assert(offset_of(&processor::m_registers) - 128);
   static const int8 dbt_offset =  sbyte_assert(offset_of(&processor::m_branch_target) - 128);
   static const int8 ic_offset =  sbyte_assert(offset_of(&processor::m_instruction_count) - 128);

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
      L("chunk_start");
      L("code_start");
   }

   static constexpr uint32 num_instructions = jit1::ChunkSize / 4u;
   for (uint32 i = start_index; i < num_instructions; ++i)
   {
      auto cur_label = GetIndexLabel(i);
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
      const mips::instructions::InstructionInfo * __restrict instruction_info_ptr = nullptr;

#if JIT_INSERT_IDENTIFIERS
      std::string id_label = GetIndexLabel(i) + "_id";
      jmp(id_label);
      nop();nop();nop();nop();
      mov(eax, int32(current_address));
      nop();nop();nop();nop();
      L(id_label);
#endif
      
      if (m_jit.m_processor.m_ticked && m_jit.m_processor.m_debugging)
      {
         mov(rax, int64(&should_debug_break));
         mov(dword[rbp + pc_offset], int32(current_address));
         mov(rcx, rbp);
         add(rcx, -128);
         call(rax);
         test(eax, eax);
         jnz("save_return", T_NEAR);
         cmp(rdi, r14);;
         je("save_return", T_NEAR);
      }
      else if (m_jit.m_processor.m_debugging)
      {
         mov(rax, int64(&should_debug_break));
         mov(dword[rbp + pc_offset], int32(current_address));
         mov(rcx, rbp);
         add(rcx, -128);
         call(rax);
         test(eax, eax);
         jnz("save_return", T_NEAR);
      }
      else if (m_jit.m_processor.m_ticked)
      {
         const auto no_flush = GetUniqueLabel();
      
         // check if we are already at our tick count.
         cmp(rdi, r14);
         mov(eax, int32(current_address));
         je("save_return_eax_pc", T_NEAR);
      }

      if (current_address >= 4)
      {
         const size_t ins_start_size = getSize();

         // Also fetch previous instruction.
         const instruction_t * __restrict prev_ptr = m_jit.m_processor.safe_mem_fetch_exec<const instruction_t>(current_address - 4);
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

         const instruction_t * __restrict ptr = m_jit.m_processor.safe_mem_fetch_exec<const instruction_t>(current_address);
         if (ptr)
         {
            const instruction_t instruction = *ptr;
            instruction_info_ptr = mips::FindExecuteInstruction(instruction);

            if (instruction_info_ptr)
            {
               if (m_jit.m_processor.m_collect_stats)
               {
                  // dispatch a stat call.
                  mov(rcx, int64(instruction_info_ptr->Name));
                  call("intrinsic_stats");
               }

               compact_branch = ((instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::CompactBranch)) != 0);
               delay_branch = ((instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::DelayBranch)) != 0);

               if (cti_test && !m_jit.m_processor.m_disable_cti)
               {
                  // CTI instruction prefix.
                  if ((instruction_info_ptr->OpFlags & uint32(mips::instructions::OpFlags::ControlInstruction)) != 0)
                  {
                     const auto no_ex = GetUniqueLabel();

                     test(ebx, processor::flag_bit(processor::flag::no_cti));
                     jz(no_ex);
                     mov(eax, int32(current_address));
                     jmp("intrinsic_ri_ex", T_NEAR);
                     L(no_ex);
                  }
                  else
                  {
                     // This cannot be a CTI. Clear the CTI bit.
                     // C6 42 7F 00 
                     // mov byte [rdx + 0x7F], 0

                     and_(ebx, ~processor::flag_bit(processor::flag::no_cti));
                  }
               }

               // Was the instruction a memory-altering instruction?
               if (
								IS_INSTRUCTION(instruction_info_ptr, PROC_SB) ||
								IS_INSTRUCTION(instruction_info_ptr, PROC_SBE) ||
								IS_INSTRUCTION(instruction_info_ptr, PROC_SC) ||
								IS_INSTRUCTION(instruction_info_ptr, PROC_SCE) ||
								IS_INSTRUCTION(instruction_info_ptr, PROC_SCWP) ||
								IS_INSTRUCTION(instruction_info_ptr, PROC_SCWPE) ||
								IS_INSTRUCTION(instruction_info_ptr, PROC_SH) ||
								IS_INSTRUCTION(instruction_info_ptr, PROC_SHE) ||
								IS_INSTRUCTION(instruction_info_ptr, PROC_SW) ||
								IS_INSTRUCTION(instruction_info_ptr, PROC_SWE) ||
								IS_INSTRUCTION(instruction_info_ptr, COP1_SDC1_v) ||
								IS_INSTRUCTION(instruction_info_ptr, COP1_SWC1_v)
               )
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
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LB) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LBE) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LBU) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LBUE) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LH) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LHE) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LHU) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LHUE) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LL) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LLE)||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LLWP) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LLWPE) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LW) ||
                  IS_INSTRUCTION(instruction_info_ptr, PROC_LWE) ||
                  //IS_INSTRUCTION(instruction_info_ptr, COP1_LDC1) ||
                  //IS_INSTRUCTION(instruction_info_ptr, COP1_LWC1) ||
								 IS_INSTRUCTION(instruction_info_ptr, PROC_LWPC)
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
               //   write_PROC_ADDI(chunk_offset, current_address, instruction, *instruction_info_ptr);
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

               cti = instruction_info_ptr->ControlInstruction;
            }
            else
            {
               // RI
               mov(eax, int32(current_address));
               jmp("intrinsic_ri_ex", T_NEAR);
            }
         }
         else
         {
            // AdEL
            mov(eax, int32(current_address));
            mov(ecx, eax);
            jmp("intrinsic_adel_ex", T_NEAR);
         }

         const size_t ins_size = getSize() - ins_start_size;
         m_jit.m_largest_instruction = std::max(m_jit.m_largest_instruction, ins_size);
      }
      else
      {
         // AdEL
         mov(eax, int32(current_address));
         mov(ecx, eax);
         jmp("intrinsic_adel_ex", T_NEAR);
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
            test(ebx, processor::flag_bit(processor::flag::jit_mem_flush));
            jnz("intrinsic_store_flush", T_NEAR);
         }
         // Handle compact branch.
         if (compact_branch && compact_branch_suffix_required)
         {
            // 8A 42 7F 84 C0 74 6A 48 FF 42 7F C6 42 7F 00 8B 42 7F B9 FF FF FF 7F 39 C8 72 2B 3D FF FF FF 7F 77 24 29 C8 48 B9 FF FF FF FF FF FF FF 7F 8B 04 01 48 B9 FF FF FF FF FF FF FF 7F 48 01 C8 45 31 C0 48 31 C9 FF E0 52 48 89 C2 48 83 EC 20 48 B8 FF FF FF FF FF FF FF 7F 48 B9 FF FF FF FF FF FF FF 7F FF D0 48 83 C4 20 5A 45 31 C0 48 31 C9 FF E0 
            // mov al, byte [rdx + 0x7F]           ; load [pcc_offset] (program counter changed)
            // test al, al                         ; is pcc_offset 0?
            // je no_change                        ; if so, jump past this routine
            // inc qword [rdx + 0x7F]              ; increment the [ic_offset] instruction counter
            // mov byte [rdx + 0x7F], 0            ; set [pcc_offset] to 0
            // mov eax, dword [rdx + 0x7F]         ; load [pc_offset] (program counter), zero extended, set by the compact branch instruction
            //                                     ; check first if this offset is within the current chunk.
            // mov ecx, 0x7FFFFFFF                 ; load the [chunk base]
            // cmp eax, ecx                        ; is the address below chunk base?
            // jb not_within                       ; if so, skip to 'not_within'
            // cmp eax, 0x7FFFFFFF                 ; is the address above [chunk last]? (chunk base + chunk size - 1) (because of 0x1'0000'0000)
            // ja not_within                       ; if so, skip to 'not within'
            //                                     ; if we are jumping internally, we can hardcode the offset
            // sub eax, ecx                        ; subtract chunk base from our address, giving us an offset within the chunk.
            //                                     ; this offset is 4-byte aligned, as are all instructions. Luckily, so is 'chunk_offset'.
            // mov rcx, qword 0x7FFFFFFFFFFFFFFF   ; load chunk_offset (uint32[])
            // mov eax, dword [rcx + rax]          ; load chunk_offset[rax] (rax vs eax doesn't matter here, the value is 32-bit zero-extended)
            // mov rcx, qword 0x7FFFFFFFFFFFFFFF   ; load chunk data pointer [pointer fixed up after generation of chunk]
            // add qword rax, rcx                  ; add the actual offset to the chunk data pointer, which is our jump target
            // xor r8d, r8d                        ; clear r8d (program counter delta)
            // xor rcx, rcx                        ; zero rcx, as our JIT guarantees that it is 0 at the start of instructions.
            // jmp rax                             ; jump to the local jump target
            // not_within:                         ; otherwise, we are jumping to a remove jump target
            // 
            // push rdx                            ; push rdx [stores 'processor' pointer + 128, a value we must retain]
            // mov rdx, rax                        ; move rax to rdx, which is the second argument in Win64 ABI
            // sub rsp, 32                         ; push the home space required by the Win64 ABI
            // mov rax, 0x7FFFFFFFFFFFFFFF         ; load the address of [jit1::get_instruction]
            // mov rcx, 0x7FFFFFFFFFFFFFFF         ; load the [this] pointer into rcx, which is the first argument in Win64 ABI
            // call rax                            ; call [jit1::get_instruction]
            // add rsp, 32                         ; pop the home space
            // pop rdx                             ; restore rdx
            // xor r8d, r8d                        ; clear r8d (program counter delta)
            // xor rcx, rcx                        ; zero rcx, as our JIT guarantees that it is 0 at the start of instructions.
            // jmp rax                             ; jump to the remote jump target
            // no_change:                          ; otherwise, the subroutine (unencoded) compact branch did not trigger a branch.
            const auto no_change = GetUniqueLabel();
            const auto not_within = GetUniqueLabel();

            test(ebx, processor::flag_bit(processor::flag::pc_changed));
            jz(no_change);
            and_(ebx, ~processor::flag_bit(processor::flag::pc_changed));
            mov(eax, dword[rbp + pc_offset]);

            mov(ecx, eax);
            and_(ecx, ~(jit1::ChunkSize - 1));
            cmp(ecx, base_address);
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
            test(ebx, processor::flag_bit(processor::flag::trapped_exception));
            jnz("intrinsic_check_ex", T_NEAR);
         }
         if (possible_after_delaybranch)
         {
            handle_delay_branch(chunk, chunk_offset, current_address - 4, prev_instruction, *prev_instruction_info_ptr);
         }
      }
#if JIT_INSTRUCTION_SEPARATE
      rdata << bytes({ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 });
#endif

      start_address += 4;
   }

   const auto patch_preprolog = [&](auto address) -> std::string
   {
      // If execution gets past the chunk, we jump to the next chunk.
      // Start with a set of nops so that we have somewhere to write patch code.
      const std::string patch = GetUniqueLabel();
      L(patch); // patch should be 12 bytes. Enough to copy an 8B pointer to rax, and then to jump to it.
      chunk.m_patches.push_back({ uint32(getSize()), 0 });
      auto &patch_pair = chunk.m_patches.back();
      uint32 &patch_target = patch_pair.target;

			// patch no-op
			if (address == nullptr) {
				for (uint8 octet : { 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x00 }) db(octet);
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

   const auto patch_epilog = [&](const std::string &patch)
   {
      static constexpr uint16 patch_prefix = 0xB848;
      static constexpr uint16 patch_suffix = 0xE0FF;

      mov(rcx, patch.c_str());
      mov(word[rcx], int16_t(patch_prefix));
      mov(qword[rcx + 2], rax);
      mov(word[rcx + 10], int16_t(patch_suffix));
   };

   const auto patch = patch_preprolog(m_jit.fetch_instruction(uint32(last_address + 1)));

   mov(edx, uint32(last_address + 1));

   patch_prolog();

   mov(dword[rbp + pc_offset], edx);
   mov(rax, uint64(jit1_get_instruction));
   mov(rcx, uint64(&m_jit));
   call(rax);

   patch_epilog(patch);

   jmp(rax);

   {
      L("intrinsic_start");

      L("intrinsic_ri_ex");
      call("save");
      mov(rax, uint64(RI_Exception));
      mov(rdx, rbp);
      add(rdx, -128);
      add(rsp, 40);
      jmp(rax);

      L("intrinsic_adel_ex");
      call("save");
      mov(rax, uint64(AdEL_Exception));
      mov(rdx, rbp);
      add(rdx, -128);
      add(rsp, 40);
      jmp(rax);

      L("intrinsic_ades_ex");
      call("save");
      mov(rax, uint64(AdES_Exception));
      mov(rdx, rbp);
      add(rdx, -128);
      add(rsp, 40);
      jmp(rax);

      L("intrinsic_ov_ex");
      call("save");
      mov(rax, uint64(OV_Exception));
      mov(rdx, rbp);
      add(rdx, -128);
      add(rsp, 40);
      jmp(rax);

      L("intrinsic_tr_ex");
      call("save");
      mov(rax, uint64(TR_Exception));
      mov(rdx, rbp);
      add(rdx, -128);
      add(rsp, 40);
      jmp(rax);

      L("intrinsic_store_flush");
      jmp("save_return");

      L("intrinsic_check_ex");
      jmp("save_return");

      L("save_return_eax_pc");
      mov(dword[rbp + pc_offset], eax);
      jmp("save_return");


      L("save");
      mov(qword[rbp + ic_offset], rdi);      // save instruction count
      mov(dword[rbp + flags_offset], ebx);
      mov(dword[rbp + dbt_offset], esi);  // set it in the interpreter
      mov(dword[rbp + (int64(gp_offset) + (mips_fp * 4))], r15d);
      ret();

      L("save_return");
      call("save");
      add(rsp, 40);
      ret();

      if (m_jit.m_processor.m_collect_stats)
      {
         // dispatch a stat call.
         L("intrinsic_stats");
         mov(rax, uint64(collect_stats));
         mov(rdx, rbp);
         add(rdx, -128);
         sub(rsp, 40);
         call(rax);
         add(rsp, 40);
         ret();
      }
}

   assert(!hasUndefinedLabel());
   ready();
   //chunk.m_datasize = getSize();
   const uint8 * __restrict data = getCode();

   //this->

   assert(chunk_offset.back() < getSize());
   assert(getSize() <= jit1::MaxChunkRealSize);

   // Finalize the data stream and then allocate a new chunk for it, if necessary.
   uint32 total_size = uint32(this->getSize()) + chunk_start_offset;
   // The current chunk is large enough for our data.
   //memcpy((char *)chunk.m_data + chunk_start_offset, data, getSize());

   if (!update)
   {
      m_jit.m_Chunks.push_back(&chunk);
      std::sort(m_jit.m_Chunks.begin(), m_jit.m_Chunks.end());
   }
}

void jit1::populate_chunk(ChunkOffset & __restrict chunk_offset, Chunk & __restrict chunk, uint32 start_address, bool update) __restrict
{
   if (!chunk.m_data)
   {
      chunk.m_datasize = MaxChunkRealSize;
      chunk.m_data = (uint8 *)allocate_executable(MaxChunkRealSize);
   }
   Jit1_CodeGen cg{ *this, (uint8 *)chunk.m_data, jit1::MaxChunkRealSize };
   cg.write_chunk(chunk_offset, chunk, start_address, update);
}

// TODO may also need to update the _following_ chunk if we are altering the last instruction of this chunk.
bool jit1::memory_touched(uint32 address) __restrict
{
   // Presume this is a sorted vector by address. We want to find a chunk that contains this address, if there is one.
   // TODO replace this with log n binary search.

   const uint32 mapped_address = address & ~(ChunkSize - 1);

   Chunk * __restrict dirty_chunk = get_chunk(address);
   if (dirty_chunk)
   {
      const uint32 adjusted_address = address & ~((1 << 2) - 1);
      if (mapped_address == (m_CurrentExecutingChunkAddress& ~(ChunkSize - 1)))
      {
         m_FlushAddress = adjusted_address;
         m_processor.set_flags(processor::flag_bit(processor::flag::jit_mem_flush));
         m_FlushChunk = dirty_chunk;
         return true;
      }
      else
      {
         populate_chunk(*(dirty_chunk->m_chunk_offset), *dirty_chunk, adjusted_address, true);
         if (adjusted_address + 4 == dirty_chunk->m_offset + ChunkSize)
         {
            // At the end of the chunk. Also update next chunk if it exists.
            if (memory_touched(adjusted_address + 4))
            {
               if (adjusted_address + 4 == mapped_address)
               {
                  return true;
               }
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
            m_processor.m_jit_meta = 1;
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


__forceinline void jit1::execute_instruction(uint32 address) __restrict
{
   m_CurrentExecutingChunkAddress = address;
   // RCX, mov'd from the machine code   // RDX       // R8
   auto instruction = get_instruction(address);
   __pragma(pack(1)) struct ParameterPack
   {
      uint64 coprocessor1;
      uint32 flags;
      uint32 delay_branch_target;
      uint64 target_instruction;
      uint32 frame_pointer;
   } ppack;
   ppack.coprocessor1 = uint64(m_processor.get_coprocessor(1));
   ppack.flags = m_processor.m_flags;
   ppack.delay_branch_target = m_processor.m_branch_target;
   ppack.target_instruction = m_processor.m_target_instructions;
   ppack.frame_pointer = m_processor.m_registers[30];

   const uint64 result = jit1_springboard(uint64(instruction), uint64(&m_processor), m_processor.m_instruction_count, uint64(&ppack), 0, 0);
   if (m_processor.get_guest_system()->is_execution_complete())
   {
      if (m_processor.get_guest_system()->is_execution_success())
      {
         throw ExecutionCompleteException();
      }
      else
      {
         throw ExecutionFailException();
      }
   }
   if (m_processor.get_flags(processor::flag_bit(processor::flag::jit_mem_flush)))
   {
      populate_chunk(*m_FlushChunk->m_chunk_offset, *m_FlushChunk, m_FlushAddress, true);
      if (m_FlushAddress + 4 == m_FlushChunk->m_offset + ChunkSize)
      {
         memory_touched(m_FlushAddress + 4);
      }
   }
   if (m_processor.get_flags(processor::flag_bit(processor::flag::trapped_exception)))
   {
      // there was an exception.
      m_processor.clear_flags(processor::flag_bit(processor::flag::trapped_exception));
      throw m_processor.m_trapped_exception;
   }
}

__forceinline jit1::jit_instructionexec_t jit1::get_instruction(uint32 address) __restrict
{
   const uint32 mapped_address = address & ~(ChunkSize - 1);
   const uint32 address_offset = (address - mapped_address) / 4u;

   Chunk * __restrict chunk = nullptr;
   ChunkOffset * __restrict chunk_offset = nullptr;

   if (m_LastChunkAddress == mapped_address)
   {
      chunk = m_LastChunk;
      chunk_offset = m_LastChunkOffset;
   }
   else
   {
      // Traverse the map to end up at the proper chunk.
      /* This is only hit once ever in the current benchmark because the chunks are huge */
      auto & __restrict cml2 = m_JitMap[address >> (32 - RemainingLog2)];
      auto & __restrict cml1 = cml2[(address >> (32 - RemainingLog2 - 8)) & 0xFF];

			const bool chunk_exists = cml1.contains((address >> (32 - RemainingLog2 - 16)) & 0xFF);
			chunk = &cml1[(address >> (32 - RemainingLog2 - 16)) & 0xFF];
			chunk_offset = &cml1.get_offsets((address >> (32 - RemainingLog2 - 16)) & 0xFF);

      if (!chunk_exists) {
         populate_chunk(*chunk_offset, *chunk, mapped_address, false);
      }

      m_LastChunk = chunk;
      m_LastChunkOffset = chunk_offset;
      m_LastChunkAddress = mapped_address;
   }

   const auto & __restrict chunk_instruction = chunk->m_data + (*chunk_offset)[address_offset];
   
   return jit_instructionexec_t(chunk_instruction);
}

__forceinline jit1::jit_instructionexec_t jit1::fetch_instruction(uint32 address) __restrict
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	Chunk* __restrict chunk = nullptr;
	ChunkOffset* __restrict chunk_offset = nullptr;

	if (m_LastChunkAddress == mapped_address)
	{
		chunk = m_LastChunk;
		chunk_offset = m_LastChunkOffset;
	}
	else
	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */
		auto& __restrict cml2 = m_JitMap[address >> (32 - RemainingLog2)];
		auto& __restrict cml1 = cml2[(address >> (32 - RemainingLog2 - 8)) & 0xFF];

		if (!cml1.contains((address >> (32 - RemainingLog2 - 16)) & 0xFF)) {
			return jit_instructionexec_t(nullptr);
		}

		chunk = &cml1[(address >> (32 - RemainingLog2 - 16)) & 0xFF];
		chunk_offset = &cml1.get_offsets((address >> (32 - RemainingLog2 - 16)) & 0xFF);

		m_LastChunk = chunk;
		m_LastChunkOffset = chunk_offset;
		m_LastChunkAddress = mapped_address;
	}

	const auto& __restrict chunk_instruction = chunk->m_data + (*chunk_offset)[address_offset];

	return jit_instructionexec_t(chunk_instruction);
}

jit1::Chunk * jit1::get_chunk(uint32 address) __restrict
{
   const uint32 mapped_address = address & ~(ChunkSize - 1);
   const uint32 address_offset = (address - mapped_address) / 4u;

   if (m_LastChunkAddress == mapped_address)
   {
      return m_LastChunk;
   }
   else
   {
      // Traverse the map to end up at the proper chunk.
      /* This is only hit once ever in the current benchmark because the chunks are huge */

      const uint32 jitmap_idx = address >> (32 - RemainingLog2);
      if (!m_JitMap.contains(jitmap_idx))
      {
         return false;
      }
      auto & __restrict cml2 = m_JitMap[jitmap_idx];

      const uint32 cml2_idx = (address >> (32 - RemainingLog2 - 8)) & 0xFF;
      if (!cml2.contains(cml2_idx))
      {
         return false;
      }
      auto & __restrict cml1 = cml2[cml2_idx];

      const uint32 cml1_idx = (address >> (32 - RemainingLog2 - 16)) & 0xFF;
      if (!cml1.contains(cml1_idx))
      {
         return nullptr;
      }
      else
      {
         return &cml1[cml1_idx];
      }
   }
}
