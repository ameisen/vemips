#pragma once

#include <common.hpp>


namespace vemips::platform
{
	[[nodiscard]]
	_func_const _nothrow uint64 get_host_frequency() noexcept;
	_nothrow void set_process_high_priority() noexcept;
	_nothrow void set_console_min_width(int32 width) noexcept;
}
