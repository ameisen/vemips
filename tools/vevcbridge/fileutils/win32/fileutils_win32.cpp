#include "fileutils_win32.hpp"

#ifdef NOMINMAX
#	undef NOMINMAX
#endif
#define NOMINMAX 1
#include <Windows.h>
#include <io.h>

using namespace buildcarbide;

fileutils::modtime_t fileutils::get_file_modtime(FILE * __restrict fp)
{
	const int file_number = _fileno(fp);
	const auto handle = HANDLE(_get_osfhandle(file_number));  // NOLINT(performance-no-int-to-ptr)
	if (handle == INVALID_HANDLE_VALUE) {
		return 0;
	}

	FILETIME write_time;
	if (
		const BOOL result = GetFileTime(handle, nullptr, nullptr, &write_time);
		result == 0
	) {
		return 0;
	}

	// TODO should we close 'handle' or something?

	const ULARGE_INTEGER time_value = {
		{
			.LowPart = write_time.dwLowDateTime,
			.HighPart = write_time.dwHighDateTime
		}
	};

	return time_value.QuadPart;
}

fileutils::modtime_t fileutils::get_file_modtime(const std::string & __restrict filename) {
	FILE * __restrict fp = fopen(filename.c_str(), "rb");
	if (!fp) {
		return 0;
	}
	const auto ret = get_file_modtime(fp);
	fclose(fp);
	return ret;
}
