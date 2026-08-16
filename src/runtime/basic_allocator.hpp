#pragma once

#include <array>
#include <bit>
#include <mutex>
#include <utility>
#include <vector>

#include "common.hpp"


template <typename T>
class contiguous_allocator final {
	static constexpr const usize chunk_size = 0x100; // or 0x40 for cache line
	static constexpr const usize chunk_count = std::max(usize(1), chunk_size / sizeof(T));

	using chunk_array_t = std::array<T, chunk_count>;

	static constexpr const usize alignment = 1UZ << std::bit_width(std::max({chunk_size, sizeof(T), sizeof(chunk_array_t)}) - 1UZ);

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

	[[nodiscard]]
	_allocator
	T* allocate() noexcept(std::is_nothrow_constructible_v<T>) {
		std::unique_lock lock{lock_};

		if _likely(!free_.empty()) [[likely]] {
			auto index = free_.back();
			free_.pop_back();
			T* result = &allocated_[index.chunk]->value[index.element];
			return std::construct_at(result);
		}

		free_.reserve(free_.size() + chunk_count - 1);

		const usize chunk_index = allocated_.size();

		auto* new_chunk = allocated_.emplace_back(std::make_unique<chunk_t>()).get();
		xassert(chunk_index <= std::numeric_limits<uint32>::max());
		for (uint32 i = 1; i < chunk_count; ++i) {
			free_.emplace_back(uint32(chunk_index), i);
		}

		return std::construct_at(new_chunk->value.data());
	}

	void release(const T* __restrict ptr) noexcept(std::is_nothrow_destructible_v<T>) {
		if _unlikely(!ptr) [[unlikely]] {
			return;
		}

		std::unique_lock lock{lock_};

		const usize allocated_size = allocated_.size();
		xassert(allocated_size <= std::numeric_limits<uint32>::max());
		for (uint32 i = 0; i < allocated_size; ++i) {
			chunk_t* chunk = allocated_[i].get();
			const intptr index = intptr(ptr - chunk->value.data());
			if (index < 0 || index > std::numeric_limits<uint32>::max() || std::cmp_greater_equal(index, chunk_count)) {
				continue;
			}

			std::destroy_at(ptr);
			free_.emplace_back(i, uint32(index));
		}
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
	[[nodiscard]]
	_forceinline _allocator T* allocate() __restrict noexcept(std::is_nothrow_constructible_v<T>) {
		return new T;
	}

	// ReSharper disable once CppMemberFunctionMayBeStatic
	_forceinline void release(const T* __restrict ptr) __restrict noexcept(std::is_nothrow_destructible_v<T>) {
		delete ptr;
	}
};

template <typename T> using basic_allocator = contiguous_allocator<T>;

