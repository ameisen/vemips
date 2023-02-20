#pragma once

#ifdef VEMIPS_INCLUDE_SYSTEM_HEADER
#	undef VEMIPS_INCLUDE_SYSTEM_HEADER
#endif
#define VEMIPS_INCLUDE_SYSTEM_HEADER 0

#if _WIN32
#	include "windows.inc"
#else
#	error Unknown Platform
#endif

#undef VEMIPS_INCLUDE_SYSTEM_HEADER
