#pragma once

#include <array>
#include <mutex>
#include <utility>
#include <vector>

#include "common.hpp"


template <typename T>
class contiguous_allocator final {
	static constexpr const size_t chunk_size = 0x100; // or 0x40 for cache line
	static constexpr const size_t chunk_count = std::max(size_t(1), chunk_size / sizeof(T));

	using chunk_array_t = std::array<T, chunk_count>;

	static constexpr const size_t alignment = 1ULL << mips::log2_ceil(std::max({chunk_size, sizeof(T), sizeof(chunk_array_t)}));

	struct alignas(alignment) chunk_t final {
		chunk_array_t value;
	};

	struct free_index final {
		uint32 chunk;
		uint32 element;
	};

	std::mutex lock_;
	std::vector<std::unique_ptr<chunk_t>> allocated_; // todo - need a contiguous allocator for 'chunk_t'
	std::vector<free_index> free_;

public:
	contiguous_allocator() = default;
	contiguous_allocator(const contiguous_allocator&) = delete;
	contiguous_allocator(contiguous_allocator&&) = default;

	contiguous_allocator& operator=(const contiguous_allocator&) = delete;
	contiguous_allocator& operator=(contiguous_allocator&&) = default;

	~contiguous_allocator() = default;

	_allocator
	T* allocate() {
		std::unique_lock lock{lock_};

		if _likely(!free_.empty()) [[likely]] {
			auto index = free_.back();
			free_.pop_back();
			T* result = &allocated_[index.chunk]->value[index.element];
			return std::construct_at(result);
		}

		free_.reserve(free_.size() + chunk_count - 1);

		const size_t chunk_index = allocated_.size();

		auto* new_chunk = allocated_.emplace_back(std::make_unique<chunk_t>()).get();
		xassert(chunk_index <= std::numeric_limits<uint32>::max());
		for (uint32 i = 1; i < chunk_count; ++i) {
			free_.emplace_back(uint32(chunk_index), i);
		}

		return std::construct_at(new_chunk->value.data());
	}

	void release(const T* __restrict ptr) {
		if _unlikely(!ptr) [[unlikely]] {
			return;
		}

		std::unique_lock lock{lock_};

		const size_t allocated_size = allocated_.size();
		xassert(allocated_size <= std::numeric_limits<uint32>::max());
		for (uint32 i = 0; i < allocated_size; ++i) {
			chunk_t* chunk = allocated_[i].get();
			const intptr index = intptr(ptr - chunk->value.data());
			if (index < 0 || index > std::numeric_limits<uint32>::max() || std::cmp_greater_equal(index, chunk_count)) {
				continue;
			}

			std::destroy_at(ptr);
			free_.emplace_back(i, uint32(index));
			return;
		}
		xassert(false);
	}
};

template <typename T>
class standard_allocator final {
public:
	standard_allocator() = default;
	standard_allocator(const standard_allocator&) = delete;
	standard_allocator(standard_allocator&&) = default;

	~standard_allocator() = default;

	standard_allocator& operator=(const standard_allocator&) = delete;
	standard_allocator& operator=(standard_allocator&&) = default;

public:
	// ReSharper disable once CppMemberFunctionMayBeStatic
	_forceinline _allocator T* allocate() __restrict {
		return new T;
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	_forceinline void release(const T* __restrict ptr) __restrict {
		delete ptr;
	}
};

template <typename T> using basic_allocator = contiguous_allocator<T>;

