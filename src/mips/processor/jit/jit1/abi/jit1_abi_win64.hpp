#pragma once

#include "common.hpp"


namespace mips::jit::abi::win64
{
	static constexpr const usize stack_alignment = 16;
	static constexpr const usize caller_stack_reserve = 32;
	static constexpr const usize callee_stack_reserve = 0;
}
