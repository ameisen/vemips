#include "pch.hpp"

#include "jit2.hpp"
#include "instructions/instructions.hpp"
//#include "instructions/instructions_common.hpp"
//#include "../../processor.hpp"
#include <cassert>
//#include "codegen.hpp"

//#define NOMINMAX 1
//#include <Windows.h>

//#include <mutex>

#include "system.hpp"

using namespace mips;

#define JIT_INSTRUCTION_SEPARATE 0
#define JIT_INSERT_IDENTIFIERS 0

#define IS_INSTRUCTION(instr, ref) \
	[&]() -> bool { \
		extern const mips::instructions::InstructionInfo StaticProc_ ## ref; \
		return &StaticProc_ ## ref == instr; \
	}()

namespace {
	__forceinline __declspec(restrict, nothrow) void * allocate_executable(size_t size) {
		 return VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	}

	__forceinline __declspec(nothrow) void free_executable(void *ptr) {
		VirtualFree(ptr, 0, MEM_RELEASE);
	}
}

__forceinline jit2::MapLevel1::~MapLevel1() {
	for (auto *ptr : m_Data) {
		if (ptr) {
			free_executable(ptr);
		}
	}
}

__forceinline jit2::Chunk & __restrict jit2::MapLevel1::operator [] (uint32 idx) {
	Chunk * __restrict result = m_Data[idx];
	if (!result) {
		result = (Chunk * __restrict)allocate_executable(sizeof(Chunk));
		new (result) Chunk;
		m_Data[idx] = result;

		m_DataOffsets[idx] = new ChunkOffset;
		memset(m_DataOffsets[idx]->data(), 0, m_DataOffsets[idx]->size() * sizeof(uint32));
	}
	return *result;
}

__forceinline jit2::ChunkOffset & __restrict jit2::MapLevel1::get_offsets(uint32 idx) {
	return *m_DataOffsets[idx];
}

jit2::jit_instructionexec_t jit1_get_instruction(jit2 * __restrict _this, uint32 address) {
	return _this->get_instruction(address);
}

jit2::jit_instructionexec_t jit1_fetch_instruction(jit2* __restrict _this, uint32 address) {
	return _this->fetch_instruction(address);
}

namespace {
	void RI_Exception (instruction_t, processor & __restrict _processor) {
		_processor.set_trapped_exception({ CPU_Exception::Type::RI, _processor.get_program_counter() });
	}

	void OV_Exception (uint32 code, processor & __restrict _processor) {
		_processor.set_trapped_exception({ CPU_Exception::Type::Ov, _processor.get_program_counter(), code });
	}

	void TR_Exception (uint32 code, processor & __restrict _processor)
	{
		_processor.set_trapped_exception({ CPU_Exception::Type::Tr, _processor.get_program_counter(), code });
	}

	void AdEL_Exception (uint32 address, processor & __restrict _processor)
	{
		_processor.set_trapped_exception({ CPU_Exception::Type::AdEL, _processor.get_program_counter(), address });
	}

	void AdES_Exception(uint32 address, processor & __restrict _processor)
	{
		_processor.set_trapped_exception({ CPU_Exception::Type::AdES, _processor.get_program_counter(), address });
	}
}

jit2::jit2(processor & __restrict _processor) : m_processor(_processor)
{
}

jit2::~jit2()
{
}


namespace
{
	// // RCX, mov'd from the machine code	// RDX		 // R8
	uint64 JumpFunction(instruction_t i, processor & __restrict p, uint64 f, uint64 rspv)
	{
		const mips::instructions::instructionexec_t executable = mips::instructions::instructionexec_t(f);
		executable(i, p);
		return rspv + 4;
	}

	struct bytes
	{
		const uint8 * __restrict m_data;
		const size_t m_datasize;

		template <size_t N>
		bytes(const uint8(&data) [N]) :
			m_data(data),
			m_datasize(N)
		{
		}
	};

	struct base_fixup
	{
		uint32 offset;
		uint64 chunk_address = 0;

		base_fixup(uint32 _offset) :
			offset(_offset) {}
	};
	template <typename T>
	struct fixup : public base_fixup
	{
		fixup(uint32 _offset) : base_fixup(_offset) {}
	};

	struct vector_wrapper
	{
		std::vector<uint8>& m_data;
		std::vector<base_fixup>& m_fixups;

		template <typename T>
		vector_wrapper& operator << (const T& value);

		template <typename U>
		vector_wrapper& operator << (const fixup<U>& value);
	};


	template <typename T>
	vector_wrapper& vector_wrapper::operator << (const T& value)
	{
		size_t sz = m_data.size();
		m_data.resize(m_data.size() + sizeof(T));
		*(T*)(m_data.data() + sz) = value;
		return *this;
	};

	template<>
	vector_wrapper& vector_wrapper::operator << <bytes> (const bytes& value)
	{
		size_t sz = m_data.size();
		m_data.resize(m_data.size() + value.m_datasize);
		memcpy(m_data.data() + sz, value.m_data, value.m_datasize);
		return *this;
	};

	template <>
	vector_wrapper& vector_wrapper::operator << <uint8> (const uint8& value)
	{
		m_data.push_back(value);
		return *this;
	};

	using instruction_pack = std::pair<uint8[32], size_t>;

	template <>
	vector_wrapper& vector_wrapper::operator << <instruction_pack> (const instruction_pack& value) 
	{
		size_t sz = m_data.size();
		m_data.resize(m_data.size() + value.second);
		memcpy(m_data.data() + sz, value.first, value.second);
		return *this;
	};

	template <typename U>
	vector_wrapper& vector_wrapper::operator << (const fixup<U>& value)
	{
		fixup<U> _value = value;
		_value.chunk_address = m_data.size();
		m_fixups.push_back(_value);
		*this << U(0);
		return *this;
	}

	static uint32 should_debug_break(processor * __restrict proc)
	{
		return proc->get_guest_system()->get_debugger()->should_interrupt_execution();
	}
}

void jit2::populate_chunk(ChunkOffset & __restrict chunk_offset, Chunk & __restrict chunk, uint32 start_address, bool update) __restrict
{
	if (!chunk.m_data)
	{
		chunk.m_datasize = MaxChunkRealSize;
		chunk.m_data = (uint8 *)allocate_executable(MaxChunkRealSize);
	}
}

// TODO may also need to update the _following_ chunk if we are altering the last instruction of this chunk.
bool jit2::memory_touched(uint32 address)
{
	// Presume this is a sorted vector by address. We want to find a chunk that contains this address, if there is one.
	// TODO replace this with log n binary search.

	const uint32 mapped_address = address & ~(ChunkSize - 1);

	Chunk * __restrict dirty_chunk = get_chunk(address);
	if (dirty_chunk)
	{
		const uint32 adjusted_address = address & ~((1 << 2) - 1);
		if (mapped_address == (m_CurrentExecutingChunkAddress& ~(ChunkSize - 1)))
		{
			m_FlushAddress = adjusted_address;
			m_processor.set_flags(processor::flag_bit(processor::flag::jit_mem_flush));
			m_FlushChunk = dirty_chunk;
			return true;
		}
		else
		{
			populate_chunk(*(dirty_chunk->m_chunk_offset), *dirty_chunk, adjusted_address, true);
			if (adjusted_address + 4 == dirty_chunk->m_offset + ChunkSize)
			{
				// At the end of the chunk. Also update next chunk if it exists.
				if (memory_touched(adjusted_address + 4))
				{
					if (adjusted_address + 4 == mapped_address)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
	/*
	for (Chunk *chunk : m_Chunks)
	{
		if (chunk->m_offset == mapped_address)
		{
			const uint32 adjusted_address = address & ~((1 << 2) - 1);
			if (mapped_address == (m_CurrentExecutingChunkAddress& ~(ChunkSize - 1)))
			{
				m_FlushAddress = adjusted_address;
				m_processor.m_jit_meta = 1;
				m_FlushChunk = chunk;
			}
			else
			{
				populate_chunk(*(chunk->m_chunk_offset), *chunk, adjusted_address, true);
				if (adjusted_address + 4 == chunk->m_offset + ChunkSize)
				{
					// At the end of the chunk. Also update next chunk if it exists.
					memory_touched(adjusted_address + 4);
				}
			}
			break;
		}
	}
	*/
}


void jit2::execute_instruction(uint32 address)
{
	m_CurrentExecutingChunkAddress = address;
	// RCX, mov'd from the machine code	// RDX		 // R8
	auto instruction = get_instruction(address);
	__pragma(pack(1)) struct ParameterPack
	{
		uint64 coprocessor1;
		uint32 flags;
		uint32 delay_branch_target;
		uint64 target_instruction;
		uint32 frame_pointer;
	} ppack;
	ppack.coprocessor1 = uint64(m_processor.get_coprocessor(1));
	ppack.flags = m_processor.m_flags;
	ppack.delay_branch_target = m_processor.m_branch_target;
	ppack.target_instruction = m_processor.m_target_instructions;
	ppack.frame_pointer = m_processor.m_registers[30];

	/*
	const uint64 result = jit1_springboard(uint64(instruction), uint64(&m_processor), m_processor.m_instruction_count, uint64(&ppack), 0, 0);
	if (m_processor.get_guest_system()->is_execution_complete())
	{
		if (m_processor.get_guest_system()->is_execution_success())
		{
			throw ExecutionCompleteException();
		}
		else
		{
			throw ExecutionFailException();
		}
	}
	if (m_processor.get_flags(processor::flag_bit(processor::flag::jit_mem_flush)))
	{
		populate_chunk(*m_FlushChunk->m_chunk_offset, *m_FlushChunk, m_FlushAddress, true);
		if (m_FlushAddress + 4 == m_FlushChunk->m_offset + ChunkSize)
		{
			memory_touched(m_FlushAddress + 4);
		}
	}
	if (m_processor.get_flags(processor::flag_bit(processor::flag::trapped_exception)))
	{
		// there was an exception.
		m_processor.clear_flags(processor::flag_bit(processor::flag::trapped_exception));
		throw m_processor.m_trapped_exception;
	}
	*/
}

__forceinline jit2::jit_instructionexec_t jit2::get_instruction(uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	Chunk * __restrict chunk = nullptr;
	ChunkOffset * __restrict chunk_offset = nullptr;

	if (m_LastChunkAddress == mapped_address)
	{
		chunk = m_LastChunk;
		chunk_offset = m_LastChunkOffset;
	}
	else
	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */
		auto & __restrict cml2 = m_JitMap[address >> (32 - RemainingLog2)];
		auto & __restrict cml1 = cml2[(address >> (32 - RemainingLog2 - 8)) & 0xFF];

			const bool chunk_exists = cml1.contains((address >> (32 - RemainingLog2 - 16)) & 0xFF);
			chunk = &cml1[(address >> (32 - RemainingLog2 - 16)) & 0xFF];
			chunk_offset = &cml1.get_offsets((address >> (32 - RemainingLog2 - 16)) & 0xFF);

		if (!chunk_exists) {
			populate_chunk(*chunk_offset, *chunk, mapped_address, false);
		}

		m_LastChunk = chunk;
		m_LastChunkOffset = chunk_offset;
		m_LastChunkAddress = mapped_address;
	}

	const auto & __restrict chunk_instruction = chunk->m_data + (*chunk_offset)[address_offset];
	
	return jit_instructionexec_t(chunk_instruction);
}

__forceinline jit2::jit_instructionexec_t jit2::fetch_instruction(uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	Chunk* __restrict chunk = nullptr;
	ChunkOffset* __restrict chunk_offset = nullptr;

	if (m_LastChunkAddress == mapped_address)
	{
		chunk = m_LastChunk;
		chunk_offset = m_LastChunkOffset;
	}
	else
	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */
		auto& __restrict cml2 = m_JitMap[address >> (32 - RemainingLog2)];
		auto& __restrict cml1 = cml2[(address >> (32 - RemainingLog2 - 8)) & 0xFF];

		if (!cml1.contains((address >> (32 - RemainingLog2 - 16)) & 0xFF)) {
			return jit_instructionexec_t(nullptr);
		}

		chunk = &cml1[(address >> (32 - RemainingLog2 - 16)) & 0xFF];
		chunk_offset = &cml1.get_offsets((address >> (32 - RemainingLog2 - 16)) & 0xFF);

		m_LastChunk = chunk;
		m_LastChunkOffset = chunk_offset;
		m_LastChunkAddress = mapped_address;
	}

	const auto& __restrict chunk_instruction = chunk->m_data + (*chunk_offset)[address_offset];

	return jit_instructionexec_t(chunk_instruction);
}

jit2::Chunk * jit2::get_chunk(uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	if (m_LastChunkAddress == mapped_address)
	{
		return m_LastChunk;
	}
	else
	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */

		const uint32 jitmap_idx = address >> (32 - RemainingLog2);
		if (!m_JitMap.contains(jitmap_idx))
		{
			return nullptr;
		}
		auto & __restrict cml2 = m_JitMap[jitmap_idx];

		const uint32 cml2_idx = (address >> (32 - RemainingLog2 - 8)) & 0xFF;
		if (!cml2.contains(cml2_idx))
		{
			return nullptr;
		}
		auto & __restrict cml1 = cml2[cml2_idx];

		const uint32 cml1_idx = (address >> (32 - RemainingLog2 - 16)) & 0xFF;
		if (!cml1.contains(cml1_idx))
		{
			return nullptr;
		}
		else
		{
			return &cml1[cml1_idx];
		}
	}
}
