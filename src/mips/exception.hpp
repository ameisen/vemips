#pragma once

#include <common.hpp>

namespace mips
{
   // TODO refactor

   struct CPU_Exception
   {
   public:
      enum class Type : uint32_t
      {
         Interrupt = 0,
         Mod = 1,
         TLBL = 2,
         TLBS = 3,
         AdEL = 4,
         AdES = 5,
         IBE = 6,
         DBE = 7,
         Sys = 8,
         Bp = 9,
         RI = 10,
         CpU = 11,
         Ov = 12,
         Tr = 14,
         FPE = 15,
         Impl1 = 16,
         Impl2 = 17,
         C2E = 18,
         TLBRI = 19,
         TLBXI = 20,
         MDMX = 22,
         WATCH = 23,
         MCheck = 24,
         Thread = 25,
         DPSPDis = 26,
         GE = 27,
         Prot = 29,
         CacheErr = 30,
      } m_ExceptionType;
      uint32 m_InstructionAddress;
      uint32 m_Code;
   };

   struct ExecutionCompleteException {};
   struct ExecutionFailException {};
}
