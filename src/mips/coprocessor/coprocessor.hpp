#pragma once

#include "common.hpp"

namespace mips
{
   class processor;
   class coprocessor
   {
   protected:
      processor & __restrict m_processor;
   public:
      coprocessor(processor & __restrict processor) : m_processor(processor) {}
      virtual ~coprocessor() {}

      virtual void clock() __restrict = 0;
   };
}

#include "coprocessor1/coprocessor1.hpp"
