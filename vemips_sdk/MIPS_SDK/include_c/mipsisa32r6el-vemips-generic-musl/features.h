#ifndef _FEATURES_H
#define _FEATURES_H

/* vemips */

#define _MUSL_VEMIPS 1
#define __MUSL_VER_MAJOR__ 1
#define __MUSL_VER_MINOR__ 2
#define __MUSL_VER_PATCH__ 5

/*
#ifndef _XOPEN_SOURCE
#	define _XOPEN_SOURCE 700
#endif
#ifndef _POSIX_SOURCE
#	define _POSIX_SOURCE 1
#endif
#ifndef _GNU_SOURCE
#	define _GNU_SOURCE 1
#endif
#ifndef _POSIX_C_SOURCE
#	define _POSIX_C_SOURCE 1
#endif

#ifndef __GNUC_PREREQ
#	define __GNUC_PREREQ(x, y) 1
#endif
#ifndef __GLIBC_PREREQ
#	define __GLIBC_PREREQ(x, y) 1
#endif
*/

/* vemips */

#if defined(_ALL_SOURCE) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif

#if defined(_DEFAULT_SOURCE) && !defined(_BSD_SOURCE)
#define _BSD_SOURCE 1
#endif

#if !defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE) \
 && !defined(_XOPEN_SOURCE) && !defined(_GNU_SOURCE) \
 && !defined(_BSD_SOURCE) && !defined(__STRICT_ANSI__)
#define _BSD_SOURCE 1
#define _XOPEN_SOURCE 700
#endif

#if __STDC_VERSION__ >= 199901L
#define __restrict restrict
#elif !defined(__GNUC__)
#define __restrict
#endif

#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
#define __inline inline
#elif !defined(__GNUC__)
#define __inline
#endif

#if __STDC_VERSION__ >= 201112L
#elif defined(__GNUC__)
#define _Noreturn __attribute__((__noreturn__))
#else
#define _Noreturn
#endif

#define __REDIR(x,y) __typeof__(x) x __asm__(#y)

#include "debug.h"

#endif
