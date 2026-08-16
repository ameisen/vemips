#include "pch.hpp"
#include "platform.hpp"
#include <common.hpp>

#define NOMINMAX 1
#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <PowrProf.h>
#include <powerbase.h>

#include <limits>
#include <memory>


using namespace vemips::platform;

_func_const _nothrow uint64 vemips::platform::get_host_frequency() noexcept {
	// ReSharper disable CppInconsistentNaming
	struct PROCESSOR_POWER_INFORMATION final {
		ULONG Number;
		ULONG MaxMhz;
		ULONG CurrentMhz;
		ULONG MhzLimit;
		ULONG MaxIdleState;
		ULONG CurrentIdleState;
	};
	// ReSharper restore CppInconsistentNaming

	SYSTEM_INFO info = {};
	::GetSystemInfo(&info);

	const auto power_info = std::make_unique<PROCESSOR_POWER_INFORMATION[]>(info.dwNumberOfProcessors);

	const size_t out_buffer_length = info.dwNumberOfProcessors * sizeof(PROCESSOR_POWER_INFORMATION);
	xassert(out_buffer_length <= std::numeric_limits<ULONG>::max());

	if (const NTSTATUS status = ::CallNtPowerInformation(
		ProcessorInformation,
		nullptr,
		0,
		power_info.get(),
		ULONG(out_buffer_length)
	); status != 0) {
		return 1;
	}

	// TODO : Also set the max performance state temporarily.

	return power_info[0].MaxMhz;
}

_nothrow void vemips::platform::set_process_high_priority() noexcept
{
	const HANDLE process = ::GetCurrentProcess();

	::SetPriorityClass(process, HIGH_PRIORITY_CLASS);
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	ULONG_PTR process_affinity, system_affinity;
	if (
		const BOOL result = ::GetProcessAffinityMask(
			process,
			&process_affinity,
			&system_affinity
		);
		result != FALSE && process_affinity != 1
	) {
		// stop the process from being placed onto the first CPU
		process_affinity &= ~1ull;
		::SetProcessAffinityMask(process, process_affinity);
		Yield();
	}
}

_nothrow void vemips::platform::set_console_min_width(const int32 width) noexcept
{
	//std::puts("\x{1B}[?1049h");
	//std::puts("\x{1B}[?3h");
	/*
	const SHORT short_width = mips::checked_cast<SHORT>(width);
	xassert(short_width > 0);

	const HANDLE console = ::GetStdHandle(STD_OUTPUT_HANDLE);

	if (console == INVALID_HANDLE_VALUE) [[unlikely]]
	{
		return;
	}

	CONSOLE_SCREEN_BUFFER_INFO buffer_info { 0 };
	if (FALSE == ::GetConsoleScreenBufferInfo(console, &buffer_info)) [[unlikey]]
	{
		return;
	}

	if (
		constexpr SMALL_RECT temp_rect {
			0, 0, 0, 0
		};
		FALSE == ::SetConsoleWindowInfo(console, TRUE, &temp_rect)
	) [[unlikely]]
	{
		return;
	}

	const COORD buffer_size {
		.X = std::max(buffer_info.dwSize.X, short_width),
		.Y = buffer_info.dwSize.Y
	};
	if (FALSE == ::SetConsoleScreenBufferSize(console, buffer_size)) [[unlikely]]
	{
		return;
	}

	const int extent_x = buffer_info.srWindow.Right - buffer_info.srWindow.Left;
	
	const SMALL_RECT new_rect {
		.Left = buffer_info.srWindow.Left,
		.Top = buffer_info.srWindow.Top,
		.Right = mips::checked_cast<SHORT>(std::max(extent_x, short_width - 1)),
		.Bottom = buffer_info.srWindow.Bottom
	};
	if (FALSE == ::SetConsoleWindowInfo(console, TRUE, &new_rect)) [[unlikely]]
	{
		return;
	}
	*/
}
