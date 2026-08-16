#include "pch.hpp"

#include "statistics.hpp"


_nothrow void mips::statistics::append(const statistics& __restrict other) noexcept
{
	xassert(&other != this);

	for (const auto&[name, count] : other.instructions)
	{
		instructions[name] += count;
	}

	for (const auto&[name, count] : other.jit_emulated_instructions)
	{
		jit_emulated_instructions[name] += count;
	}

	jit_transitions = other.jit_transitions;
}