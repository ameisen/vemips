#pragma once

#include <cstdio>
#include <exception>

// ReSharper disable IdentifierTypo
#if defined(__GNUC__)
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

#define MIPSTEST_STRINGIZE_DETAIL(x) #x  // NOLINT(clang-diagnostic-unused-macros)
#define MIPSTEST_STRINGIZE(x) MIPSTEST_STRINGIZE_DETAIL(x)  // NOLINT(clang-diagnostic-unused-macros)

#if defined(DEBUG)

#if defined(_MSC_VER)
# define MIPSTEST_DEBUGBREAK() __debugbreak()
#else
# define MIPSTEST_DEBUGBREAK()
#endif

# define MIPSTEST_ASSERT(expr, message) \
  [&] () noexcept { \
    if (!(expr)) [[unlikely]] { \
      std::fputs("Assertion `" #expr "` failed at " __FILE__ ":" MIPSTEST_STRINGIZE(__LINE__) " :: " message, stderr); \
      MIPSTEST_DEBUGBREAK(); \
      std::terminate(); \
    } \
    return true; \
  }()

// ReSharper disable once CppInconsistentNaming
# define UNREACHABLE() MIPSTEST_ASSERT(false, "unreachable point reached")
# define ASSUME(expr) MIPSTEST_ASSERT(expr, "assume check failed")
#else
# if defined(__INTELLISENSE__) || defined(_MSC_VER)
#  define UNREACHABLE() __assume(0)
#  define ASSUME(expr) __assume(expr)
# else
#  define UNREACHABLE() __builtin_unreachable()
#  if __clang__
#    define ASSUME(expr) __builtin_assume(expr)
#  else
#    define ASSUME(expr) do { if (!(expr)) __builtin_unreachable(); } while(0)
#  endif
# endif
#endif
// ReSharper restore IdentifierTypo

// ReSharper disable CppInconsistentNaming
#if defined(NDEBUG)
# define check(expr) [&] () noexcept { ASSUME(expr); return true; }()
#else
# if defined(__GNUC__) || defined(__clang__)
#  define check(expr) ({ const bool cond = !!(expr); MIPSTEST_ASSERT((expr), "check failed"); cond; })
# else
#  define check(expr) [&] () noexcept { const bool cond = !!(expr); MIPSTEST_ASSERT((expr), "check failed"); return cond; }()
# endif
#endif
// ReSharper restore CppInconsistentNaming
