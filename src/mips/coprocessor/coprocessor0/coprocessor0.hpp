#pragma once

#include "pch.hpp"
#include "common.hpp"
#include "mips/coprocessor/coprocessor.hpp"

namespace mips
{
   class coprocessor0 : public coprocessor
   {
   private:
      struct UserLocal // R4S2
      {
         uint32 val;
      };
      struct SegCtl0 // R5S2
      {
         uint32 val;
      };
      struct SegCtl1 // R5S3
      {
         uint32 val;
      };
      struct SegCtl2 // R5S4
      {
         uint32 val;
      };
      struct HWREna // R7S0
      {
         uint32 val;
      };
      struct BadVAddr // R8S0
      {
         uint32 val;
      };
      struct BadInstr // R8S1
      {
         uint32 val;
      };
      struct BadInstrP // R8S2
      {
         uint32 val;
      };
      struct Count // R9S0
      {
         uint32 val;
      };
      struct Compare // R11S0
      {
         uint32 val;
      };
      struct Status // R12S0
      {
         uint32 val;
      };
      struct IntCtl // R12S1
      {
         uint32 val;
      };
      struct SRSCtl // R12S2
      {
         uint32 val;
      };
      struct SRSMap // R12S3
      {
         uint32 val;
      };
      struct Cause // R13S0
      {
         uint32 val;
      };
      struct EPC // R14S0
      {
         uint32 val;
      };
      struct PRId // R15S0
      {
         uint32 val;
      };
      struct EBase // R15S1
      {
         uint32 val;
      };
      struct Config // R16S0
      {
         uint32 val;
      };
      struct Config1 // R16S1
      {
         uint32 val;
      };
      struct LLAddr // R17S0
      {
         uint32 val;
      };
      struct WatchLo {};// R18Sn
      struct WatchHi {};// R19Sn
      struct PerfCnt {};// R25Sn
      struct ErrorEPC //R30S0
      {
      };
      struct KScratch {};// R31S2-7

      FIR                                       m_fir;
      FCSR                                      m_fcsr;

   public:
      FIR & get_FIR() __restrict
      {
         return m_fir;
      }

      FCSR & get_FCSR() __restrict
      {
         return m_fcsr;
      }

      coprocessor0(processor & __restrict processor) : coprocessor(processor) {}
      virtual ~coprocessor0() {}

      virtual void clock() __restrict;

      // Get the register as a specific type
      template <typename T>
      T get_register(uint32 idx) const __restrict
      {
         // Strict-aliasing rules apply
         static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
         return ((T *)&m_registers[idx])[0];
      }

      // Set the register from a given type
      template <typename T>
      void set_register(uint32 idx, T value) __restrict
      {
         // Strict-aliasing rules apply
         static_assert(sizeof(T) <= sizeof(register_type), "get_register is casting to invalid size");
         ((T *)&m_registers[idx])[0] = value;
      }
   };
}
