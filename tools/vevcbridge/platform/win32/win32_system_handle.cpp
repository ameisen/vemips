#include "buildcarbide.hpp"

#include "win32_system_handle.hpp"

#include "platform/platform_headers.hpp"

no_throw void buildcarbide::detail::close_handle(handle_t& handle, const uintptr_t invalid_value) noexcept {
	if (handle != handle_t(invalid_value)) [[likely]] {  // NOLINT(performance-no-int-to-ptr)
		check(CloseHandle(HANDLE(handle)) != FALSE);
		handle = handle_t(invalid_value); // NOLINT(performance-no-int-to-ptr)
	}
}
