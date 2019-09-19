#include "pch.hpp"
#include "processor_inst.hpp"
#include "mips/processor/processor.hpp"
#include "mips/coprocessor/coprocessor.hpp"

#include <limits>

using namespace mips;

extern __forceinline processor * __restrict get_current_processor();
extern __forceinline coprocessor * __restrict get_current_coprocessor();
extern __forceinline void set_current_coprocessor(coprocessor * __restrict cop);

#define ProcInstructionDef(InsInstruction, InsOperFlags, InsOpMask, InsOpRef)                                                     \
struct PROC_ ## InsInstruction                                                                                                    \
{                                                                                                                                 \
   friend class PROC_Helper;                                                                                                      \
   PROC_ ## InsInstruction() = delete;                                                                                            \
   static_assert(std::is_same<decltype(InsOperFlags), OpFlags>::value, "Operation Flags field must be of type 'OpFlags'");        \
   static constexpr uint32 OpMask = InsOpMask;                                                                                    \
   static constexpr OpFlags Flags = InsOperFlags;                                                                                 \
                                                                                                                                  \
   static __forceinline bool SubExecute                                                                                           \
      (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor);                       \
                                                                                                                                  \
   template <typename reg_t, typename format_t>                                                                                   \
   static __forceinline bool write_result(reg_t &dest, format_t value)                                                            \
   {                                                                                                                              \
      return PROC_Helper::_write_result<reg_t, format_t, Flags>(dest, value);                                                     \
   }                                                                                                                              \
};                                                                                                                                \
namespace PROC_ ## InsInstruction ## _NS                                                                                          \
{                                                                                                                                 \
   uint64 Execute(instruction_t instruction, processor & __restrict processor)                                                    \
   {                                                                                                                              \
      set_current_coprocessor(processor.get_coprocessor(1));                                                                      \
      PROC_Helper::Execute<PROC_ ## InsInstruction >(                                                                             \
         instruction, processor, *(coprocessor1 * __restrict)processor.get_coprocessor(1)                                         \
      );                                                                                                                          \
      return 0;                                                                                                                   \
   }                                                                                                                              \
   class _StaticInit : public _InstructionInitializer                                                                             \
   {                                                                                                                              \
   public:                                                                                                                        \
      _StaticInit() :                                                                                                             \
         _InstructionInitializer(                                                                                                 \
            "PROC_" #InsInstruction,                                                                                              \
            PROC_ ## InsInstruction::OpMask,                                                                                      \
            InsOpRef,                                                                                                             \
            &Execute,                                                                                                             \
            uint32(PROC_ ## InsInstruction::Flags),                                                                               \
            uint32(PROC_ ## InsInstruction::Flags & OpFlags::ControlInstruction) != 0                                             \
         )                                                                                                                        \
      {}                                                                                                                          \
   } static _StaticInitObj;                                                                                                       \
}                                                                                                                                 \
__forceinline bool PROC_ ## InsInstruction::SubExecute

namespace mips::instructions
{
   class _InstructionInitializer
   {
   public:
      _InstructionInitializer(const char *name, uint32 instructionMask, uint32 instructionRef, instructionexec_t exec, uint32 OpFlags, bool control)
      {
#if !USE_STATIC_INSTRUCTION_SEARCH
         if (!instructions::StaticInitVarsPtr)
         {
            instructions::StaticInitVarsPtr = new instructions::StaticInitVars;
         }
         const InstructionInfo Procs{ name, control, 1, exec, OpFlags };
         FullProcInfo FullProc = { instructionMask, instructionRef, Procs };
         instructions::StaticInitVarsPtr->g_ProcInfos.push_back(FullProc);
#endif
      }
   };

#pragma message("replace coprocessor1 with coprocessor0 when implemented")

   class PROC_Helper
   {
      PROC_Helper() = delete;

   public:
      template <typename InsT>
      __forceinline static void Execute(instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
      {
         try
         {
            if (uint32(InsT::Flags & OpFlags::ControlInstruction) && processor.get_no_cti())
            {
               throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
            }

            //if (uint32(InsT::Flags & OpFlags::ClearsCause))
            //{
            //   ((coprocessor1::FCSR &)coprocessor.get_FCSR()).Cause = 0;
            //}

            InsT::SubExecute(instruction, processor, coprocessor);
         }
         catch (const CPU_Exception &ex)
         {
            if (processor.get_jit_type() == JitType::Jit)
            {
               processor.set_trapped_exception(ex);
            }
            else
            {
               throw;
            }
         }
      }

      template <typename reg_t, typename format_t, OpFlags Flags>
      __forceinline static bool _write_result(reg_t & __restrict dest, format_t value)
      {
         dest.template set<format_t>(value);
         return true;
      }
   };

   ProcInstructionDef(
      ADD,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000100000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const int64 result = int64(rs.value<int32>()) + rt.value<int32>();
      if (result > std::numeric_limits<int32>::max() || result < std::numeric_limits<int32>::min())
      {
         throw CPU_Exception{ CPU_Exception::Type::Ov, processor.get_program_counter() };
      }

      return write_result(rd, int32(result));
   }

   ProcInstructionDef(
      ADDIU,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b00100100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 immediate = TinyInt<16>(instruction).sextend<int32>();

      const uint32 result = rs.value<uint32>() + immediate;

      return write_result(rt, result);
   }

   ProcInstructionDef(
      MOVE,
      (OpFlags::None),
      0b11111100000000001111111111111111,
      0b00100100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);

      const uint32 result = rs.value<uint32>();

      return write_result(rt, result);
   }

   ProcInstructionDef(
      ADDIUPC,
      (OpFlags::None),
      0b11111100000110000000000000000000,
      0b11101100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      GPRegister<21, 5> rs(instruction, processor);
      const int32 immediate = TinyInt<21>(instruction << 2).sextend<int32>();

      const uint32 result = processor.get_program_counter() + immediate;

      return write_result(rs, result);
   }

   ProcInstructionDef(
      ADDU,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000100001
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 result = rs.value<uint32>() + rt.value<uint32>();
#pragma message("make sure we aren't supposed to clamp the value")
      return write_result(rd, result);
   }

   ProcInstructionDef(
      ALIGN,
      (OpFlags::None),
      0b11111100000000000000011100111111,
      0b01111100000000000000001000100000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      const uint32 bp = TinyInt<2>(instruction).zextend<uint32>();

      const uint32 result = (rt.value<uint32>() << (8 * bp)) | (rs.value<uint32>() >> (32 - (8 * bp)));
      return write_result(rd, result);
   }

   ProcInstructionDef(
      ALUIPC,
      (OpFlags::None),
      0b11111100000111110000000000000000,
      0b11101100000111110000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      GPRegister<21, 5> rs(instruction, processor);
      const int32 immediate = TinyInt<32>(instruction << 16).sextend<int32>();

      const uint32 result =  ~0xFFFF & uint32_t(processor.get_program_counter() + immediate);
      return write_result(rs, result);
   }

   ProcInstructionDef(
      AND,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000100100
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 result = rs.value<uint32>() & rt.value<uint32>();
      return write_result(rd, result);
   }

   ProcInstructionDef(
      ANDI,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b00110000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const uint32 immediate = TinyInt<16>(instruction).zextend<uint32>();

      const uint32 result = rs.value<uint32>() & immediate;
      return write_result(rt, result);
   }

   ProcInstructionDef(
      AUI,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b00111100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 immediate = TinyInt<32>(instruction << 16).sextend<int32>();

      const uint32 result = rs.value<uint32>() + immediate;
      return write_result(rt, result);
   }

   ProcInstructionDef(
      AUIPC,
      (OpFlags::None),
      0b11111100000111110000000000000000,
      0b11101100000111100000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      GPRegister<21, 5> rs(instruction, processor);
      const int32 immediate = TinyInt<16>(instruction).sextend<int32>() << 16; // this is intentional

      const uint32 result = processor.get_program_counter() + immediate;
      return write_result(rs, result);
   }

   ProcInstructionDef(
      BAL,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111111111111110000000000000000,
      0b00000100000100010000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const int32 immediate = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      const uint32 address = pc + 4 + immediate;

      processor.set_link(pc + 8);
      processor.delay_branch(address);

      return true;
   }

   ProcInstructionDef(
      BALC,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch),
      0b11111100000000000000000000000000,
      0b11101000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const int32 immediate = TinyInt<28>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      const uint32 address = pc + 4 + immediate;

      processor.set_link(pc + 4);
      processor.compact_branch(address);
      // BALC does not have a forbidden slot as it is unconditional compact.

      return true;
   }

   ProcInstructionDef(
      BC,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch),
      0b11111100000000000000000000000000,
      0b11001000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const int32 immediate = TinyInt<28>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      const uint32 address = pc + 4 + immediate;

      processor.compact_branch(address);
      // BC does not have a forbidden slot as it is unconditional compact.

      return true;
   }

   ProcInstructionDef(
      BEQ,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b00010000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 immediate = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.value<uint32>() == rt.value<uint32>())
      {
         const uint32 address = pc + 4 + immediate;
         processor.delay_branch(address);
      }
      else
      {
         processor.set_flags(processor::flag_bit(processor::flag::no_cti));
      }

      return true;
   }

   ProcInstructionDef(
      BGEZ,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000111110000000000000000,
      0b00000100000000010000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const int32 immediate = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.value<int32>() >= 0)
      {
         const uint32 address = pc + 4 + immediate;
         processor.delay_branch(address);
      }
      else
      {
         processor.set_flags(processor::flag_bit(processor::flag::no_cti));
      }

      return true;
   }

   ProcInstructionDef(
      BLEZALC,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111111111000000000000000000000,
      0b00011000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rt.get_register() != 0) // BLEZALC
      {
         processor.set_link(pc + 4);
         if (rt.value<int32>() <= 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      POP06,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b00011000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs == rt && rs.get_register() != 0) // BGEZALC
      {
         processor.set_link(pc + 4);
         if (rt.value<int32>() >= 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else if (rs != rt && rs.get_register() != 0 && rt.get_register() != 0) // BGEUC
      {
         if (rs.value<uint32>() >= rt.value<uint32>())
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      BGTZALC,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111111111000000000000000000000,
      0b00011100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rt.get_register() != 0) // BGTZALC
      {
         processor.set_link(pc + 4);
         if (rt.value<int32>() > 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      POP07,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b00011100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs == rt && rs.get_register() != 0) // BLTZALC
      {
         processor.set_link(pc + 4);
         if (rt.value<int32>() < 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else if (rs != rt && rs.get_register() != 0 && rt.get_register() != 0) // BLTUC
      {
         if (rs.value<uint32>() < rt.value<uint32>())
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      POP10,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b00100000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.get_register() == 0 && rs.get_register() < rt.get_register()) // BEQZALC
      {
         processor.set_link(pc + 4);
         if (rt.value<int32>() == 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() < rt.get_register()) // BEQC
      {
         if (rt.value<uint32>() == rs.value<uint32>())
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else if (rs.get_register() >= rt.get_register()) // BOVC
      {
         const int64 result = int64(rs.value<int32>()) + rt.value<int32>();
         const bool overflow = (result > std::numeric_limits<int32>::max() || result < std::numeric_limits<int32>::min());

         if (overflow)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      POP30,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b01100000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.get_register() == 0 && rs.get_register() < rt.get_register()) // BNEZALC
      {
         processor.set_link(pc + 4);
         if (rt.value<int32>() != 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() < rt.get_register()) // BNEC
      {
         if (rt.value<uint32>() != rs.value<uint32>())
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else if (rs.get_register() >= rt.get_register()) // BNVC
      {
         const int64 result = int64(rs.value<int32>()) + rt.value<int32>();
         const bool overflow = (result > std::numeric_limits<int32>::max() || result < std::numeric_limits<int32>::min());

         if (!overflow)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      BLEZC,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111111111000000000000000000000,
      0b01011000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rt.get_register() != 0) // BLEZC
      {
         if (rt.value<int32>() <= 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      POP26,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b01011000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() == rt.get_register()) // BGEZC
      {
         if (rt.value<int32>() >= 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() != rt.get_register()) // BGEC / BLEC
      {
         if (rs.value<int32>() >= rt.value<int32>())
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      BGTZC,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111111111000000000000000000000,
      0b01011100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rt.get_register() != 0) // BGTZC
      {
         if (rt.value<int32>() > 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      POP27,
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b01011100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() == rt.get_register()) // BLTZC
      {
         if (rt.value<int32>() < 0)
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else if (rs.get_register() != 0 && rt.get_register() != 0 && rs.get_register() != rt.get_register()) // BLTC / BGTC
      {
         if (rs.value<int32>() < rt.value<int32>())
         {
            const uint32 address = pc + 4 + offset;
            processor.compact_branch(address);
         }
         else
         {
            processor.set_no_cti();
         }
      }
      else
      {
         throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter() };
      }

      return true;
   }

   ProcInstructionDef(
      BEQZC, // POP66 where rs != 0
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b11011000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const int32 offset = TinyInt<23>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.value<int32>() == 0)
      {
         const uint32 address = pc + 4 + offset;
         processor.compact_branch(address);
      }
      else
      {
         processor.set_no_cti();
      }

      return true;
   }

   ProcInstructionDef(
      JIC, // PO66 where rs == 0
      (OpFlags::ControlInstruction | OpFlags::CompactBranch),
      0b11111111111000000000000000000000,
      0b11011000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      const uint32 address = rt.value<uint32>() + offset;

      processor.compact_branch(address);

      return true;
   }

   ProcInstructionDef(
      BNEZC, // POP76 where rs != 0
      (OpFlags::ControlInstruction | OpFlags::CompactBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b11111000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const int32 offset = TinyInt<23>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.value<int32>() != 0)
      {
         const uint32 address = pc + 4 + offset;
         processor.compact_branch(address);
      }
      else
      {
         processor.set_no_cti();
      }

      return true;
   }

   ProcInstructionDef(
      JIALC, // POP76 where rs == 0
      (OpFlags::ControlInstruction | OpFlags::CompactBranch),
      0b11111111111000000000000000000000,
      0b11111000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      const uint32 address = rt.value<uint32>() + offset;

      processor.set_link(pc + 4);
      processor.compact_branch(address);

      return true;
   }

   ProcInstructionDef(
      BGTZ,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000111110000000000000000,
      0b00011100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.value<int32>() > 0)
      {
         const uint32 address = pc + 4 + offset;
         processor.delay_branch(address);
      }
      else
      {
         processor.set_flags(processor::flag_bit(processor::flag::no_cti));
      }

      return true;
   }

   ProcInstructionDef(
      BITSWAP,
      (OpFlags::None),
      0b11111111111000000000011111111111,
      0b01111100000000000000000000100000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      
      // TODO optimize
      union register_alias
      {
         uint32 reg;
         uint8 bytes[4];
      } reg;
      reg.reg = rt.value<uint32>();

      for (size_t i = 0; i < sizeof(uint32); ++i)
      {
         // http://stackoverflow.com/questions/2602823/in-c-c-whats-the-simplest-way-to-reverse-the-order-of-bits-in-a-byte
         reg.bytes[i] = (reg.bytes[i] & 0xF0) >> 4 | (reg.bytes[i] & 0x0F) << 4;
         reg.bytes[i] = (reg.bytes[i] & 0xCC) >> 2 | (reg.bytes[i] & 0x33) << 2;
         reg.bytes[i] = (reg.bytes[i] & 0xAA) >> 1 | (reg.bytes[i] & 0x55) << 1;
      }

      write_result<>(rd, reg.reg);

      return true;
   }

   ProcInstructionDef(
      BLEZ,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000111110000000000000000,
      0b00011000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.value<int32>() <= 0)
      {
         const uint32 address = pc + 4 + offset;
         processor.delay_branch(address);
      }
      else
      {
         processor.set_flags(processor::flag_bit(processor::flag::no_cti));
      }

      return true;
   }

   ProcInstructionDef(
      BLTZ,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000111110000000000000000,
      0b00000100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.value<int32>() < 0)
      {
         const uint32 address = pc + 4 + offset;
         processor.delay_branch(address);
      }
      else
      {
         processor.set_flags(processor::flag_bit(processor::flag::no_cti));
      }

      return true;
   }

   ProcInstructionDef(
      BNE,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b00010100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<18>(instruction << 2).sextend<int32>();
      const uint32 pc = processor.get_program_counter();
      
      if (rs.value<uint32>() != rt.value<uint32>())
      {
         const uint32 address = pc + 4 + offset;
         processor.delay_branch(address);
      }
      else
      {
         processor.set_flags(processor::flag_bit(processor::flag::no_cti));
      }

      return true;
   }

   ProcInstructionDef(
      BREAK,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b00000000000000000000000000001101
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const uint32 code = TinyInt<20>(instruction >> 6).zextend<uint32>();

      throw CPU_Exception{ CPU_Exception::Type::Bp, processor.get_program_counter(), code };
   }

   ProcInstructionDef(
      CACHE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000100101
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      const uint32 op = TinyInt<5>(instruction >> 16).zextend<uint32>();
      const uint32 offset = TinyInt<9>(instruction >> 6).sextend<uint32>();

      const uint32 address = base.get_register() + offset;

      processor.mem_poke<char>(address); // We still want to touch the address.

      return true;
   }

   ProcInstructionDef(
      CACHEE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000110110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      const uint32 op = TinyInt<5>(instruction >> 16).zextend<uint32>();
      const uint32 offset = TinyInt<9>(instruction >> 6).sextend<uint32>();

      const uint32 address = base.get_register() + offset;

      processor.mem_poke<char>(address); // We still want to touch the address.

      return true;
   }

   ProcInstructionDef(
      CLO,
      (OpFlags::None),
      0b11111100000111110000011111111111,
      0b00000000000000000000000001100001
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      uint32 ones = 32;
      uint32 source = rs.value<uint32>();
      for (int i = 31; i >= 0; --i)
      {
         if (((source >> i) & 1) == 0)
         {
            ones = 31 - i;
            break;
         }
      }

      return write_result<>(rd, ones);
   }

   ProcInstructionDef(
      CLZ,
      (OpFlags::None),
      0b11111100000111110000011111111111,
      0b00000000000000000000000001010000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      uint32 zeroes = 32;
      uint32 source = rs.value<uint32>();
      for (int i = 31; i >= 0; --i)
      {
         if (((source >> i) & 1) != 0)
         {
            zeroes = 31 - i;
            break;
         }
      }

      return write_result<>(rd, zeroes);
   }

   ProcInstructionDef(
      DIV,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000010011010
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      if (rt.value<int32>() == 0)
      {
         return write_result(rd, int32(0));  // UNPREDICTABLE
      }

      const int32 result = rs.value<int32>() / rt.value<int32>();

      return write_result(rd, int32(result));
   }

   ProcInstructionDef(
      MOD,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000011011010
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      if (rt.value<int32>() == 0)
      {
         return write_result(rd, int32(0));  // UNPREDICTABLE
      }

      const int32 result = rs.value<int32>() % rt.value<int32>();

      return write_result(rd, int32(result));
   }

   ProcInstructionDef(
      DIVU,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000010011011
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      if (rt.value<uint32>() == 0)
      {
         return write_result(rd, uint32(0));  // UNPREDICTABLE
      }

      const uint32 result = rs.value<uint32>() / rt.value<uint32>();

      return write_result(rd, uint32(result));
   }

   ProcInstructionDef(
      MODU,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000011011011
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      if (rt.value<uint32>() == 0)
      {
         return write_result(rd, uint32(0));  // UNPREDICTABLE
      }

      const uint32 result = rs.value<uint32>() % rt.value<uint32>();

      return write_result(rd, uint32(result));
   }

   ProcInstructionDef(
      EHB,
      (OpFlags::None),
      0b11111111111111111111111111111111,
      0b00000000000000000000000011000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      // do nothing. We have no hazards.
      return true;
   }

   ProcInstructionDef(
      EXT,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b01111100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const uint32 msbd = TinyInt<5>(instruction >> 11).zextend<uint32>();
      const uint32 lsb = TinyInt<5>(instruction >> 6).zextend<uint32>();

      if (lsb + msbd > 31)
      {
         return write_result(rt, uint32(-1)); // UNPREDICTABLE
      }

      uint32 value = rs.value<uint32>();
      value >>= lsb;
      value &= ((1U << (msbd + 1)) - 1); // is + 1 correct?

      return write_result(rt, value);
   }

   ProcInstructionDef(
      INS,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b01111100000000000000000000000100
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const uint32 msb = TinyInt<5>(instruction >> 11).zextend<uint32>();
      const uint32 lsb = TinyInt<5>(instruction >> 6).zextend<uint32>();

      if (lsb > msb)
      {
         return write_result(rt, uint32(-1)); // UNPREDICTABLE
      }

      const uint32 size = msb - lsb + 1;

      const uint32 rs_val = rs.value<uint32>();
      uint32 rt_val = rt.value<uint32>();

      const uint32 extracted_val = rs_val & Bits(size);
      rt_val &= ~(Bits(size) << lsb);
      rt_val |= extracted_val << lsb;

      return write_result(rt, rt_val);
   }

   ProcInstructionDef(
      J,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b00001000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const uint32 instr_index = TinyInt<28>(instruction << 2).zextend<uint32>();
      
      const uint32 address = (processor.get_program_counter() & HighBits(4)) | instr_index;

      processor.delay_branch(address);

      return true;
   }

   ProcInstructionDef(
      JAL,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000000000000000000000000,
      0b00001100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const uint32 instr_index = TinyInt<28>(instruction << 2).zextend<uint32>();
      
      const uint32 address = (processor.get_program_counter() & HighBits(4)) | instr_index;

      processor.set_link(processor.get_program_counter() + 8);
      processor.delay_branch(address);

      return true;
   }

   ProcInstructionDef(
      JALR,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000111110000000000111111,
      0b00000000000000000000000000001001
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      rd.set<uint32>(processor.get_program_counter() + 8);
      processor.delay_branch(rs.value<uint32>());

      return true;
   }

   ProcInstructionDef(
      JR,
      (OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::SetNoCTI),
      0b11111100000111111111100000111111,
      0b00000000000000000000000000001001
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);

      processor.delay_branch(rs.value<uint32>());

      return true;
   }

   ProcInstructionDef(
      LB,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b10000000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const int32 value = int32(processor.mem_fetch<int8>(address));
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LBE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000101100
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const int32 value = int32(processor.mem_fetch<int8>(address));
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LBU,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b10010000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint32 value = uint32(processor.mem_fetch<uint8>(address));
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LBUE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000101000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint32 value = uint32(processor.mem_fetch<uint8>(address));
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LH,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b10000100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const int32 value = int32(processor.mem_fetch<int16>(address));
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LHE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000101101
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const int32 value = int32(processor.mem_fetch<int16>(address));
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LHU,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b10010100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint32 value = uint32(processor.mem_fetch<uint16>(address));
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LHUE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000101001
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint32 value = uint32(processor.mem_fetch<uint16>(address));
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LL,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000110110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      // ATOMIC
#pragma message("Set LLbit")

      const uint32 address = base.value<uint32>() + offset;
      const uint32 value = processor.mem_fetch<uint32>(address);
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LLE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000101110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      // ATOMIC
#pragma message("Set LLbit")

      const uint32 address = base.value<uint32>() + offset;
      const uint32 value = processor.mem_fetch<uint32>(address);
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LLWP,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b01111100000000000000000001110110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      // ATOMIC
#pragma message("Set LLbit")

      const uint32 address = base.value<uint32>();
      const uint64 value = processor.mem_fetch<uint64>(address);
      rt.set(uint32(value));
      rd.set(uint32(value >> 32));
      return true;

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LLWPE,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b01111100000000000000000001101110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      // ATOMIC
#pragma message("Set LLbit")

      const uint32 address = base.value<uint32>();
      const uint64 value = processor.mem_fetch<uint64>(address);
      rt.set(uint32(value));
      rd.set(uint32(value >> 32));
      return true;

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LSA,
      (OpFlags::None),
      0b11111100000000000000011100111111,
      0b00000000000000000000000000000101
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      const uint32 sa = TinyInt<2>(instruction >> 6).zextend<uint32>() + 1U;

      // Not sure if this is right. The docs say sign extend... but it also says to use logical shifts.
      const uint32 result = (rs.value<uint32>() << sa) + rt.value<uint32>();
      return write_result(rd, result);
   }

   ProcInstructionDef(
      LW,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b10001100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const int32 value = processor.mem_fetch<int32>(address);
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LWE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000101111
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const int32 value = processor.mem_fetch<int32>(address);
      return write_result(rt, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      LWPC,
      (OpFlags::None),
      0b11111100000110000000000000000000,
      0b11101100000010000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      GPRegister<21, 5> rs(instruction, processor);
      const int32 offset = TinyInt<21>(instruction << 2).sextend<int32>();

      const uint32 address = processor.get_program_counter() + offset;
      const int32 value = processor.mem_fetch<int32>(address);
      return write_result(rs, value);

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      MUL,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000010011000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint64 result = uint64(int64(rs.value<int32>()) * int64(rt.value<int32>()));

      return write_result(rd, uint32(result));
   }

   ProcInstructionDef(
      MUH,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000011011000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint64 result = uint64(int64(rs.value<int32>()) * int64(rt.value<int32>()));

      return write_result(rd, uint32(result >> 32));
   }

   ProcInstructionDef(
      MULU,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000010011001
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint64 result = uint64(uint64(rs.value<uint32>()) * uint64(rt.value<uint32>()));

      return write_result(rd, uint32(result));
   }

   ProcInstructionDef(
      MUHU,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000011011001
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint64 result = uint64(uint64(rs.value<uint32>()) * uint64(rt.value<uint32>()));

      return write_result(rd, uint32(result >> 32));
   }

   ProcInstructionDef(
      NAL,
      (OpFlags::ControlInstruction | OpFlags::SetNoCTI),
      0b11111111111111110000000000000000,
      0b00000100000100000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {      
      processor.set_link(processor.get_program_counter() + 8);
      processor.set_no_cti();

      return true;
   }

   ProcInstructionDef(
      NOP, // technically SLL $0, $0, 0 - given own slot for performance.
      (OpFlags::None),
      0b11111111111111111111111111111111,
      0b00000000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {      
      return true;
   }

   ProcInstructionDef(
      NOR,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000100111
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 result = ~(rs.value<uint32>() | rt.value<uint32>());

      return write_result(rd, result);
   }

   ProcInstructionDef(
      OR,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000100101
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 result = rs.value<uint32>() | rt.value<uint32>();

      return write_result(rd, result);
   }

   ProcInstructionDef(
      ORI,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b00110100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const uint32 offset = TinyInt<16>(instruction).zextend<uint32>();

      const uint32 result = rs.value<uint32>() | offset;

      return write_result(rt, result);
   }

   ProcInstructionDef(
      PAUSE,
      (OpFlags::ControlInstruction),
      0b11111111111111111111111111111111,
      0b00000000000000000000000101000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
#pragma message ("make wait for LLBit to clear")
      return true;
   }

   ProcInstructionDef(
      PREF,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000110101
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;

      //processor.mem_fetch<char>(address); // Does NOT throw address-related exceptions.

      return true;
   }

   ProcInstructionDef(
      PREFE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000100011
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;

      //processor.mem_fetch<char>(address); // Does NOT throw address-related exceptions.

      return true;
   }

   ProcInstructionDef(
      RDHWR,
      (OpFlags::None),
      0b11111111111000000000011000111111,
      0b01111100000000000000000000111011
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      GPRegister<16, 5> rt(instruction, processor);
      const GPRegister<11, 5> rd(instruction, processor);
      const uint32 selector = TinyInt<3>(instruction >> 6).zextend<uint32>();
      const uint32 reg_number = rd.get_register();

      if (reg_number == 29 && selector == 0)
      {
         return write_result(rt, processor.m_user_value);
      }
      else if (reg_number == 1 && selector == 0)
      {
         return write_result(rt, 0x100ul);
      }

      // only handle 0:1 and 0:29, as MUSL uses that.

#pragma message("Implement when COP0 is finished")
      throw CPU_Exception{ CPU_Exception::Type::Impl1, processor.get_program_counter() };
   }

   ProcInstructionDef(
      ROTR, // specialization of SRL
      (OpFlags::None),
      0b11111111111000000000000000111111,
      0b00000000001000000000000000000010
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      const uint32 sa = TinyInt<5>(instruction >> 6).zextend<uint32>();

      const uint32 rt_val = rt.value<uint32>();
      uint32 result = (rt_val >> sa) | (rt_val << (32 - sa));

      return write_result(rd, result);
   }

   ProcInstructionDef(
      ROTRV, // specialization of SRLV
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000001000110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 sa = rs.value<uint32>() & Bits(5);
      const uint32 rt_val = rt.value<uint32>();
      uint32 result = (rt_val >> sa) | (rt_val << (32 - sa));

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SB,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b10100000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint8 value = rt.value<uint8>();
      processor.mem_write(address, value);
      return true;

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SBE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000011100
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint8 value = rt.value<uint8>();
      processor.mem_write(address, value);
      return true;

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SC,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000100110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint8 value = rt.value<uint8>();
      rt.set<uint32>(1);
      processor.mem_write(address, value);
      return true;

#pragma message("Lots of times this should fail. Needs further implementation. This and LL.")
#pragma message("Should fail if address does not match preceding LL")
#pragma message("Implement LLbit weirdness")
#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SCE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000011110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint8 value = rt.value<uint8>();
      rt.set<uint32>(1);
      processor.mem_write(address, value);
      return true;

#pragma message("Lots of times this should fail. Needs further implementation. This and LLE.")
#pragma message("Should fail if address does not match preceding LLE")
#pragma message("Implement LLbit weirdness")
#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SCWP,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b01111100000000000000000001100110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const GPRegister<11, 5> rd(instruction, processor);

      const uint32 address = base.value<uint32>();
      const uint64 value = rt.value<uint32>() | (uint64(rd.value<uint32>()) << 32);
      rt.set<uint32>(1);
      processor.mem_write(address, value);
      return true;

#pragma message("Lots of times this should fail. Needs further implementation. This and LL.")
#pragma message("Should fail if address does not match preceding LL")
#pragma message("Implement LLbit weirdness")
#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SCWPE,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b01111100000000000000000001011110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const GPRegister<11, 5> rd(instruction, processor);

      const uint32 address = base.value<uint32>();
      const uint64 value = rt.value<uint32>() | (uint64(rd.value<uint32>()) << 32);
      rt.set<uint32>(1);
      processor.mem_write(address, value);
      return true;

#pragma message("Lots of times this should fail. Needs further implementation. This and LL.")
#pragma message("Should fail if address does not match preceding LL")
#pragma message("Implement LLbit weirdness")
#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SDBBP,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b00000000000000000000000000001110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const uint32 code = TinyInt<20>(instruction >> 6).zextend<uint32>();

#pragma message("should do special things with SRBI bit in COP0, or Debug bit")

      throw CPU_Exception{ CPU_Exception::Type::Bp, processor.get_program_counter(), code };
   }

   ProcInstructionDef(
      SEB, // specialization of BSHFL
      (OpFlags::None),
      0b11111111111000000000011111111111,
      0b01111100000000000000010000100000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const int32 result = rt.value<int8>();

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SEH, // specialization of BSHFL
      (OpFlags::None),
      0b11111111111000000000011111111111,
      0b01111100000000000000011000100000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const int32 result = rt.value<int16>();

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SELEQZ,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000110101
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 rt_val = rt.value<uint32>();
      const uint32 result = (rt_val != 0) ? 0 : rs.value<uint32>();

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SELNEZ,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000110111
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 rt_val = rt.value<uint32>();
      const uint32 result = (rt_val != 0) ? rs.value<uint32>() : 0;

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SH,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b10100100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint16 value = rt.value<uint16>();
      processor.mem_write(address, value);
      return true;

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SHE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000011101
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint16 value = rt.value<uint16>();
      processor.mem_write(address, value);
      return true;

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SIGRIE,
      (OpFlags::None),
      0b11111111111111110000000000000000,
      0b00000100000101110000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const uint32 code = TinyInt<16>(instruction).zextend<uint32>();

      throw CPU_Exception{ CPU_Exception::Type::RI, processor.get_program_counter(), code };
   }

   ProcInstructionDef(
      SLL,
      (OpFlags::None),
      0b11111111111000000000000000111111,
      0b00000000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      const uint32 sa = TinyInt<5>(instruction >> 6).zextend<uint32>();

      const uint32 result = rt.value<uint32>() << sa;

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SLLV,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000000100
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      const uint32 sa = rs.value<uint32>() & Bits(5);

      const uint32 result = rt.value<uint32>() << sa;

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SLT,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000101010
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const int32 rt_val = rt.value<int32>();
      const int32 rs_val = rs.value<int32>();
      const uint32 result = (rs_val < rt_val) ? 1 : 0;

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SLTI,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b00101000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 immediate = TinyInt<16>(instruction).sextend<int32>();

      const int32 rs_val = rs.value<int32>();
      const uint32 result = (rs_val < immediate) ? 1 : 0;

      return write_result(rt, result);
   }

   ProcInstructionDef(
      SLTIU,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b00101100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const uint32 immediate = uint32(TinyInt<16>(instruction).sextend<int32>()); // intended

      const uint32 rs_val = rs.value<uint32>();
      const uint32 result = (rs_val < immediate) ? 1 : 0;

      return write_result(rt, result);
   }

   ProcInstructionDef(
      SLTU,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000101011
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 rt_val = rt.value<uint32>();
      const uint32 rs_val = rs.value<uint32>();
      const uint32 result = (rs_val < rt_val) ? 1 : 0;

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SRA,
      (OpFlags::None),
      0b11111111111000000000000000111111,
      0b00000000000000000000000000000011
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      const uint32 sa = TinyInt<5>(instruction >> 6).zextend<uint32>();

      const int32 result = rt.value<int32>() >> sa;

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SRAV,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000000111
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      const uint32 sa = rs.value<uint32>() & Bits(5);

      const int32 result = rt.value<int32>() >> sa;

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SRL,
      (OpFlags::None),
      0b11111111111000000000000000111111,
      0b00000000000000000000000000000010
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      const uint32 sa = TinyInt<5>(instruction >> 6).zextend<uint32>();

      const uint32 result = rt.value<uint32>() >> sa;

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SRLV,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000000110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);
      const uint32 sa = rs.value<uint32>() & Bits(5);

      const uint32 result = rt.value<uint32>() >> sa;

      return write_result(rd, result);
   }

   ProcInstructionDef(
      SSNOP, // technically SLL $0, $0, 1 - given own slot for performance.
      (OpFlags::None),
      0b11111111111111111111111111111111,
      0b00000000000000000000000001000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {      
      return true;
   }

   ProcInstructionDef(
      SUB,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000100010
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const int64 result = int64(rs.value<int32>()) - rt.value<int32>();
      if (result > std::numeric_limits<int32>::max() || result < std::numeric_limits<int32>::min())
      {
         throw CPU_Exception{ CPU_Exception::Type::Ov, processor.get_program_counter() };
      }

      return write_result(rd, int32(result));
   }

   ProcInstructionDef(
      SUBU,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000100011
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 result = rs.value<uint32>() - rt.value<uint32>();
#pragma message("make sure we aren't supposed to clamp the value")

      return write_result(rd, int32(result));
   }

   ProcInstructionDef(
      SW,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b10101100000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint32 value = rt.value<uint32>();
      processor.mem_write(address, value);
      return true;

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SWE,
      (OpFlags::None),
      0b11111100000000000000000001111111,
      0b01111100000000000000000000011111
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const int32 offset = TinyInt<9>(instruction >> 7).sextend<int32>();

      const uint32 address = base.value<uint32>() + offset;
      const uint32 value = rt.value<uint32>();
      processor.mem_write(address, value);
      return true;

#pragma message("Implement Watch")
   }

   ProcInstructionDef(
      SYNC,
      (OpFlags::None),
      0b11111111111111111111100000111111,
      0b00000000000000000000000000001111
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      // Do Nothing
      return true;
   }

   ProcInstructionDef(
      SYNCI,
      (OpFlags::None),
      0b11111100000111110000000000000000,
      0b00000100000111110000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> base(instruction, processor);
      const int32 offset = TinyInt<16>(instruction).sextend<int32>();
      
      const uint32 address = base.value<uint32>() + offset;

      processor.mem_poke<char>(address); // We still want to touch the address.

      return true;
   }

   ProcInstructionDef(
      SYSCALL,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b00000000000000000000000000001100
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const uint32 code = TinyInt<20>(instruction >> 6).zextend<uint32>();

      throw CPU_Exception{ CPU_Exception::Type::Sys, processor.get_program_counter(), code };
   }

   ProcInstructionDef(
      TEQ,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b00000000000000000000000000110100
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

      if (rs.value<int32>() == rt.value<int32>())
      {
         throw CPU_Exception{ CPU_Exception::Type::Tr, processor.get_program_counter(), code };
      }

      return true;
   }

   ProcInstructionDef(
      TGE,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b00000000000000000000000000110000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

      if (rs.value<int32>() >= rt.value<int32>())
      {
         throw CPU_Exception{ CPU_Exception::Type::Tr, processor.get_program_counter(), code };
      }

      return true;
   }

   ProcInstructionDef(
      TGEU,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b00000000000000000000000000110001
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

      if (rs.value<uint32>() >= rt.value<uint32>())
      {
         throw CPU_Exception{ CPU_Exception::Type::Tr, processor.get_program_counter(), code };
      }

      return true;
   }

   ProcInstructionDef(
      TLT,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b00000000000000000000000000110010
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

      if (rs.value<int32>() < rt.value<int32>())
      {
         throw CPU_Exception{ CPU_Exception::Type::Tr, processor.get_program_counter(), code };
      }

      return true;
   }

   ProcInstructionDef(
      TLTU,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b00000000000000000000000000110011
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

      if (rs.value<uint32>() < rt.value<uint32>())
      {
         throw CPU_Exception{ CPU_Exception::Type::Tr, processor.get_program_counter(), code };
      }

      return true;
   }

   ProcInstructionDef(
      TNE,
      (OpFlags::None),
      0b11111100000000000000000000111111,
      0b00000000000000000000000000110110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      const uint32 code = TinyInt<10>(instruction >> 6).zextend<uint32>();

      if (rs.value<int32>() != rt.value<int32>())
      {
         throw CPU_Exception{ CPU_Exception::Type::Tr, processor.get_program_counter(), code };
      }

      return true;
   }

   ProcInstructionDef(
      WSBH,
      (OpFlags::None),
      0b11111111111000000000011111111111,
      0b01111100000000000000000010100000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      // Swap bytes _within each halfword_
      union register_alias
      {
         uint32 reg;
         uint8 bytes[4];
      } reg;
      reg.reg = rt.value<uint32>();

      std::swap(reg.bytes[0], reg.bytes[1]);
      std::swap(reg.bytes[2], reg.bytes[3]);

      return write_result(rd, reg.reg);
   }

   ProcInstructionDef(
      XOR,
      (OpFlags::None),
      0b11111100000000000000011111111111,
      0b00000000000000000000000000100110
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      const GPRegister<16, 5> rt(instruction, processor);
      GPRegister<11, 5> rd(instruction, processor);

      const uint32 result = rs.value<uint32>() ^ rt.value<uint32>();

      return write_result(rd, result);
   }

   ProcInstructionDef(
      XORI,
      (OpFlags::None),
      0b11111100000000000000000000000000,
      0b00111000000000000000000000000000
   ) (instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
   {
      const GPRegister<21, 5> rs(instruction, processor);
      GPRegister<16, 5> rt(instruction, processor);
      const uint32 offset = TinyInt<16>(instruction).zextend<uint32>();

      const uint32 result = rs.value<uint32>() ^ offset;

      return write_result(rt, result);
   }
}
