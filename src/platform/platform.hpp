#pragma once

#define VEMIPS_INCLUDE_SYSTEM_HEADER 1

#if _WIN32
#	include "windows.inc"
#else
#	error Unknown Platform
#endif

#undef VEMIPS_INCLUDE_SYSTEM_HEADER
