#pragma once

#include "common.hpp"

#include "jit1.hpp"


namespace Xbyak
{
	class Label;	
}

namespace mips
{
	class Jit1_CodeGen;

	namespace jit1_common::branch
	{
		extern void emit_local_jmp(
			Jit1_CodeGen& cg,
			const jit1::ChunkOffset& __restrict chunk_offset,
			uint32 target_offset,
			uint32 current_offset
		);
	}
}
