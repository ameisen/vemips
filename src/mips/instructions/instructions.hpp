#pragma once

#include <common.hpp>

#include "instructions_common.hpp"
#include "mips/mips_common.hpp"


namespace mips {
	extern _nothrow const instructions::InstructionInfo* FindExecuteInstruction(instruction_t instruction) noexcept;
}
