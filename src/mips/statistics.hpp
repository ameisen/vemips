#pragma once

#include "mips/config.hpp"
#include <common.hpp>

#include <unordered_map>


namespace mips
{
	struct statistics final
	{
		struct name_hash final
		{
			[[nodiscard]]
			_func_const _nothrow _forceinline usize operator()(const char* const name) const noexcept
			{
				return reinterpret_cast<usize>(name);
			}
		};

		using instruction_map = std::unordered_map<const char*, size_t, name_hash>;

		instruction_map instructions;
		instruction_map jit_emulated_instructions;
		usize jit_transitions = 0;

		_nothrow void append(const statistics& __restrict other) noexcept;
	};
}
