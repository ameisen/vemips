#include "fileutils_win32.hpp"

#define NOMINMAX 1
#include <Windows.h>
#include <io.h>

using namespace buildcarbide;

fileutils::modtime_t buildcarbide::fileutils::get_file_modtime(FILE * __restrict fp)
{
	const int filenum = _fileno(fp);
	HANDLE handle = HANDLE(_get_osfhandle(filenum));
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	FILETIME write_time;
	BOOL result = GetFileTime(handle, nullptr, nullptr, &write_time);
	if (result == 0)
	{
		return 0;
	}

	// TODO should we close 'handle' or something?

	LARGE_INTEGER time_value;
	time_value.HighPart = write_time.dwHighDateTime;
	time_value.LowPart = write_time.dwLowDateTime;

	return time_value.QuadPart;
}

fileutils::modtime_t buildcarbide::fileutils::get_file_modtime(const std::string & __restrict filename)
{
	FILE * __restrict fp = fopen(filename.c_str(), "rb");
	if (!fp)
	{
		return 0;
	}
	auto ret = get_file_modtime(fp);
	fclose(fp);
	return ret;
}
