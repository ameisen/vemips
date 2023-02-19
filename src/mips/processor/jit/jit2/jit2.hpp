#pragma once

#include <unordered_map>
#include <array>
#include "mips_common.hpp"
#include <cassert>
#include <list>
#include <limits>

#define xassert(expr) do { assert(expr); __assume(expr); } while (false)

namespace mips {
	namespace instructions {
		struct InstructionInfo;
	};

	class processor;
	class jit2 final {
		template <size_t x>
		struct log2 { enum { value = 1 + log2<x/2>::value }; };
  
		template <> struct log2<1> { enum { value = 1 }; };

		static constexpr const uint32 ChunkSize = 0x100; // ChunkSize represents the size for MIPS memory.
		static constexpr const size_t MaxChunkRealSize = (ChunkSize / 0x100) * 8192;
		static constexpr const size_t ChunkSizeLog2 = log2<ChunkSize - 1>::value;
		static constexpr const size_t RemainingLog2 = 32 - ChunkSizeLog2 - 8 - 8;
		static constexpr const size_t NumInstructionsChunk = ChunkSize / 4;
		using ChunkOffset = std::array<uint32, NumInstructionsChunk>;
		struct Chunk final {
			ChunkOffset * __restrict m_chunk_offset;
			const uint8 * __restrict m_data;
			uint32 m_offset;
			uint32 m_datasize;
			// TODO there are better ways to handle this that don't require reconfiguring the entire chunk.
			bool m_has_fixups = false;
			struct patch final {
				uint32 offset;
				uint32 target;
			};
			std::list<patch> m_patches;
		};

		template <typename T>
		struct identity_hash final {
			__forceinline T operator()(T v) const __restrict {
				return v;
			}
		};

		processor & __restrict m_processor;

		class MapLevel1 final {
			std::array<Chunk * __restrict, 256> m_Data = { 0 };
			std::array<ChunkOffset * __restrict, 256> m_DataOffsets = { 0 };

		public:
			__forceinline ~MapLevel1();
			__forceinline const bool contains(uint32 idx) const {
				return m_Data[idx] != nullptr;
			}
			__forceinline Chunk & __restrict operator [] (uint32 idx);
			__forceinline ChunkOffset & __restrict get_offsets(uint32 idx);
		};

		class MapLevel2 final {
				std::array<MapLevel1* __restrict, 256> m_Data = { 0 };
		public:
			__forceinline ~MapLevel2() {
				for (auto * __restrict ptr : m_Data) {
					delete ptr;
				}
			}
			__forceinline const bool contains(uint32 idx) const {
				return m_Data[idx] != nullptr;
			}
			__forceinline MapLevel1 & __restrict operator [] (uint32 idx) {
				MapLevel1 * __restrict &result = m_Data[idx];
				if (!result) {
					result = new MapLevel1;
				}
				return *result;
			}
		};

		class MapLevel3 final {
			std::array<MapLevel2 * __restrict, (1 << RemainingLog2)> m_Data = { 0 };
		public:
			__forceinline ~MapLevel3() {
				for (auto * __restrict ptr : m_Data) {
					delete ptr;
				}
			}
			__forceinline const bool contains(uint32 idx) const {
				return m_Data[idx] != nullptr;
			}
			__forceinline MapLevel2 & __restrict operator [] (uint32 idx) {
				MapLevel2 * __restrict &result = m_Data[idx];
				if (!result) {
					result = new MapLevel2;
				}
				return *result;
			}
		};

		MapLevel3 m_JitMap;

		Chunk * __restrict m_LastChunk = nullptr;
		ChunkOffset * __restrict m_LastChunkOffset = nullptr;
		uint32 m_LastChunkAddress = uint32(-1);
		std::vector<Chunk *> m_Chunks;
		uint32 m_CurrentExecutingChunkAddress;
		uint32 m_FlushAddress;
		Chunk * __restrict m_FlushChunk;
		size_t m_largest_instruction = 0;

		void populate_chunk(ChunkOffset & __restrict chunk_offset, Chunk & __restrict chunk, uint32 start_address, bool update) __restrict;
	public:
		using jit_instructionexec_t = uint64 (*)  (uint64 instruction, uint64 processor, uint32 pc_runner, uint64, uint64, uint64);

		jit2(processor & __restrict _processor);
		~jit2();

		void execute_instruction(uint32 address);
		jit_instructionexec_t get_instruction(uint32 address);
		jit_instructionexec_t fetch_instruction(uint32 address);
		Chunk * get_chunk(uint32 address);
		bool memory_touched(uint32 address);

		size_t get_max_instruction_size() const __restrict {
			return m_largest_instruction;
		}
	};
}
