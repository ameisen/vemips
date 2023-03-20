#pragma once

#define USE_LEVELED_MAP 1

#include "common.hpp"

#include <unordered_map>
#include <array>
#include "mips_common.hpp"
#include <cassert>
#include <list>
#include <limits>
#include "runtime/basic_allocator.hpp"
#include "runtime/directory_table.hpp"
#include "runtime/associate_cache.hpp"

namespace mips {
	namespace instructions {
		struct InstructionInfo;
	}
}

namespace {
	namespace _detail {
		using instruction_info = mips::instructions::InstructionInfo;

		static inline bool is_instruction(const instruction_info &lhs, const instruction_info *rhs) {
			return &lhs == rhs;
		}

		static inline bool is_instruction(const instruction_info &lhs, const instruction_info &rhs) {
			return &lhs == &rhs;
		}
	}
}

#define IS_INSTRUCTION(instr, ref) \
	[&]() -> bool { \
		return _detail::is_instruction(mips::instructions::StaticProc_ ## ref, instr); \
	}()

namespace mips {
	class processor;
	class jit1 final {
		friend class Jit1_CodeGen;

		template <size_t X>
		struct log2 { enum { value = 1 + log2<X/2>::value }; };
  
		template <> struct log2<1> { enum { value = 1 }; };
		template <> struct log2<0> { enum { value = 0 }; };

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
			std::vector<patch> m_patches;
		};

		template <typename T>
		struct identity_hash final {
			_forceinline T operator()(T v) const __restrict {
				return v;
			}
		};

		static _nothrow void free_executable(void*);

		struct chunk_data;
		static void initialize_chunk(chunk_data& __restrict chunk);

		static inline basic_allocator<ChunkOffset> m_chunkoffset_allocator;

		struct chunk_data final {
			Chunk* __restrict chunk = nullptr;
			ChunkOffset* __restrict offset = nullptr;

			
			_nothrow chunk_data() noexcept = default;
			chunk_data(const chunk_data&) = delete;
			_nothrow chunk_data(chunk_data && data) noexcept : chunk(data.chunk), offset(data.offset) {
				data.chunk = nullptr;
				data.offset = nullptr;
			}

			chunk_data& operator = (const chunk_data &) = delete;
			_nothrow chunk_data& operator = (chunk_data&& data) noexcept {
				chunk = data.chunk;
				offset = data.offset;
				data.chunk = nullptr;
				data.offset = nullptr;
				return *this;
			}

			_nothrow ~chunk_data() noexcept {
				m_chunkoffset_allocator.release(offset);
			}

			_forceinline _nothrow void initialize() {
				initialize_chunk(*this);
			}

			_forceinline _nothrow void release() const {
				if (chunk) {
					free_executable(chunk);
				}
			}

			_forceinline _nothrow bool contained() const __restrict {
				return chunk != nullptr;
			}
		};

#if !USE_LEVELED_MAP
		class MapLevel1 final {
			std::array<chunk_data, 256> m_Data = { 0 };

		public:
			~MapLevel1();
			[[nodiscard]]
			_forceinline bool contains(const uint32 idx) const {
				return m_Data[idx].contained();
			}
			_forceinline chunk_data& operator [] (uint32 idx);
		};

		class MapLevel2 final {
				std::array<MapLevel1* __restrict, 256> m_Data = { 0 };
		public:
			_forceinline ~MapLevel2() {
				for (const auto * __restrict ptr : m_Data) {
					delete ptr;
				}
			}
			[[nodiscard]]
			_forceinline bool contains(const uint32 idx) const {
				return m_Data[idx] != nullptr;
			}
			_forceinline MapLevel1 & operator [] (uint32 idx) {
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
			_forceinline ~MapLevel3() {
				for (auto * __restrict ptr : m_Data) {
					delete ptr;
				}
			}
			[[nodiscard]]
			_forceinline bool contains(const uint32 idx) const {
				return m_Data[idx] != nullptr;
			}
			_forceinline MapLevel2 & operator [] (uint32 idx) {
				MapLevel2 * __restrict &result = m_Data[idx];
				if (!result) {
					result = new MapLevel2;
				}
				return *result;
			}
		};
#endif

	public:
		using jit_instructionexec_t = uint64(*)  (uint64 instruction, uint64 processor, uint32 pc_runner, uint64, uint64, uint64);

	private:

#if USE_LEVELED_MAP
		directory_table<chunk_data, true, 1 << RemainingLog2, 256, 256> jit_map_;
#else
		MapLevel3 jit_map;
#endif
		std::vector<Chunk*> chunks_;
		using instruction_cache_t = sentinel_associate_cache<uint32, jit1::jit_instructionexec_t, 0, uintptr(-1)>;
		_no_unique
		instruction_cache_t lookup_cache_;
		processor& __restrict processor_;
		Chunk * __restrict last_chunk_ = nullptr;
		ChunkOffset * __restrict last_chunk_offset_ = nullptr;
		Chunk* __restrict flush_chunk_;
		size_t largest_instruction_ = 0;
		uint32 last_chunk_address_ = uint32(-1);
		uint32 current_executing_chunk_address_;
		uint32 flush_address_;

		void populate_chunk(ChunkOffset & __restrict chunk_offset, Chunk & __restrict chunk, uint32 start_address, bool update);
	public:
		jit1(processor & __restrict _processor);
		~jit1();

		void execute_instruction(uint32 address);
		jit_instructionexec_t get_instruction(uint32 address);
		jit_instructionexec_t fetch_instruction(uint32 address);
		Chunk * get_chunk(uint32 address);
		bool memory_touched(uint32 address);

		[[nodiscard]]
		size_t get_max_instruction_size() const __restrict {
			return largest_instruction_;
		}
	};
}
