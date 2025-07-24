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

#include <io.h>
#include <shellapi.h>
#include <ShlObj.h>

#ifdef VEVC_NOMINMAX_DEFINED
#	undef NOMINMAX
#	undef VEVC_NOMINMAX_DEFINED
#endif
#ifdef VEVC_LEAN_AND_MEAN_DEFINED
#	undef WIN32_LEAN_AND_MEAN
#	undef VEVC_LEAN_AND_MEAN_DEFINED
#endif
