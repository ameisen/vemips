#pragma once

#include "common.hpp"
#include <array>
#include "basic_allocator.hpp"

namespace mips {
	template<typename T, bool Own, int Size, size_t... Sizes>
	class directory_table final {
		static constexpr const size_t levels = sizeof...(Sizes) + 1;
		static_assert(levels != 0);
		static_assert((sizeof...(Sizes) + 1) == levels);
		static_assert(Size != 0);

		using sub_map_t = directory_table<T, Own, Sizes...>;

		static inline basic_allocator<sub_map_t> allocator_;

		std::array<sub_map_t* __restrict, Size> data_ = { nullptr };

	public:
		_forceinline _nothrow directory_table() = default;
		_forceinline _nothrow directory_table(const directory_table&) = delete;
		_forceinline _nothrow directory_table(directory_table&& table) noexcept : data_(table.data_) {
			table.data_ = {};
		}

		_forceinline _nothrow ~directory_table() {
			for (auto* __restrict ptr : data_) {
				allocator_.release(ptr);
			}
		}

		_forceinline directory_table& operator = (const directory_table&) = delete;
		_forceinline directory_table& operator = (directory_table&& table) noexcept {
			data_ = table.data_;
			table.data_ = {};
			return *this;
		}

		[[nodiscard]]
		_forceinline _nothrow bool contains(const uint32 idx) const {
			return data_[idx] != nullptr;
		}

		_forceinline _nothrow sub_map_t& operator [] (uint32 idx) {
			sub_map_t* __restrict result = data_[idx];
			if (!result) {
				result = data_[idx] = allocator_.allocate();
			}
			return *result;
		}
	};

	template <typename T, bool Own, size_t Size>
	class directory_table<T, Own, Size> final {
		static_assert(Size != 0);

		std::array<T, Size> data_ = {};

	public:
		_forceinline _nothrow directory_table() = default;
		_forceinline _nothrow directory_table(const directory_table&) = delete;
		_forceinline _nothrow directory_table(directory_table&& table) noexcept : data_(table.data_) {
			table.data_ = {};
		}

		_forceinline _nothrow ~directory_table() {
			if constexpr (Own) {
				for (const auto& element : data_) {
					element.release();
				}
			}
		}

		_forceinline directory_table& operator = (const directory_table&) = delete;
		_forceinline directory_table& operator = (directory_table&& table) noexcept {
			data_ = table.data_;
			table.data_ = {};
			return *this;
		}

		[[nodiscard]]
		_forceinline _nothrow bool contains(const uint32 idx) const {
			return data_[idx].contained();
		}

		_forceinline _nothrow T& operator [] (uint32 idx) {
			T& __restrict result = data_[idx];
			if (!result.contained()) {
				result.initialize();
			}
			return result;
		}
	};

}
