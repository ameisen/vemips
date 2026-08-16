#pragma once

#include <atomic>
#include <bit>
#include <cstring>

#define MUSL_WITH_VEMIPS 1
#define MUSL_VEMIPS_WITHOUT_LOCKS 1
#define SET_UNINITIALIZED 1

#if defined(__GNUC__) && defined(__PIC__)
#define inline inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define inline inline __forceinline
#endif

template <typename T>
static constexpr inline T uninitialized() noexcept {
#if SET_UNINITIALIZED
	T result;
	std::memset(&result, 0xFE, sizeof(result));
	return result;
#else
	return {};
#endif
}

struct chunk {
	size_t psize, csize;
	struct chunk *next, *prev;
};

struct bin {
	#if !MUSL_VEMIPS_WITHOUT_LOCKS
	volatile int lock[2];
	#endif
	struct chunk *head;
	struct chunk *tail;
};

#define SIZE_ALIGN (4*sizeof(size_t))
#define SIZE_MASK (-SIZE_ALIGN)
#define OVERHEAD (2*sizeof(size_t))
#define MMAP_THRESHOLD (0x1c00*SIZE_ALIGN)
#define DONTCARE 16
#define RECLAIM 163840

#define CHUNK_SIZE(c) ((c)->csize & -2)
#define CHUNK_PSIZE(c) ((c)->psize & -2)
#define PREV_CHUNK(c) ((struct chunk *)((char *)(c) - CHUNK_PSIZE(c)))
#define NEXT_CHUNK(c) ((struct chunk *)((char *)(c) + CHUNK_SIZE(c)))
#define MEM_TO_CHUNK(p) (struct chunk *)((char *)(p) - OVERHEAD)
#define CHUNK_TO_MEM(c) (void *)((char *)(c) + OVERHEAD)
#define BIN_TO_CHUNK(i) (MEM_TO_CHUNK(&mal.bins[i].head))

#define C_INUSE  ((size_t)1)

#if _MSC_VER && !__clang__
#	define a_crash() std::abort()
#else
#	define a_crash() __builtin_trap()
#endif

static inline int a_ctz_32(uint32_t x);

static inline int a_cas(volatile int *p, int t, int s)
{
	auto v = *p;
	if (v == t) {
		*p = s;
	}
	return v;
}

static inline void a_or_64(volatile uint64_t *p, uint64_t v)
{
	*p |= v;
}

static inline int a_fetch_and(volatile int *p, int v)
{
	auto old = *p;
	*p = v;
	return old;
}

static inline void a_and(volatile int *p, int v)
{
	a_fetch_and(p, v);
}

static inline void a_and_64(volatile uint64_t *p, uint64_t v)
{
	union { uint64_t v; uint32_t r[2]; } u = { v };
	if (u.r[0]+1) a_and((int *)p, u.r[0]);
	if (u.r[1]+1) a_and((int *)p+1, u.r[1]);
}

static inline int a_clz_32(uint32_t x)
{
	x >>= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return 31-a_ctz_32(x);
}

static inline int a_clz_64(uint64_t x)
{
#ifdef a_clz_32
	if (x>>32)
		return a_clz_32(x>>32);
	return a_clz_32(x) + 32;
#else
	uint32_t y = uninitialized<uint32_t>();
	int r = uninitialized<int>();
	if (x>>32) y=x>>32, r=0; else y=x, r=32;
	if (y>>16) y>>=16; else r |= 16;
	if (y>>8) y>>=8; else r |= 8;
	if (y>>4) y>>=4; else r |= 4;
	if (y>>2) y>>=2; else r |= 2;
	return r | !(y>>1);
#endif
}

static inline int a_ctz_32(uint32_t x)
{
#ifdef a_clz_32
	return 31-a_clz_32(x&-x);
#else
	static const char debruijn32[32] = {
		0, 1, 23, 2, 29, 24, 19, 3, 30, 27, 25, 11, 20, 8, 4, 13,
		31, 22, 28, 18, 26, 10, 7, 12, 21, 17, 9, 6, 16, 5, 15, 14
	};
	return debruijn32[(x&-x)*0x076be629 >> 27];
#endif
}

static inline int a_ctz_64(uint64_t x)
{
	static const char debruijn64[64] = {
		0, 1, 2, 53, 3, 7, 54, 27, 4, 38, 41, 8, 34, 55, 48, 28,
		62, 5, 39, 46, 44, 42, 22, 9, 24, 35, 59, 56, 49, 18, 29, 11,
		63, 52, 6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
		51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12
	};
	if (sizeof(long) < 8) {
		uint32_t y = x;
		if (!y) {
			y = x>>32;
			return 32 + a_ctz_32(y);
		}
		return a_ctz_32(y);
	}
	return debruijn64[(x&-x)*0x022fdd63cc95386dull >> 58];
}
