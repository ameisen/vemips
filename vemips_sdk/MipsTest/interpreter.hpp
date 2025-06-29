#pragma once

#define FMT_OPTIMIZE_SIZE 1

#include <cstdio>
#include <exception>

#define FMT_HEADER_ONLY 1
#include "../../ext/fmt/include/fmt/core.h"
#include "../../ext/fmt/include/fmt/xchar.h"

// ReSharper disable IdentifierTypo
#if defined(__GNUC__) || defined(__clang__)
# if defined(NDEBUG) || defined(__OPTIMIZE__)
#   ifndef NDEBUG
#     define NDEBUG 1  // NOLINT(clang-diagnostic-unused-macros)
#   endif
#   undef DEBUG
# elif defined(DEBUG) || !defined(__OPTIMIZE__)
#   undef NDEBUG
#   ifndef DEBUG
#     define DEBUG 1  // NOLINT(clang-diagnostic-unused-macros)
#   endif
# endif
#else
# if defined(NDEBUG)
#   ifndef NDEBUG
#     define NDEBUG 1  // NOLINT(clang-diagnostic-unused-macros)
#   endif
#   undef DEBUG
# elif defined(DEBUG)
#   undef NDEBUG
#   ifndef DEBUG
#     define DEBUG 1  // NOLINT(clang-diagnostic-unused-macros)
#   endif
# else
#   ifndef NDEBUG
#     define NDEBUG 1  // NOLINT(clang-diagnostic-unused-macros)
#   endif
#   undef DEBUG
#
# endif
#endif

#ifdef _MSC_VER
#	define __builtin_unreachable() __assume(0)
#endif

#define MIPSTEST_STRINGIZE_DETAIL(x) #x  // NOLINT(clang-diagnostic-unused-macros)
#define MIPSTEST_STRINGIZE(x) MIPSTEST_STRINGIZE_DETAIL(x)  // NOLINT(clang-diagnostic-unused-macros)

#if defined(__clang__) || (defined(__GNUC__) && __GNUC_MAJOR__ >= 10)
# define MIPSTEST_HAS_BUILTIN(expr) __has_builtin(expr)
#else
# undef MIPSTEST_HAS_BUILTIN
#endif

#if defined(DEBUG)
# if defined(_MSC_VER)
#  define MIPSTEST_DEBUGBREAK() __debugbreak()
# elif defined(MIPSTEST_HAS_BUILTIN) && MIPSTEST_HAS_BUILTIN(__builtin_debugtrap)
#  define MIPSTEST_DEBUGBREAK() __builtin_debugtrap()
# elif defined(MIPSTEST_HAS_BUILTIN) && MIPSTEST_HAS_BUILTIN(__builtin_trap)
#  define MIPSTEST_DEBUGBREAK() __builtin_trap()
# elif __has_include(<signal.h>)
#  include <signal.h>
#  define MIPSTEST_DEBUGBREAK() raise(SIGTRAP)
# else
#  define MIPSTEST_DEBUGBREAK() do { *((int*)nullptr) = 0; } while(false)
# endif

# define MIPSTEST_ASSERT_VAL(condition, expr, message) \
  [&] () noexcept { \
    if (!(condition)) [[unlikely]] { \
      fmt::println(stderr, "Assertion `" #expr "` failed at " __FILE__ ":" MIPSTEST_STRINGIZE(__LINE__) " :: " message); \
      MIPSTEST_DEBUGBREAK(); \
      std::terminate(); \
    } \
    return true; \
  }()

// ReSharper disable once CppInconsistentNaming
# define UNREACHABLE() do { MIPSTEST_ASSERT(false && "unreachable", "unreachable point reached"); __builtin_unreachable(); } while(true)
# define ASSUME(expr) MIPSTEST_ASSERT(expr, "assume check failed")
#else
# if defined(__INTELLISENSE__) || defined(_MSC_VER)
#  define UNREACHABLE() __assume(0)
#  define ASSUME(expr) __assume(expr)
# elif defined(__GNUC__) || defined(__clang__)
#  define UNREACHABLE() __builtin_unreachable()
#  if __clang__
#    define ASSUME(expr) __builtin_assume(expr)
#  else
#    define ASSUME(expr) do { if (!(expr)) __builtin_unreachable(); } while(0)
#  endif
# else
#  define UNREACHABLE()
#  define ASSUME(expr)
# endif

# define MIPSTEST_ASSERT_VAL(condition, expr, message)
#endif

#define MIPSTEST_ASSERT(expr, message) \
  MIPSTEST_ASSERT_VAL(!(expr), expr, message)
// ReSharper restore IdentifierTypo

#if defined(__INTELLISENSE__) || defined(_MSC_VER)
# define NO_THROW __declspec(nothrow)
#elif defined(__GNUC__) || defined(__clang__)
# define NO_THROW __attribute__((__nothrow__))
#else
# pragma message("Unknown Architecture")
# define NO_THROW
#endif

// ReSharper disable CppInconsistentNaming
#if defined(NDEBUG)
# if defined(__GNUC__) || defined(__clang__)
#   define check(expr) ({ ASSUME(expr); true; })  // NOLINT(clang-diagnostic-assume)
# else
#   define check(expr) [&] () noexcept { ASSUME(expr); return true; }()  // NOLINT(clang-diagnostic-assume)
# endif
#else
# if defined(__GNUC__) || defined(__clang__)
#  define check(expr) ({ const bool cond = !!(expr); MIPSTEST_ASSERT_VAL(cond, expr, "check failed"); cond; })
# else
#  define check(expr) [&] () noexcept -> bool { const bool cond = !!(expr); MIPSTEST_ASSERT_VAL(cond, expr, "check failed"); return cond; }()
# endif
#endif
// ReSharper restore CppInconsistentNaming
