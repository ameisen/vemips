#pragma once

#include "common.hpp"
#include <array>
#include "basic_allocator.hpp"

template<typename T, bool Own, int Size, size_t... Sizes>
class leveled_map final {
	static constexpr const size_t Levels = sizeof...(Sizes) + 1;
	static_assert(Levels != 0);
	static_assert((sizeof...(Sizes) + 1) == Levels);
	static_assert(Size != 0);

	using sub_map_t = leveled_map<T, Own, Sizes...>;

	static inline basic_allocator<sub_map_t> allocator;

	std::array<sub_map_t* __restrict, Size> data_ = { nullptr };

public:
	_forceinline _nothrow ~leveled_map() {
		for (auto * __restrict ptr : data_) {
			allocator.release(ptr);
		}
	}

	[[nodiscard]]
	_forceinline _nothrow bool contains(const uint32 idx) const {
		return data_[idx] != nullptr;
	}

	_forceinline _nothrow sub_map_t& operator [] (uint32 idx) {
		sub_map_t* __restrict result = data_[idx];
		if (!result) {
			result = data_[idx] = allocator.allocate();
		}
		return *result;
	}
};

template <typename T, bool Own, size_t Size>
class leveled_map<T, Own, Size> final {
	static_assert(Size != 0);

	std::array<T, Size> data_ = {};

public:
	_forceinline _nothrow ~leveled_map() {
		if constexpr (Own) {
			for (const auto & element : data_) {
				element.release();
			}
		}
	}

	[[nodiscard]]
	_forceinline _nothrow bool contains(const uint32 idx) const {
		return data_[idx].contained();
	}

	_forceinline _nothrow T& operator [] (uint32 idx) {
		T & __restrict result = data_[idx];
		if (!result.contained()) {
			result.initialize();
		}
		return result;
	}
};
