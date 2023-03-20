#pragma once

#include "common.hpp"

#include <vector>
#include <exception>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <span>

namespace elf {
	class binary final {
		class data_stream final {
			const char * __restrict data_;
			size_t size_;

			template <size_t N>
			_forceinline void check_size(const size_t size, const char (& __restrict message)[N]) const __restrict {
				if _unlikely(size_ < size) [[unlikely]] {
					throw std::runtime_error(message);
				}
			}

		public:
			data_stream(const std::span<char> data) : data_(data.data()), size_(data.size()) {}

			template <typename T>
			T pop() __restrict {
				check_size(sizeof(T), "Out Of Range ELF Binary Seek");

				const auto* __restrict data_pointer = data_;
				data_ += sizeof(T);
				size_ -= sizeof(T);

				T result;
				memcpy(&result, data_pointer, sizeof(T));
				return result;
			}

			void skip(const size_t size) __restrict {
				check_size(size, "Out Of Range ELF Binary Seek");

				data_ += size;
				size_ -= size;
			}

			template <typename T>
			void extract(T &out) __restrict {
				check_size(sizeof(T), "Out Of Range ELF Binary Seek");

				memcpy(&out, data_, sizeof(T));
				data_ += sizeof(T);
				size_ -= sizeof(T);
			}

			// TODO : Technically UB
			template <typename T>
			const T & reference() __restrict {
				check_size(sizeof(T), "Out Of Range ELF Binary Seek");

				const T & __restrict value = *(const T * __restrict)data_;
				data_ += sizeof(T);
				size_ -= sizeof(T);
				return value;
			}

			// TODO : Technically UB
			template <typename T>
			[[nodiscard]]
			const T & offset(const size_t offset) const __restrict {
				check_size(offset + sizeof(T), "Out Of Range ELF Binary Read");

				return *(const T * __restrict)(data_ + offset);
			}

			[[nodiscard]]
			const char * address_offset(const size_t offset) const __restrict {
				check_size(offset, "Out Of Range ELF Binary Read");

				return data_ + offset;
			}
		};

		template <typename PtrType>
		void read_binary(data_stream & __restrict stream_data);
	public:
		template <typename T>
		struct extent final {
			T offset;
			T size;

			T end_offset() const {
				return offset + size;
			}
		};

		struct section final {
			std::string	name;
			extent<uint32> file_extent;
			extent<uint32> memory_extent;
			bool				executable;

			bool operator < (const section & __restrict section) const {
				return memory_extent.offset < section.memory_extent.offset;
			}
		};

		const data_stream																	raw_data_stream_;
		std::vector<section>															sections_;
		std::unordered_map<std::string, const section *>	named_sections_;

		uint32															entry_address_;
		uint32															program_headers_;
		uint32															program_headers_size_;
		uint32															program_header_count_;

		binary(std::span<char> binary_data);
	};
}
