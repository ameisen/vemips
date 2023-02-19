#pragma once

// TABLEGEN is 1 if we are building the tablegen. It is 0/undefined otherwise.

#define WIDE_INSTRUCTION_BRANCHES 1

#if TABLEGEN
#  define USE_STATIC_INSTRUCTION_SEARCH 0
#else
#  define USE_STATIC_INSTRUCTION_SEARCH 1
#endif
#define USE_GOOGLE_DENSE_HASH_MAP 0
#define USE_GOOGLE_SPARSE_HASH_MAP 0
#define USE_HASH_MAP 1

static constexpr const bool ScriptMode = false;
