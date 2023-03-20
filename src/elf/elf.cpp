#include "pch.hpp"
#include "elf.hpp"

#include <algorithm>
#include <cassert>

/* For details about the ELF format: */
/* http://www.sco.com/developers/gabi/latest/ch5.pheader.html */

#if _MSC_VER
#	define _packed_struct __pragma(pack(1)) struct
#else
#	define _packed_struct struct __attribute__((packed))
#endif

#define _unused(var) (void)(var)

namespace elf {
	namespace {
		enum class elf_class : uint8_t {
			ELF32 = 1,
			ELF64 = 2,
		};

		enum class elf_endian : uint8_t {
			Little = 1,
			Big = 2,
		};

		enum class elf_type : uint16_t {
			Relocatable = 1,
			Executable = 2,
			Shared = 3,
			Core = 4,
		};

		enum class program_header_type : uint32_t {
			Null = 0,										// ignore
			Load = 1,										// this should be loaded
			DynamicLink = 2,						// DynamicLink linking information
			Interpret = 3,							// for interpreters
			Note = 4,										// auxiliary information
			SharedLib = 5,							// unspecified - ignore
			ProgramHeader = 6,					// Program Header - should load into memory image.
			TLS = 7,										// Thread-Local Storage
			LOOS = 0x60000000,					// ignore
			HIOS = 0x6fffffff,					// ignore
			LOPROC = 0x70000000,				// ignore, though check MIPS doc to see if information is available
			HIPROC = 0x7fffffff,				// ignore, though check MIPS doc to see if information is available
			GNU_EH_FRAME = 0x6474e550,
		};

		enum class program_header_access_bits : uint32_t {
			Execute = 1,
			Write = 2,
			Read = 4,
			MaskOS = 0x0FF00000,
			MaskProc = 0xF0000000,
		};

		enum class section_type : uint32_t {
			Null = 0,											// ignore
			ProgramBits = 1,							// Program-defined contents
			SymbolTable = 2,							// Symbol Table
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

		enum class section_flags : uint32_t {
			Write = 1,
			Alloc = 2,
			ExecInstr = 4,
			Rela_LivePatch = 0x00100000,
			MaskProc = 0xF0000000,
		};
	}

	static constinit const uint32_t magic_number = 0X464C457F;

	binary::binary(const std::span<char> binary_data) : raw_data_stream_(binary_data) {
		data_stream stream_data = { binary_data };

		// validate magic number.
		if _unlikely(stream_data.pop<uint32_t>() != magic_number) [[unlikely]] {
			throw std::runtime_error("ELF Binary magic number mismatch");
		}

		// now we dispatch to the typed ELF reader. We will 'support' 64-bit binaries, but we will truncate addresses.
		// It's theoretically possible to make an ELF64 with 32-bit machine code.
		switch (stream_data.pop<elf_class>()) {
			case elf_class::ELF32:
				[[likely]]
				read_binary<uint32_t>(stream_data);
				break;
			case elf_class::ELF64:
				read_binary<uint64_t>(stream_data);
				break;
			default:
				[[unlikely]]
				throw std::runtime_error("Unknown ELF Class");
		}
	}

	template <typename PtrType> struct program_header;

	template <> _packed_struct program_header<uint32_t> {
		using elfptr_t = uint32;

		program_header_type Type;
		elfptr_t Offset;
		elfptr_t VirtualAddr;
		elfptr_t PhysicalAddr;
		elfptr_t FileSize;
		elfptr_t MemorySize;
		program_header_access_bits Flags;
		elfptr_t MemoryAlign;

		void validate() const {
			if _unlikely(MemoryAlign > 1 && ((VirtualAddr % MemoryAlign) != (PhysicalAddr % MemoryAlign))) [[unlikely]] {
				throw std::runtime_error("VirtualAddr and PhysicalAddr alignments mismatch");
			}
		}
	};

	template <> _packed_struct program_header<uint64_t> {
		using elfptr_t = uint64;

		program_header_type Type;
		program_header_access_bits Flags;
		elfptr_t Offset;
		elfptr_t VirtualAddr;
		elfptr_t PhysicalAddr;
		elfptr_t FileSize;
		elfptr_t MemorySize;
		elfptr_t MemoryAlign;

		void validate() const {
			if _unlikely(MemoryAlign > 1 && ((VirtualAddr % MemoryAlign) != (PhysicalAddr % MemoryAlign))) [[unlikely]] {
				throw std::runtime_error("VirtualAddr and PhysicalAddr alignments mismatch");
			}
		}
	};

	template <typename elfptr_t> _packed_struct SectionHeader {
		uint32_t NameOffset;
		section_type Type;
		section_flags Flags;
		elfptr_t Address;
		elfptr_t Offset;
		elfptr_t Size;
		uint32_t Link;
		uint32_t Info;
		elfptr_t AddrAlign;
		elfptr_t EntSize;
	};

	template <typename PtrType>
	void binary::read_binary(data_stream & __restrict stream_data) {
		using elfptr_t = PtrType;

		if _unlikely(stream_data.pop<elf_endian>() != elf_endian::Little) [[unlikely]] {
			throw std::runtime_error("Only Little Endian ELF binaries supported");
		}

		const uint8_t ElfIdenVersion = stream_data.pop<uint8_t>();
		_unused(ElfIdenVersion);
		const uint8_t ABI = stream_data.pop<uint8_t>();
		_unused(ABI);
		const uint8_t ABIVersion = stream_data.pop<uint8_t>();
		_unused(ABIVersion);
		stream_data.skip(7);

		const elf_type ObjectType = stream_data.pop<elf_type>();
		_unused(ObjectType);

		// We only support a single ISA, so a full enum is unnecessary.
		static const uint16_t MIPS_ISA = 0x08;
		if _unlikely(stream_data.pop<uint16_t>() != MIPS_ISA) [[unlikely]] {
			throw std::runtime_error("Only MIPS binaries are supported");
		}

		const uint32_t ElfVersion = stream_data.pop<uint32_t>();
		_unused(ElfVersion);

		const elfptr_t entryOffset = stream_data.pop<elfptr_t>();
		if _unlikely(entryOffset > 0xFFFFFFFF) [[unlikely]] {
			throw std::runtime_error("ELF Entry Address out of range");
		}
		entry_address_ = uint32_t(entryOffset);

		const elfptr_t programHeaderOffset = stream_data.pop<elfptr_t>();
		const elfptr_t sectionHeaderOffset = stream_data.pop<elfptr_t>();
		_unused(sectionHeaderOffset);

		const uint32_t Flags = stream_data.pop<uint32_t>();
		_unused(Flags);
		const uint16_t HeaderSize = stream_data.pop<uint16_t>();
		_unused(HeaderSize);
		const uint16_t ProgramHeaderSize = stream_data.pop<uint16_t>();
		const uint16_t ProgramHeaderEntries = stream_data.pop<uint16_t>();
		const uint16_t SectionHeaderSize = stream_data.pop<uint16_t>();
		_unused(SectionHeaderSize);
		const uint16_t SectionHeaderEntries = stream_data.pop<uint16_t>();
		const uint16_t SectionHeaderNamesIndex = stream_data.pop<uint16_t>();
		if (SectionHeaderNamesIndex >= SectionHeaderEntries) {
			throw std::runtime_error("Section Header Names Index out of range");
		}

		// Validate that the entry point is in-range. The program may crash if it's near the end, but that's Not Our Problem (tm)
		if _unlikely(entryOffset >= 0x100000000ull) [[unlikely]] {
			throw std::runtime_error("ELF offsets out of range (32-bit only)");
		}

		sections_.reserve(size_t(ProgramHeaderEntries + SectionHeaderEntries));

		xassert(programHeaderOffset <= std::numeric_limits<decltype(program_headers_)>::max());
		program_headers_ = uint32(programHeaderOffset);
		program_headers_size_ = ProgramHeaderSize;
		program_header_count_ = ProgramHeaderEntries;

		// Now we need to parse the program header table. 32-bit and 64-bit for some reason have different types.
		{
			using Header = program_header<elfptr_t>;
			size_t curEntryOffset = programHeaderOffset;
			for (size_t entry = 0; entry < ProgramHeaderEntries; ++entry) {
				const Header &programHeader = raw_data_stream_.offset<Header>(curEntryOffset);
				programHeader.validate();

				curEntryOffset += ProgramHeaderSize;

				switch (programHeader.Type) {
					case program_header_type::Null:
					case program_header_type::Interpret:
					case program_header_type::Note:
					case program_header_type::SharedLib:
					//case program_header_type::program_header:
						continue; // ignore
					default: break;
				}

				if (programHeader.Type == program_header_type::ProgramHeader) {
					program_headers_ = uint32_t(programHeader.VirtualAddr);
				}

				if (programHeader.MemorySize == 0) {
					continue;
				}

				// Make sure that the size is valid.
				// ReSharper disable once CppNoDiscardExpression
				raw_data_stream_.offset<char>(programHeader.Offset + (programHeader.FileSize - 1));
				if _unlikely((programHeader.VirtualAddr + programHeader.MemorySize) >= 0x100000000ull) [[unlikely]] {
					throw std::runtime_error("ELF Program Header out of virtual address range");
				}

				sections_.push_back(
					{
						.name = "",
						.file_extent = { (uint32)programHeader.Offset, (uint32)std::min(programHeader.FileSize, programHeader.MemorySize) },
						.memory_extent = { uint32_t(programHeader.VirtualAddr), uint32_t(programHeader.MemorySize) },
						.executable = (uint32(programHeader.Flags) & uint32(program_header_access_bits::Execute)) != 0
					}
				);
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
		//			case section_type::Null:
		//			case section_type::Note:
		//			case section_type::SharedLib:
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
		//		if (uint32_t(sectionHeader.Flags) > (uint32_t(section_flags::Write) | uint32_t(section_flags::Alloc) | uint32_t(section_flags::ExecInstr)))
		//		{
		//			continue;
		//		}
		//
		//		if (sectionHeader.Type != section_type::NoBits)
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
		//			uint32((sectionHeader.Type == section_type::NoBits) ? 0 : sectionHeader.Size),
		//			uint32_t(sectionHeader.Address),
		//			uint32_t(sectionHeader.Size),
		//			(uint32_t(sectionHeader.Flags) & uint32(section_flags::ExecInstr)) != 0
		//		};
		//		m_Sections.emplace_back(section);
		//
		//		if (sectionName.length())
		//		{
		//			m_NamedSections[sectionName] = &m_Sections.back();
		//		}
		//	}
		//}

		std::stable_sort(sections_.begin(), sections_.end());
	}
}