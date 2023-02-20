// ReSharper disable CppClangTidyModernizeMacroToEnum CppMissingIncludeGuard IdentifierTypo

#ifndef NOMINMAX
#	undef NOMINMAX
#endif
#define NOMINMAX 1

#ifdef WIN32_LEAN_AND_MEAN
#	undef WIN32_LEAN_AND_ME
#endif
#define WIN32_LEAN_AND_ME 1

#if VEMIPS_INCLUDE_SYSTEM_HEADER
#	include <windows.h>
#endif
