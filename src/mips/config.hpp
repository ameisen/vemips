#pragma once


// VEMIPS_TABLEGEN is 1 if we are building the tablegen.
#ifndef VEMIPS_TABLEGEN
#	define VEMIPS_TABLEGEN 0
#endif

#define USE_STATIC_INSTRUCTION_SEARCH !VEMIPS_TABLEGEN
#define USE_GOOGLE_DENSE_HASH_MAP 0
#define USE_GOOGLE_SPARSE_HASH_MAP 0
#define USE_HASH_MAP 1
#define WIDE_INSTRUCTION_BRANCHES 1
#define ALWAYS_REFRESH_BRANCH_TARGET 1
#define SET_RESULT_REGISTER_ON_SYSCALL_EXIT 1

namespace mips
{
	static constexpr const bool ScriptMode = false;
}
