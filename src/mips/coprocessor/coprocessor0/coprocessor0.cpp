#include "pch.hpp"
#include "coprocessor0.hpp"

using namespace mips;

void coprocessor0::clock() __restrict
{
   m_fir.clock();
   m_fcsr.clock();
}

namespace
{
   static const coprocessor0::FIR g_reference_fir;
   static const coprocessor0::FCSR g_reference_fcsr_ones = []() -> coprocessor0::FCSR {
      coprocessor0::FCSR value;
      memset(&value, 0x0, sizeof(value));
      value.NAN2008 = 1;
      value.ABS2008 = 1;
      return value;
   }();
   static const coprocessor0::FCSR g_reference_fcsr_zeros = []() -> coprocessor0::FCSR {
      coprocessor0::FCSR value;
      memset(&value, 0xFF, sizeof(value));
      value._Z0 = 0;
      value._Z1 = 0;
      value._Z2 = 0;
      return value;
   }();
}

coprocessor0::FIR::FIR()
{
   *(uint32 * __restrict)this = 0;

   Revision = 0b00000001;
   ProcessorID = 0b11111111;
   SinglePrecision = 0b1;
   DoublePrecision = 0b1;
   FixedPointW = 0b0;
   FixedPointL = 0b0;
   Float64 = 0b1;
   Has2008 = 0b1;
   FREP = 0b0;
}

void coprocessor0::FIR::clock() __restrict
{
   // FIR is supposed to be read-only.
   *this = g_reference_fir;
}

coprocessor0::FCSR::FCSR()
{
   *(uint32 * __restrict)this = 0;

   RoundingMode = 0;
   Flags = 0;
   Enables = 0;
   Cause = 0;
   NAN2008 = 1;
   ABS2008 = 1;
   Impl = 0;
   FlushZero = 1;
}

void coprocessor0::FCSR::clock() __restrict
{
   *(uint32 * __restrict)this |= uint32(g_reference_fcsr_ones);
   *(uint32 * __restrict)this &= uint32(g_reference_fcsr_zeros);
}

uint32 coprocessor0::FCSR::get_FEXR() const __restrict
{
   uint32 ret = 0;
   ((FCSR & __restrict)ret).Flags = Flags;
   ((FCSR & __restrict)ret).Cause = Cause;
   return ret;
}

void coprocessor0::FCSR::set_FEXR(uint32 fexr) __restrict
{
   Flags = ((const FCSR & __restrict)fexr).Flags;
   Cause = ((const FCSR & __restrict)fexr).Cause;
}

uint32 coprocessor0::FCSR::get_FENR() const __restrict
{
   uint32 ret = 0;
   ((FCSR & __restrict)ret).RoundingMode = RoundingMode;
   ((FCSR & __restrict)ret).Enables = Enables;
   ((FCSR & __restrict)ret).FlushZero = FlushZero;
   return ret;
}

void coprocessor0::FCSR::set_FENR(uint32 fenr) __restrict
{
   RoundingMode = ((const FCSR & __restrict)fenr).RoundingMode;
   Enables = ((const FCSR & __restrict)fenr).Enables;
   FlushZero = ((const FCSR & __restrict)fenr).FlushZero;
}
