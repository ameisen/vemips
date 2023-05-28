#include "buildcarbide.hpp"

#include "file_utils_win32.hpp"

#include <memory>

#define PLATFORM_HEADER_WITH_IO 1
#include "platform/platform_headers.hpp"
#undef PLATFORM_HEADER_WITH_IO
#include "platform/win32/win32_system_handle.hpp"

using namespace buildcarbide;

namespace {
	struct file_closer final {
		no_throw void operator()(FILE *__restrict fp) const __restrict noexcept {
			check(fclose(fp) == 0);
		}
	};

	struct file_ptr final : std::unique_ptr<FILE, file_closer> {
		no_throw file_ptr(const wchar_t *const __restrict path, const wchar_t *const __restrict mode) noexcept :
			std::unique_ptr<FILE, file_closer>(_wfopen(path, mode)) {}
	};
}

no_throw std::optional<file_utils::modtime_t> file_utils::get_file_time(const filetime time_type, FILE *__restrict file) noexcept {
	const int file_number = _fileno(file);
	const auto handle = HANDLE(_get_osfhandle(file_number));  // NOLINT(performance-no-int-to-ptr)
	if (handle == INVALID_HANDLE_VALUE) [[unlikely]] {
		return 0;
	}

	FILETIME time;
	if (
		const BOOL result = GetFileTime(
			handle, 
			time_type == filetime::created ? &time : nullptr, 
			time_type == filetime::accessed ? &time : nullptr,
			time_type == filetime::modified ? &time : nullptr
		);
		result == FALSE
		) [[unlikely]] {
			return 0;
	}

		// TODO should we close 'handle' or something?

		return ULARGE_INTEGER{ {
			.LowPart = time.dwLowDateTime,
			.HighPart = time.dwHighDateTime
		} }.QuadPart;
}

no_throw std::optional<file_utils::modtime_t> file_utils::get_file_time(const filetime time_type, const std::wstring &__restrict filename) noexcept {
	if (const file_ptr file = { filename.c_str(), L"rb" }; bool(file) == true) [[likely]] {
		const auto result = get_file_time(time_type, file.get());
		return result;
	}

	return {};
}

/*
no_throw uint64_t get_file_time(const wstring &__restrict filename, const uint64_t def = uint64_t(-1)) noexcept {
	const file_handle file_handle = CreateFile(
		filename.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (!file_handle) [[unlikely]] {
		return def;
	}

	FILETIME file_time;
	if (GetFileTime(HANDLE(file_handle), nullptr, nullptr, &file_time) == 0) [[unlikely]] {
		return def;
	}

		return ULARGE_INTEGER{ {
			.LowPart = file_time.dwLowDateTime,
			.HighPart = file_time.dwHighDateTime
		} }.QuadPart;
}
*/
