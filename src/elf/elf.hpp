#pragma once

#include "../common.hpp"

#include <span>
#include <string>
#include <unordered_map>
#include <vector>


namespace mips::elf {
	class binary final {
		class data_stream final {
			const char* __restrict data_;
			usize size_;

			template <usize N>
			_forceinline void check_size(const usize size, const char (& __restrict message)[N]) const __restrict {
				if _unlikely(size_ < size) [[unlikely]] {
					throw std::runtime_error(message);
				}
			}

		public:
			data_stream(const std::span<char> data) :
				data_(data.data()),
				size_(data.size())
			{}

			template <typename T>
			_forceinline T pop() __restrict {
				check_size(sizeof(T), "Out Of Range ELF Binary Seek");

				const auto* __restrict data_pointer = data_;
				data_ += sizeof(T);
				size_ -= sizeof(T);

				T result;
				std::memcpy(&result, data_pointer, sizeof(T));
				return result;
			}

			_forceinline void skip(const usize size) __restrict {
				check_size(size, "Out Of Range ELF Binary Seek");

				data_ += size;
				size_ -= size;
			}

			template <typename T>
			_forceinline void extract(T &out) __restrict {
				check_size(sizeof(T), "Out Of Range ELF Binary Seek");

				std::memcpy(&out, data_, sizeof(T));
				data_ += sizeof(T);
				size_ -= sizeof(T);
			}

			// TODO : Technically UB
			template <typename T>
			_forceinline const T& reference() __restrict {
				check_size(sizeof(T), "Out Of Range ELF Binary Seek");

				const T & __restrict value = *static_cast<const T * __restrict>(data_);
				data_ += sizeof(T);
				size_ -= sizeof(T);
				return value;
			}

			// TODO : Technically UB
			template <typename T>
			[[nodiscard]]
			_forceinline const T& offset(const usize offset) const __restrict {
				check_size(offset + sizeof(T), "Out Of Range ELF Binary Read");

				return *reinterpret_cast<const T * __restrict>(data_ + offset);
			}

			[[nodiscard]]
			_forceinline const char* address_offset(const usize offset) const __restrict {
				check_size(offset, "Out Of Range ELF Binary Read");

				return data_ + offset;
			}
		};

		template <typename PtrType>
		void read_binary(data_stream& __restrict stream_data);
	public:
		template <typename T>
		struct extent final {
			T offset;
			T size;

			_nothrow _forceinline T end_offset() const noexcept {
				return offset + size;
			}
		};

		struct section final {
			std::string         name;
			extent<usize_guest> file_extent;
			extent<usize_guest> memory_extent;
			bool                executable : 1 = false;
			bool                zero_init : 1 = false;

			_nothrow _forceinline bool operator < (const section& __restrict section) const noexcept {
				return memory_extent.offset < section.memory_extent.offset;
			}
		};

		const data_stream																	raw_data_stream_;
		std::vector<section>															sections_;
		std::unordered_map<std::string, const section*>	named_sections_;

		uptr_guest															entry_address_;
		uptr_guest															program_headers_;
		usize_guest															program_headers_size_;
		usize_guest															program_header_count_;

		binary(std::span<char> binary_data);
	};
}
