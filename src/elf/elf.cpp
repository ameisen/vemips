#include "pch.hpp"
#include "elf.hpp"

#include <algorithm>
#include <cassert>

/* For details about the ELF format: */
/* http://www.sco.com/developers/gabi/latest/ch5.pheader.html */

#if _MSC_VER
#  define __prepacked __pragma(pack(push, 1))
#  define __postpacked
#else
#  define __prepacked
#  define __postpacked __attribute__((packed))
#endif

namespace
{
	enum class ElfClass : uint8_t
	{
		ELF32 = 1,
		ELF64 = 2,
	};

	enum class ElfEndian : uint8_t
	{
		Little = 1,
		Big = 2,
	};

	enum class ElfType : uint16_t
	{
		Relocatable = 1,
		Executable = 2,
		Shared = 3,
		Core = 4,
	};

	enum class ProgramHeaderType : uint32_t
	{
		Null = 0, // ignore
		Load = 1, // this should be loaded
		DynamicLink = 2, // DynamicLink linking information
		Interpret = 3, // for interpreters
		Note = 4, // auxiliary information
		SharedLib = 5, // unspecified - ignore
		ProgramHeader = 6, // Program Header - should load into memory image.
		TLS = 7, // Thread-Local Storage
		LOOS = 0x60000000, // ignore
		HIOS = 0x6fffffff, // ignore
		LOPROC = 0x70000000, // ignore, though check MIPS doc to see if information is available
		HIPROC = 0x7fffffff, // ignore, though check MIPS doc to see if information is available
		GNU_EH_FRAME = 0x6474e550,
	};

	enum class ProgramHeaderAccessBits : uint32_t
	{
		Execute = 1,
		Write = 2,
		Read = 4,
		MaskOS = 0x0FF00000,
		MaskProc = 0xF0000000,
	};

	enum class SectionType : uint32_t
	{
		Null = 0, // ignore
		ProgramBits = 1, // Program-defined contents
		SymbolTable = 2, // Symbol Table
		StringTable = 3,
		RelocationExplicit = 4,
		SymbolHash = 5,
		DynamicLink = 6,
		Note = 7,
		NoBits = 8,
		RelocationNonExplicit = 9,
		SharedLib = 10,
		DynamicSymbolTable = 11,
		Num = 12,
		Initialization = 14,
		Termination = 15,
		PreInitialization = 16,
		Group = 17,
		SymbolTableIndex = 18,
		LoProc = 0x70000000,
		HiProc = 0x7FFFFFFF,
		LoUser = 0x80000000,
		Hiuser = 0xFFFFFFFF,
		MIPS_RegisterUsage = 0x70000006,
		MIPS_Options = 0x7000000D,
		MIPS_ABIFlags = 0x7000002A,
	};

	enum class SectionFlags : uint32_t
	{
		Write = 1,
		Alloc = 2,
		ExecInstr = 4,
		Rela_LivePatch = 0x00100000,
		MaskProc = 0xF0000000,
	};
}

namespace elf
{

	binary::binary(const std::vector<char> &binary_data) : m_RawDataStream(binary_data)
	{
		data_stream stream_data = { binary_data };

		// validate magic number.
		static const uint32_t RefMagicNumber = 0X464C457F;
		if (stream_data.pop<uint32_t>() != RefMagicNumber)
		{
			throw std::runtime_error("ELF Binary magic number mismatch");
		}

		const ElfClass ElfPtrClass = stream_data.pop<ElfClass>();

		// now we dispatch to the typed ELF reader. We will 'support' 64-bit binaries, but we will truncate addresses.
		// It's theoretically possible to make an ELF64 with 32-bit machine code.
		switch (ElfPtrClass)
		{
			case ElfClass::ELF32:
				read_binary<4>(stream_data);
				break;
			case ElfClass::ELF64:
				read_binary<8>(stream_data);
				break;
			default:
				throw std::runtime_error("Unknown ELF Class");
		}
	}

	template <size_t PtrSize> struct PtrType;

	template <> struct PtrType<4> { using ptr_t = uint32_t; };
	template <> struct PtrType<8> { using ptr_t = uint64_t; };

	template <typename PtrType> struct ProgramHeader;

	__prepacked template <> struct __postpacked ProgramHeader<uint32_t>
	{
		using elfptr_t = uint32;

		ProgramHeaderType Type;
		elfptr_t Offset;
		elfptr_t VirtualAddr;
		elfptr_t PhysicalAddr;
		elfptr_t FileSize;
		elfptr_t MemorySize;
		ProgramHeaderAccessBits Flags;
		elfptr_t MemoryAlign;

		void validate() const
		{
			if (MemoryAlign > 1 && ((VirtualAddr % MemoryAlign) != (PhysicalAddr % MemoryAlign)))
			{
				throw std::runtime_error("VirtualAddr and PhysicalAddr alignments mismatch");
			}
		}
	};

	__prepacked template <> struct __postpacked ProgramHeader<uint64_t>
	{
		using elfptr_t = uint64;

		ProgramHeaderType Type;
		ProgramHeaderAccessBits Flags;
		elfptr_t Offset;
		elfptr_t VirtualAddr;
		elfptr_t PhysicalAddr;
		elfptr_t FileSize;
		elfptr_t MemorySize;
		elfptr_t MemoryAlign;

		void validate() const
		{
			if (MemoryAlign > 1 && ((VirtualAddr % MemoryAlign) != (PhysicalAddr % MemoryAlign)))
			{
				throw std::runtime_error("VirtualAddr and PhysicalAddr alignments mismatch");
			}
		}
	};

	template <typename PtrType> struct SectionHeader;

	__prepacked template <> struct __postpacked SectionHeader<uint32_t>
	{
		using elfptr_t = uint32;

		uint32_t NameOffset;
		SectionType Type;
		SectionFlags Flags;
		elfptr_t Address;
		elfptr_t Offset;
		elfptr_t Size;
		uint32_t Link;
		uint32_t Info;
		elfptr_t AddrAlign;
		elfptr_t EntSize;
	};

	__prepacked template <> struct __postpacked SectionHeader<uint64_t>
	{
		using elfptr_t = uint64;

		uint32_t NameOffset;
		SectionType Type;
		SectionFlags Flags;
		elfptr_t Address;
		elfptr_t Offset;
		elfptr_t Size;
		uint32_t Link;
		uint32_t Info;
		elfptr_t AddrAlign;
		elfptr_t EntSize;
	};

	template <size_t PtrSize>
	void binary::read_binary(data_stream &stream_data)
	{
		using elfptr_t = typename PtrType<PtrSize>::ptr_t;

		if (stream_data.pop<ElfEndian>() != ElfEndian::Little)
		{
			throw std::runtime_error("Only Little Endian ELF binaries supported");
		}

		const uint8_t ElfIdenVersion = stream_data.pop<uint8_t>();
		const uint8_t ABI = stream_data.pop<uint8_t>();
		const uint8_t ABIVersion = stream_data.pop<uint8_t>();
		stream_data.skip(7);

		const ElfType ObjectType = stream_data.pop<ElfType>();

		// We only support a single ISA, so a full enum is unnecessary.
		static const uint16_t MIPS_ISA = 0x08;
		if (stream_data.pop<uint16_t>() != MIPS_ISA)
		{
			throw std::runtime_error("Only MIPS binaries are supported");
		}

		const uint32_t ELfVersion = stream_data.pop<uint32_t>();

		const elfptr_t entryOffset = stream_data.pop<elfptr_t>();
		if (entryOffset > 0xFFFFFFFF)
		{
			throw std::runtime_error("ELF Entry Address out of range");
		}
		m_EntryAddress = uint32_t(entryOffset);

		const elfptr_t programHeaderOffset = stream_data.pop<elfptr_t>();
		const elfptr_t sectionHeaderOffset = stream_data.pop<elfptr_t>();

		const uint32_t Flags = stream_data.pop<uint32_t>();
		const uint16_t HeaderSize = stream_data.pop<uint16_t>();
		const uint16_t ProgramHeaderSize = stream_data.pop<uint16_t>();
		const uint16_t ProgramHeaderEntries = stream_data.pop<uint16_t>();
		const uint16_t SectionHeaderSize = stream_data.pop<uint16_t>();
		const uint16_t SectionHeaderEntries = stream_data.pop<uint16_t>();
		const uint16_t SectionHeaderNamesIndex = stream_data.pop<uint16_t>();
		if (SectionHeaderNamesIndex >= SectionHeaderEntries)
		{
			throw std::runtime_error("Section Header Names Index out of range");
		}

		// Validate that the entry point is in-range. The program may crash if it's near the end, but that's Not Our Problem (tm)
		if (entryOffset >= 0x100000000ull)
		{
			throw std::runtime_error("ELF offsets out of range (32-bit only)");
		}

		m_Sections.reserve(size_t(ProgramHeaderEntries + SectionHeaderEntries));

		assert(programHeaderOffset <= std::numeric_limits<decltype(m_ProgramHeaders)>::max());
		m_ProgramHeaders = uint32(programHeaderOffset);
		m_ProgramHeaderSize = ProgramHeaderSize;
		m_ProgramHeaderCnt = ProgramHeaderEntries;

		// Now we need to parse the program header table. 32-bit and 64-bit for some reason have different types.
		{
			using Header = ProgramHeader<elfptr_t>;
			size_t curEntryOffset = programHeaderOffset;
			for (size_t entry = 0; entry < ProgramHeaderEntries; ++entry)
			{
				const Header &programHeader = m_RawDataStream.offset<Header>(curEntryOffset);
				programHeader.validate();

				curEntryOffset += ProgramHeaderSize;

				switch (programHeader.Type)
				{
					case ProgramHeaderType::Null:
					case ProgramHeaderType::Interpret:
					case ProgramHeaderType::Note:
					case ProgramHeaderType::SharedLib:
					//case ProgramHeaderType::ProgramHeader:
						continue; // ignore
					default: break;
				}

				if (programHeader.Type == ProgramHeaderType::ProgramHeader)
				{
					m_ProgramHeaders = uint32_t(programHeader.VirtualAddr);
				}

				if (programHeader.MemorySize == 0)
				{
					continue;
				}

				// Make sure that the size is valid.
				m_RawDataStream.offset<char>(programHeader.Offset + (programHeader.FileSize - 1));
				if ((programHeader.VirtualAddr + programHeader.MemorySize) >= 0x100000000ull)
				{
					throw std::runtime_error("ELF Program Header out of virtual address range");
				}

				Section section = {
					"",
					(uint32)programHeader.Offset,
					(uint32)std::min(programHeader.FileSize, programHeader.MemorySize),
					uint32_t(programHeader.VirtualAddr),
					uint32_t(programHeader.MemorySize),
					(uint32(programHeader.Flags) & uint32(ProgramHeaderAccessBits::Execute)) != 0
				};
				m_Sections.emplace_back(section);
			}
		}

		// And now we need to parse the section table.
		//const size_t StringSectionOffset = (SectionHeaderSize * SectionHeaderNamesIndex) + sectionHeaderOffset;
		//const SectionHeader<elfptr_t> &stringSectionHeader = m_RawDataStream.offset<SectionHeader<elfptr_t>>(StringSectionOffset);
		//{
		//	using Header = SectionHeader<elfptr_t>;
		//	size_t curEntryOffset = sectionHeaderOffset;
		//	for (size_t entry = 0; entry < SectionHeaderEntries; ++entry)
		//	{
		//		const Header &sectionHeader = m_RawDataStream.offset<Header>(curEntryOffset);
		//
		//		curEntryOffset += SectionHeaderSize;
		//
		//		// Read Section Name.
		//		std::string sectionName;
		//		sectionName.reserve(16);
		//		size_t curNameOffset = stringSectionHeader.Offset + sectionHeader.NameOffset;
		//		char curChar;
		//		while ((curChar = m_RawDataStream.offset<char>(curNameOffset++)) != '\0')
		//		{
		//			sectionName += curChar;
		//		}
		//
		//		switch (sectionHeader.Type)
		//		{
		//			case SectionType::Null:
		//			case SectionType::Note:
		//			case SectionType::SharedLib:
		//				continue; // ignore
		//			default: break;
		//		}
		//
		//		if (sectionHeader.Size == 0)
		//		{
		//			continue;
		//		}
		//
		//		if (uint32_t(sectionHeader.Flags) == 0)
		//		{
		//			continue;
		//		}
		//		if (uint32_t(sectionHeader.Flags) > (uint32_t(SectionFlags::Write) | uint32_t(SectionFlags::Alloc) | uint32_t(SectionFlags::ExecInstr)))
		//		{
		//			continue;
		//		}
		//
		//		if (sectionHeader.Type != SectionType::NoBits)
		//		{
		//			// Make sure that the size is valid.
		//			m_RawDataStream.offset<char>(sectionHeader.Offset + (sectionHeader.Size - 1));
		//		}
		//		if ((sectionHeader.Address + sectionHeader.Size) >= 0x100000000ull)
		//		{
		//			throw std::runtime_error("ELF Section Header out of virtual address range");
		//		}
		//
		//		if (sectionName == ".reginfo" || sectionName == ".MIPS.abiflags")
		//		{
		//			continue;
		//		}
		//
		//		Section section = {
		//			sectionName,
		//			(uint32)sectionHeader.Offset,
		//			uint32((sectionHeader.Type == SectionType::NoBits) ? 0 : sectionHeader.Size),
		//			uint32_t(sectionHeader.Address),
		//			uint32_t(sectionHeader.Size),
		//			(uint32_t(sectionHeader.Flags) & uint32(SectionFlags::ExecInstr)) != 0
		//		};
		//		m_Sections.emplace_back(section);
		//
		//		if (sectionName.length())
		//		{
		//			m_NamedSections[sectionName] = &m_Sections.back();
		//		}
		//	}
		//}

		std::sort(m_Sections.begin(), m_Sections.end());
	}
}