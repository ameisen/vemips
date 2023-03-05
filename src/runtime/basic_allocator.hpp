#pragma once

#include "common.hpp"
#include <array>
#include <vector>

template <typename T>
class contiguous_allocator final {
	static constexpr const size_t chunk_size = 0x1000; // or 0x40 for cache line
	static constexpr const size_t chunk_count = std::max(size_t(1), chunk_size / sizeof(T));

	using chunk_array_t = std::array<T, chunk_count>;

	static constexpr const size_t alignment = std::max(std::max(chunk_size, sizeof(T)), sizeof(chunk_array_t));

	struct alignas(alignment) chunk_t final {
		chunk_array_t value;
	};

	struct free_index final {
		uint32 chunk;
		uint32 element;
	};

	std::vector<chunk_t*> allocated; // todo - need a contiguous allocator for 'chunk_t'
	std::vector<free_index> free;

public:
	_allocator
	T* allocate() {
		if _likely(!free.empty()) [[likely]] {
			auto index = free.back();
			free.pop_back();
			T* result = &allocated[index.chunk]->value[index.element];
			new (result) T();
			return result;
		}

		free.reserve(free.size() + chunk_count - 1);
		auto* new_chunk = new chunk_t;
		allocated.push_back(new_chunk);
		uint32 chunk_index = allocated.size() - 1;
		for (uint32 i = 1; i < chunk_count; ++i) {
			free.push_back({ chunk_index, i });
		}

		return new (new_chunk->value.data()) T();
	}

	void release(const T* __restrict ptr) {
		if _unlikely(!ptr) [[unlikely]] {
			return;
		}

		for (size_t i = 0; i < allocated.size(); ++i) {
			chunk_t* chunk = allocated[i];
			const intptr index = intptr(ptr - chunk->value.data());
			if (index < 0 || index >= chunk_count) {
				continue;
			}

			ptr->~T();
			free.push_back({ uint32(i), uint32(index) });
			return;
		}
		xassert(false);
	}
};

template <typename T>
class standard_allocator final {
public:
	_forceinline _allocator T* allocate() __restrict {
		return new T;
	}

	_forceinline void release(const T* __restrict ptr) __restrict {
		delete ptr;
	}
};

template <typename T> using basic_allocator = contiguous_allocator<T>;

