#pragma once

#include <common.hpp>

#include "instructions_common.hpp"

namespace mips
{
   extern const instructions::InstructionInfo * __restrict FindExecuteInstruction(instruction_t instruction);
}
