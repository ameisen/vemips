#pragma once

#include "common.hpp"
#include <array>
#include "basic_allocator.hpp"

#define USING_TABLE 1

namespace mips {
	template<typename T, size_t Size, size_t... Sizes>
	class directory_table final {
		static_assert(Size != 0);
		static_assert(log2_ceil(Size) == log2_floor(Size));

	public:
		static constexpr const usize index_bits = 32;
		static constexpr const usize size = Size;
		static constexpr const usize size_bits = log2_ceil(Size);
		static constexpr const uint32 index_mask = (1U << size_bits) - 1U;
		static constexpr const bool has_sub_tables = true;

	private:
		static constexpr const size_t levels = sizeof...(Sizes) + 1;
		static_assert(levels != 0);
		static_assert((sizeof...(Sizes) + 1) == levels);
		static_assert(Size != 0);

		static constexpr const usize total_size_bits = size_bits + (log2_ceil(Sizes) + ...);
		static constexpr const usize remaining_size_bits = index_bits - total_size_bits;

		using sub_map_t = directory_table<T, Sizes...>;

		static inline basic_allocator<sub_map_t> allocator_;

		struct deleter final
		{
			void operator()(sub_map_t* ptr)
			{
				allocator_.release(ptr);
			}
		};

		std::array<std::unique_ptr<sub_map_t, deleter>, Size> data_ = { nullptr };

	public:
		_forceinline _nothrow directory_table() noexcept = default;
		directory_table(const directory_table&) = delete;
		_forceinline _nothrow directory_table(directory_table&& table) noexcept :
			data_(std::move(table.data_))
		{
			table.data_ = {};
		}

		_forceinline _nothrow ~directory_table() noexcept = default;

		directory_table& operator = (const directory_table&) = delete;
		_forceinline directory_table& operator = (directory_table&& table) noexcept {
			data_ = std::move(table.data_);
			table.data_ = {};
			return *this;
		}

		[[nodiscard]]
		_forceinline _nothrow bool contains_sub(const uint32 idx) const noexcept {
			xassert(idx < Size);

			return static_cast<bool>(data_[idx]);
		}

		_forceinline _nothrow sub_map_t& sub(const uint32 idx) noexcept {
			xassert(idx < Size);

			sub_map_t* __restrict result = data_[idx].get();
			if _unlikely(!result) [[unlikely]] {
				result = allocator_.allocate();
				data_[idx].reset(result);
			}
			return *result;
		}

		_forceinline const sub_map_t& sub(const uint32 idx) const {
			xassert(idx < Size);

			sub_map_t* __restrict result = data_[idx].get();
			if _unlikely(!result) [[unlikely]] {
				exceptions::throw_assert<std::runtime_error>("directory_table sub-table does not exist at given index");
			}
			return *result;
		}

		[[nodiscard]]
		_forceinline _nothrow bool contains(const uint32 idx) const noexcept {
			const uint32 table_index = (idx >> (index_bits - size_bits - remaining_size_bits)) & index_mask;
			xassert(table_index < Size);

			const auto& sub_table_ptr = data_[table_index];
			if (!sub_table_ptr) [[unlikely]]
			{
				return false;
			}
			
			return sub_table_ptr->contains(idx);
		}

		// second element is `true` if element existed
		template <typename Self>
		_forceinline auto get(this Self&& self, const uint32 idx) noexcept(std::is_const_v<Self>) ->
		std::pair<_make_qual(T&), bool> {
			const uint32 table_index = (idx >> (index_bits - size_bits - remaining_size_bits)) & index_mask;
			xassert(table_index < Size);

			auto&& sub_table_ptr = self.sub(table_index);

			return sub_table_ptr.get(idx);
		}

		template <typename Self>
		_forceinline _nothrow auto fetch(this Self&& self, const uint32 idx) noexcept -> _make_qual(T*) {
			const uint32 table_index = (idx >> (index_bits - size_bits - remaining_size_bits)) & index_mask;
			xassert(table_index < Size);

			if (!self.contains_sub(table_index))
			{
				return nullptr;
			}

			auto&& sub_table_ptr = self.sub(table_index);

			return sub_table_ptr.fetch(idx);
		}

		template <typename Self>
		_forceinline auto operator [] (this Self&& self, const uint32 idx) noexcept(std::is_const_v<Self>) ->
		_make_qual(T&) {
			const uint32 table_index = (idx >> (index_bits - size_bits - remaining_size_bits)) & index_mask;
			xassert(table_index < Size);

			auto&& sub_table_ptr = self.sub(table_index);

			return sub_table_ptr[idx];
		}
	};

	template <typename T, size_t Size>
	class directory_table<T, Size> final {
		static_assert(Size != 0);
		static_assert(log2_ceil(Size) == log2_floor(Size));

		std::array<T, Size> data_ = {};

	public:
		static constexpr const usize index_bits = 32;
		static constexpr const usize size = Size;
		static constexpr const usize size_bits = log2_ceil(Size);
		static constexpr const uint32 index_mask = (1U << size_bits) - 1U;
		static constexpr const bool has_sub_tables = false;

	private:
		static constexpr const usize total_size_bits = size_bits;
		static constexpr const usize remaining_size_bits = index_bits - total_size_bits;

	public:
		_forceinline _nothrow directory_table() noexcept = default;
		directory_table(const directory_table&) = delete;
		_forceinline _nothrow directory_table(directory_table&& table) noexcept :
			data_(std::move(table.data_))
		{
			table.data_ = {};
		}

		_forceinline _nothrow ~directory_table() noexcept {
			for (auto& element : data_) {
				element.release();
			}
		}

		_forceinline directory_table& operator = (const directory_table&) = delete;
		_forceinline _nothrow directory_table& operator = (directory_table&& table) noexcept {
			data_ = std::move(table.data_);
			table.data_ = {};
			return *this;
		}

		[[nodiscard]]
		// ReSharper disable once CppMemberFunctionMayBeStatic
		_forceinline _nothrow bool contains_sub([[maybe_unused]] const uint32 idx) const noexcept {
			return false;
		}

		template <typename Self>
		[[noreturn]]
		_forceinline void sub([[maybe_unused]] this Self&& self, [[maybe_unused]] const uint32 idx) noexcept(false) {
			exceptions::throw_assert<std::runtime_error>("directory_table sub-table does not exist at given index");
		}

		[[nodiscard]]
		_forceinline _nothrow bool contains(const uint32 idx) const noexcept {
			const uint32 index = (idx >> (index_bits - size_bits - remaining_size_bits)) & index_mask;
			xassert(index < Size);

			return data_[index].contained();
		}

		// second element is `true` if element existed
		template <typename Self>
		_forceinline auto get(this Self&& self, const uint32 idx) noexcept(std::is_const_v<Self>) ->
		std::pair<_make_qual(T&), bool> {
			const uint32 index = (idx >> (index_bits - size_bits - remaining_size_bits)) & index_mask;
			xassert(index < Size);

			T& __restrict result = self.data_[index];
			const bool existed = result.contained();
			if _unlikely(!existed) [[unlikely]] {
				if constexpr (!std::is_const_v<Self>)
				{
					result.initialize();
				}
				else
				{
					exceptions::throw_assert<std::runtime_error>("directory_table element does not exist at given index");
				}
			}
			return { result, existed };
		}

		template <typename Self>
		_forceinline _nothrow auto fetch(this Self&& self, const uint32 idx) noexcept -> _make_qual(T*) {
			const uint32 index = (idx >> (index_bits - size_bits - remaining_size_bits)) & index_mask;
			xassert(index < Size);

			_make_qual(T&) result = std::forward<Self>(self).data_[index];
			const bool existed = result.contained();
			if _unlikely(!existed) [[unlikely]] {
				return nullptr;
			}
			return &result;
		}

		template <typename Self>
		_forceinline auto operator [] (this Self&& self, const uint32 idx) noexcept(std::is_const_v<Self>) -> _make_qual(T&) {
			const uint32 index = (idx >> (index_bits - size_bits - remaining_size_bits)) & index_mask;
			xassert(index < Size);

			T& __restrict result = self.data_[index];
			if _unlikely(!result.contained()) [[unlikely]] {
				if constexpr (!std::is_const_v<Self>)
				{
					result.initialize();
				}
				else
				{
					exceptions::throw_assert<std::runtime_error>("directory_table element does not exist at given index");
				}
			}
			return result;
		}
	};

	template<typename T, [[maybe_unused]] size_t Size>
	class directory_std_map final
	{
		static_assert(Size != 0);
		static_assert(log2_ceil(Size) == log2_floor(Size));

		std::unordered_map<uint32, T> map_;
	public:
		static constexpr const usize index_bits = 32;
		static constexpr const usize size = Size;
		static constexpr const usize size_bits = log2_ceil(Size);
		static constexpr const uint32 index_mask = (1U << size_bits) - 1U;
		static constexpr const bool has_sub_tables = false;

	public:
		_forceinline _nothrow directory_std_map() noexcept = default;
		directory_std_map(const directory_std_map&) = delete;
		_forceinline _nothrow directory_std_map(directory_std_map&& table) noexcept :
			map_(std::move(table.map_))
		{
		}

		_forceinline _nothrow ~directory_std_map() noexcept {
			for (auto& element : map_) {
				element.second.release();
			}
		}

		_forceinline directory_std_map& operator = (const directory_std_map&) = delete;
		_forceinline _nothrow directory_std_map& operator = (directory_std_map&& table) noexcept {
			map_ = std::move(table.map_);
			return *this;
		}

		[[nodiscard]]
		// ReSharper disable once CppMemberFunctionMayBeStatic
		_forceinline _nothrow bool contains_sub([[maybe_unused]] const uint32 idx) const noexcept {
			return false;
		}

		template <typename Self>
		[[noreturn]]
		_forceinline void sub([[maybe_unused]] this Self&& self, [[maybe_unused]] const uint32 idx) noexcept(false) {
			exceptions::throw_assert<std::runtime_error>("directory_std_map sub-table does not exist at given index");
		}

		[[nodiscard]]
		_forceinline _nothrow bool contains(const uint32 idx) const noexcept {
			return map_.contains(idx);
		}

		// second element is `true` if element existed
		template <typename Self>
		_forceinline auto get(this Self&& self, const uint32 idx) noexcept(std::is_const_v<Self>) ->
		std::pair<_make_qual(T&), bool> {

			// TODO: can be done more optimally
			const auto&& f_iter = self.map_.find(idx);
			if _unlikely(f_iter == self.map_.end()) [[unlikely]]
			{
				if constexpr (!std::is_const_v<Self>)
				{
					auto& result = self.map_[idx];//.emplace(idx).first->second;
					result.initialize();
					return { result, false };
				}
				else
				{
					exceptions::throw_assert<std::runtime_error>("directory_std_map element does not exist at given index");
				}
			}

			T& __restrict result = f_iter->second;
			const bool existed = result.contained();
			if _unlikely(!existed) [[unlikely]] {
				if constexpr (!std::is_const_v<Self>)
				{
					result.initialize();
				}
				else
				{
					exceptions::throw_assert<std::runtime_error>("directory_std_map element does not exist at given index");
				}
			}
			return { result, existed };
		}

		template <typename Self>
		_forceinline _nothrow auto fetch(this Self&& self, const uint32 idx) noexcept -> _make_qual(T*) {
			const auto&& f_iter = self.map_.find(idx);

			if _unlikely(f_iter == self.map_.end() || !f_iter->second.contained()) [[unlikely]]
			{
				return nullptr;
			}

			return &f_iter->second;
		}

		template <typename Self>
		_forceinline auto operator [] (this Self&& self, const uint32 idx) noexcept(std::is_const_v<Self>) -> _make_qual(T&) {
			return self.get(idx).first;
		}
	};

#if USING_TABLE
	template<typename T, size_t Size, size_t... Sizes>
	using directory_map = directory_table<T, Size, Sizes...>;
#else
	template<typename T, size_t Size, [[maybe_unused]] size_t... Sizes>
	using directory_map = directory_std_map<T, Size>;
#endif

}
