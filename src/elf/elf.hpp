#pragma once

#include "common.hpp"

#include <vector>
#include <exception>
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace elf {
	class binary final {
		class data_stream final {
			const char * __restrict m_Data;
			size_t m_Size;

			template <size_t N>
			_forceinline void check_size(size_t size, const char (& __restrict message)[N]) const __restrict {
				if _unlikely(m_Size < size) [[unlikely]] {
					throw std::runtime_error(message);
				}
			}

		public:
			data_stream(const std::vector<char> & __restrict data) : m_Data(data.data()), m_Size(data.size()) {}

			template <typename T>
			T pop() __restrict {
				check_size(sizeof(T), "Out Of Range ELF Binary Seek");

				const auto* __restrict dataPtr = m_Data;
				m_Data += sizeof(T);
				m_Size -= sizeof(T);

				T result;
				memcpy(&result, dataPtr, sizeof(T));
				return result;
			}

			void skip(size_t size) __restrict {
				check_size(size, "Out Of Range ELF Binary Seek");

				m_Data += size;
				m_Size -= size;
			}

			template <typename T>
			void extract(T &out) __restrict {
				check_size(sizeof(T), "Out Of Range ELF Binary Seek");

				memcpy(&out, m_Data, sizeof(T));
				m_Data += sizeof(T);
				m_Size -= sizeof(T);
			}

			// TODO : Technically UB
			template <typename T>
			const T & reference() __restrict {
				check_size(sizeof(T), "Out Of Range ELF Binary Seek");

				const T & __restrict value = *(const T * __restrict)m_Data;
				m_Data += sizeof(T);
				m_Size -= sizeof(T);
				return value;
			}

			// TODO : Technically UB
			template <typename T>
			const T & offset(size_t offset) const __restrict {
				check_size(offset + sizeof(T), "Out Of Range ELF Binary Read");

				return *(const T * __restrict)(m_Data + offset);
			}

			const char * address_offset(size_t offset) const __restrict {
				check_size(offset, "Out Of Range ELF Binary Read");

				return m_Data + offset;
			}
		};

		template <typename PtrType>
		void read_binary(data_stream & __restrict stream_data);
	public:
		struct Section {
			std::string	Name;
			uint32			FileOffset;
			uint32			FileSize;
			uint32			MemoryOffset;
			uint32			MemorySize;
			bool				Executable;

			bool operator < (const Section & __restrict section) const {
				return MemoryOffset < section.MemoryOffset;
			}
		};

		const data_stream																	m_RawDataStream;
		std::vector<Section>															m_Sections;
		std::unordered_map<std::string, const Section *>	m_NamedSections;

		uint32															m_EntryAddress;
		uint32															m_ProgramHeaders;
		uint32															m_ProgramHeaderSize;
		uint32															m_ProgramHeaderCnt;

		binary(const std::vector<char> & __restrict binary_data);
	};
}
