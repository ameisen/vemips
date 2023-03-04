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

_forceinline jit2::MapLevel1::~MapLevel1() {
	for (auto *ptr : data_) {
		delete ptr;
	}
}

_forceinline jit2::Chunk & jit2::MapLevel1::operator [] (uint32 idx) {
	Chunk * __restrict result = data_[idx];
	if (!result) {
		result = new Chunk;
		data_[idx] = result;
	}
	return *result;
}

jit2::jit2(processor & __restrict _processor) : m_processor(_processor)
{
}

jit2::~jit2() = default;

void jit2::populate_chunk(Chunk & __restrict chunk, uint32 start_address, bool update) __restrict
{
	if (!chunk.data)
	{
		chunk.populate_data();
	}

	xassert((start_address % 4) == 0);
	const uint32 base_address = start_address & ~(jit2::ChunkSize - 1);
	const uint32 last_address = base_address + (jit2::ChunkSize - 1);

	const uint32 current_address = start_address;

	const uint32 start_index = 0;
	if (!update)
	{
		chunk.offset = base_address;
	}

	auto &&chunk_data = *chunk.data;

	static constexpr uint32 num_instructions = jit2::NumInstructionsChunk;
	for (uint32 i = start_index; i < num_instructions; ++i) {
		const uint32 offset_address = base_address + (i * 4);

		const instruction_t* __restrict ptr = m_processor.safe_mem_fetch_exec<const instruction_t>(offset_address);

		if (!ptr) {
			// AdEL
			chunk_data[i] = { {}, {}, offset_address, CPU_Exception::Type::AdEL };
			continue;
		}

		const instruction_t instruction = *ptr;
		const auto instruction_info_ptr = mips::FindExecuteInstruction(instruction);
		if (!instruction_info_ptr) {
			// RI
			chunk_data[i] = { {}, instruction, offset_address, CPU_Exception::Type::RI };
			continue;
		}

		chunk_data[i] = { instruction_info_ptr, instruction, offset_address };
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
			m_processor.set_flags(processor::flag::jit_mem_flush);
			m_FlushChunk = dirty_chunk;
			return true;
		}
		else
		{
			populate_chunk(*dirty_chunk, adjusted_address, true);
			if (adjusted_address + 4 == dirty_chunk->offset + ChunkSize)
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

	auto&& processor = m_processor;
	const auto guest = processor.get_guest_system();

	auto chunk_offset = get_chunk_offset(address);
	if (chunk_offset.chunk == nullptr) {
		get_instruction(address);
		chunk_offset = get_chunk_offset(address);
	}
	auto&& chunk_data = *chunk_offset.chunk->data;
	for (size_t i = chunk_offset.offset; i < NumInstructionsChunk; ++i) {

		auto&& chunk_instruction = chunk_data[i];
		auto program_counter = processor.get_program_counter();
		if _unlikely(chunk_instruction.instruction_info == nullptr) {
			throw CPU_Exception{ chunk_instruction.exception, program_counter };
		}
		else {
			if (!processor.execute_explicit(chunk_instruction.instruction_info, chunk_instruction.instruction)) {
				break;
			}
			if (chunk_instruction.instruction_info->Flags.control) {
				break;
			}
			if (program_counter + 4 != processor.get_program_counter()) {
				break;
			}
		}
	}

	if (guest->is_execution_complete())
	{
		if (guest->is_execution_success())
		{
			throw ExecutionCompleteException();
		}
		else
		{
			throw ExecutionFailException();
		}
	}
	if (processor.get_flags(processor::flag::jit_mem_flush))
	{
		populate_chunk(*m_FlushChunk, m_FlushAddress, true);
		if (m_FlushAddress + 4 == m_FlushChunk->offset + ChunkSize)
		{
			memory_touched(m_FlushAddress + 4);
		}
	}
	if (processor.get_flags(processor::flag::trapped_exception))
	{
		// there was an exception.
		processor.clear_flags(processor::flag::trapped_exception);
		throw processor.trapped_exception_;
	}
}

_forceinline jit2::instruction_wrapper jit2::get_instruction(uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	Chunk * __restrict chunk = nullptr;

	if (m_LastChunkAddress == mapped_address)
	{
		chunk = m_LastChunk;
	}
	else
	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */
		auto & __restrict cml2 = m_JitMap[address >> (32 - RemainingLog2)];
		auto & __restrict cml1 = cml2[(address >> (32 - RemainingLog2 - 8)) & 0xFF];

		const bool chunk_exists = cml1.contains((address >> (32 - RemainingLog2 - 16)) & 0xFF);
		chunk = &cml1[(address >> (32 - RemainingLog2 - 16)) & 0xFF];

		if (!chunk_exists) {
			populate_chunk(*chunk, mapped_address, false);
		}

		m_LastChunk = chunk;
		m_LastChunkAddress = mapped_address;
	}

	const auto & __restrict chunk_instruction = (*chunk->data)[address_offset];
	
	return chunk_instruction;
}

_forceinline jit2::jit2::instruction_wrapper jit2::fetch_instruction(uint32 address)
{
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	Chunk* __restrict chunk = nullptr;

	if (m_LastChunkAddress == mapped_address)
	{
		chunk = m_LastChunk;
	}
	else
	{
		// Traverse the map to end up at the proper chunk.
		/* This is only hit once ever in the current benchmark because the chunks are huge */
		auto& __restrict cml2 = m_JitMap[address >> (32 - RemainingLog2)];
		auto& __restrict cml1 = cml2[(address >> (32 - RemainingLog2 - 8)) & 0xFF];

		if (!cml1.contains((address >> (32 - RemainingLog2 - 16)) & 0xFF)) {
			return {};
		}

		chunk = &cml1[(address >> (32 - RemainingLog2 - 16)) & 0xFF];

		m_LastChunk = chunk;
		m_LastChunkAddress = mapped_address;
	}

	const auto& __restrict chunk_instruction = (*chunk->data)[address_offset];

	return chunk_instruction;
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

jit2::chunk_data jit2::get_chunk_offset(uint32 address) {
	const uint32 mapped_address = address & ~(ChunkSize - 1);
	const uint32 address_offset = (address - mapped_address) / 4u;

	return { get_chunk(address), address_offset };
}
