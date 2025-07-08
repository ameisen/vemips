#include "pch.hpp"

#undef WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <PowrProf.h>

#include "platform.hpp"


using namespace mips::platform;

uint64 mips::platform::get_host_frequency() {
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
	GetSystemInfo(&info);

	const auto power_info = std::make_unique<PROCESSOR_POWER_INFORMATION[]>(info.dwNumberOfProcessors);

	const size_t out_buffer_length = info.dwNumberOfProcessors * sizeof(PROCESSOR_POWER_INFORMATION);
	xassert(out_buffer_length <= std::numeric_limits<ULONG>::max());

	if (const NTSTATUS status = CallNtPowerInformation(
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
