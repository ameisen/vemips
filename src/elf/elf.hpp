#pragma once

#include "common.hpp"

#include <vector>
#include <exception>
#include <unordered_map>
#include <string>

namespace elf
{
	class binary
	{
		class data_stream
		{
			const char *m_Data;
			size_t m_Size;
		public:
			data_stream(const std::vector<char> &data) : m_Data(data.data()), m_Size(data.size()) {}

			template <typename T>
			T pop()
			{
				if (m_Size < sizeof(T))
				{
					throw std::runtime_error("Out Of Range ELF Binary Seek");
				}
				const T *value = (const T *)m_Data;
				m_Data += sizeof(T);
				m_Size -= sizeof(T);
				return *value;
			}

			void skip(size_t size)
			{
				if (m_Size < size)
				{
					throw std::runtime_error("Out Of Range ELF Binary Seek");
				}
				m_Data += size;
				m_Size -= size;
			}

			template <typename T>
			void extract(T &out)
			{
				if (m_Size < sizeof(T))
				{
					throw std::runtime_error("Out Of Range ELF Binary Seek");
				}
				out = *(const T *)m_Data;
				m_Data += sizeof(T);
				m_Size -= sizeof(T);
			}

			template <typename T>
			const T & reference()
			{
				if (m_Size < sizeof(T))
				{
					throw std::runtime_error("Out Of Range ELF Binary Seek");
				}
				const T *value = (const T *)m_Data;
				m_Data += sizeof(T);
				m_Size -= sizeof(T);
				return *value;
			}

			template <typename T>
			const T & offset(size_t offset) const
			{
				if (m_Size < (offset + sizeof(T)))
				{
					throw std::runtime_error("Out Of Range ELF Binary Read");
				}
				return *(const T *)&m_Data[offset];
			}

			const char * address_offset(size_t offset) const
			{
				if (m_Size < offset)
				{
					throw std::runtime_error("Out Of Range ELF Binary Read");
				}
				return &m_Data[offset];
			}
		};

		template <size_t PtrSize>
		void read_binary(data_stream &stream_data);
	public:
		struct Section
		{
			std::string	 Name;
			uint32			FileOffset;
			uint32			FileSize;
			uint32			MemoryOffset;
			uint32			MemorySize;
			bool			  Executable;

			bool operator < (const Section &section) const
			{
				return MemoryOffset < section.MemoryOffset;
			}
		};

		const data_stream											 m_RawDataStream;
		std::vector<Section>										 m_Sections;
		std::unordered_map<std::string, const Section *>	m_NamedSections;

		uint32															m_EntryAddress;
		uint32															m_ProgramHeaders;
		uint32															m_ProgramHeaderSize;
		uint32															m_ProgramHeaderCnt;

		binary(const std::vector<char> &binary_data);
	};
}
