// ReSharper disable IdentifierTypo
#pragma once

#ifndef NOMINMAX
#	define NOMINMAX 1
#	define VEVC_NOMINMAX_DEFINED 1
#endif
#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN 1
#	define VEVC_LEAN_AND_MEAN_DEFINED 1
#endif

#include <Windows.h>

#if defined(PLATFORM_HEADER_WITH_IO) && PLATFORM_HEADER_WITH_IO != 0
#	include <io.h>
#endif

#if defined(PLATFORM_HEADER_WITH_SHELL) && PLATFORM_HEADER_WITH_SHELL != 0
#	include <shellapi.h>
#endif

#if defined(PLATFORM_HEADER_WITH_SHLOBJ) && PLATFORM_HEADER_WITH_SHLOBJ != 0
#	include <ShlObj.h>
#endif

#ifdef VEVC_NOMINMAX_DEFINED
#	undef NOMINMAX
#	undef VEVC_NOMINMAX_DEFINED
#endif
#ifdef VEVC_LEAN_AND_MEAN_DEFINED
#	undef WIN32_LEAN_AND_MEAN
#	undef VEVC_LEAN_AND_MEAN_DEFINED
#endif
