#include "pch.hpp"

#include "global_state.hpp"


namespace mips::global_state
{
	namespace
	{
		static thread_local mips::processor* __restrict current_processor = nullptr;
		static thread_local mips::jit_base* __restrict current_jit = nullptr;
	}

	_pure _nothrow processor::type processor::get_current() noexcept
	{
		return current_processor;
	}

	_nothrow void processor::set_current(const type processor) noexcept
	{
		current_processor = processor;
	}

	_pure _nothrow jit::type jit::get_current() noexcept
	{
		return current_jit;
	}

	_nothrow void jit::set_current(const type jit) noexcept
	{
		current_jit = jit;
	}
}
