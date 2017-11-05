#pragma once

#include <common.hpp>
#include "mips/system.hpp"

namespace mips
{
   class system_vemix : public system
   {
   private:
      virtual uint32 handle_exception(const CPU_Exception &ex) __restrict override final;
   public:
      system_vemix(const options &init_options, const elf::binary & __restrict binary);
      virtual ~system_vemix() override final;

      virtual void clock(uint64 clocks = 0) __restrict override final;
   };
}
