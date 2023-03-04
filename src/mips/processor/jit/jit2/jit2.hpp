#pragma once

#include <unordered_map>
#include <array>
#include "mips_common.hpp"
#include <cassert>
#include <list>
#include <limits>

namespace mips {
	namespace instructions {
		struct InstructionInfo;
	};

	class processor;
	class jit2 final {
	public:
		using jit_instructionexec_t = uint64 (*) (instruction_t instruction, mips::processor& __restrict processor);
	private:

		struct instruction_wrapper final {
			const instructions::InstructionInfo* instruction_info;
			instruction_t instruction;
			uint32 address;
			CPU_Exception::Type exception = CPU_Exception::Type(-1);
		};

		template <size_t x>
		struct log2 { enum { value = 1 + log2<x/2>::value }; };
  
		template <> struct log2<1> { enum { value = 1 }; };

		static constexpr const uint32 ChunkSize = 0x100; // ChunkSize represents the size for MIPS memory.
		static constexpr const size_t MaxChunkRealSize = (ChunkSize / 4) * sizeof(uintptr);
		static constexpr const size_t ChunkSizeLog2 = log2<ChunkSize - 1>::value;
		static constexpr const size_t RemainingLog2 = 32 - ChunkSizeLog2 - 8 - 8;
		static constexpr const size_t NumInstructionsChunk = ChunkSize / 4;
		struct Chunk final {
			std::array<instruction_wrapper, NumInstructionsChunk>* data = nullptr;
			uint32 offset;

			~Chunk() {
				delete data;
			}

			_nothrow void populate_data() __restrict {
				xassert(data == nullptr);
				data = new std::remove_pointer_t<decltype(data)>;
			}
		};

		template <typename T>
		struct identity_hash final {
			_forceinline T operator()(T v) const __restrict {
				return v;
			}
		};

		processor & __restrict m_processor;

		class MapLevel1 final {
			std::array<Chunk * __restrict, 256> data_ = { 0 };

		public:
			_forceinline ~MapLevel1();
			_forceinline bool contains(uint32 idx) const {
				return data_[idx] != nullptr;
			}
			_forceinline Chunk & operator [] (uint32 idx);
		};

		class MapLevel2 final {
				std::array<MapLevel1* __restrict, 256> data_ = { 0 };
		public:
			_forceinline ~MapLevel2() {
				for (auto * __restrict ptr : data_) {
					delete ptr;
				}
			}
			_forceinline bool contains(uint32 idx) const {
				return data_[idx] != nullptr;
			}
			_forceinline MapLevel1 & operator [] (uint32 idx) {
				MapLevel1 * __restrict &result = data_[idx];
				if (!result) {
					result = new MapLevel1;
				}
				return *result;
			}
		};

		class MapLevel3 final {
			std::array<MapLevel2 * __restrict, (1 << RemainingLog2)> data_ = { 0 };
		public:
			_forceinline ~MapLevel3() {
				for (auto * __restrict ptr : data_) {
					delete ptr;
				}
			}
			_forceinline bool contains(uint32 idx) const {
				return data_[idx] != nullptr;
			}
			_forceinline MapLevel2 & operator [] (uint32 idx) {
				MapLevel2 * __restrict &result = data_[idx];
				if (!result) {
					result = new MapLevel2;
				}
				return *result;
			}
		};

		MapLevel3 m_JitMap;

		Chunk * __restrict m_LastChunk = nullptr;
		uint32 m_LastChunkAddress = uint32(-1);
		std::vector<Chunk *> m_Chunks;
		uint32 m_CurrentExecutingChunkAddress = {};
		uint32 m_FlushAddress = {};
		Chunk * __restrict m_FlushChunk = {};
		size_t m_largest_instruction = 0;

		void populate_chunk(Chunk & __restrict chunk, uint32 start_address, bool update) __restrict;
	public:
		jit2(processor & __restrict _processor);
		~jit2();

		void execute_instruction(uint32 address);
		jit2::instruction_wrapper get_instruction(uint32 address);
		jit2::instruction_wrapper fetch_instruction(uint32 address);
		Chunk * get_chunk(uint32 address);
		struct chunk_data final {
			const Chunk* chunk;
			uint32 offset;
		};
		chunk_data get_chunk_offset(uint32 address);
		bool memory_touched(uint32 address);

		size_t get_max_instruction_size() const __restrict {
			return m_largest_instruction;
		}
	};
}
