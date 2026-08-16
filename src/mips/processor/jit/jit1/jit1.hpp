#pragma once

#define USE_LEVELED_MAP 1
#define USE_CACHE 1

#include "common.hpp"
#include "mips_common.hpp"

#include <bit>
#include <array>
#include <cassert>

#include "jit1_xbyak.hpp"
#include "processor/jit/jit.hpp"

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

		[[nodiscard]]
		_func_const
		static inline _nothrow bool is_instruction(const instruction_info &lhs, const instruction_info *rhs) noexcept {
			return &lhs == rhs;
		}

		[[nodiscard]]
		_func_const
		static inline _nothrow bool is_instruction(const instruction_info &lhs, const instruction_info &rhs) noexcept {
			return &lhs == &rhs;
		}

		struct hazard_barrier_instruction final
		{
			bool is_instruction = false;
			bool is_hazard_barrier = false;

			_nothrow hazard_barrier_instruction(const bool _is_instruction, const bool _is_hazard_barrier) noexcept
				: is_instruction(_is_instruction || _is_hazard_barrier)
				, is_hazard_barrier(_is_hazard_barrier)
			{}

			[[nodiscard]]
			_pure // not really pure, but acts like it
			_nothrow operator bool() const noexcept
			{
				return is_instruction;
			}
		};
	}
}

#define IS_INSTRUCTION(instr, ref) \
	[&] () noexcept -> bool { \
		return _detail::is_instruction(mips::instructions::StaticProc_ ## ref, instr); \
	}()

#define IS_INSTRUCTION_HB(instr, ref) \
	[&] () noexcept -> _detail::hazard_barrier_instruction { \
		return { \
			_detail::is_instruction(mips::instructions::StaticProc_ ## ref, instr), \
			_detail::is_instruction(mips::instructions::StaticProc_ ## ref ## _HB, instr) \
		}; \
	}()

namespace mips {
	class processor;

	class jit1 final : public jit_base
	{
		friend class Jit1_CodeGen;

		static constexpr const uint32 ChunkSize = 0x100; // ChunkSize represents the size for MIPS memory.
	public:
		static constexpr const size_t MaxChunkRealSize = std::max(ChunkSize / 0x100, 1U) * 8192;
	private:
		static constexpr const size_t ChunkSizeLog2 = std::bit_width(ChunkSize - 1UZ);
		static constexpr const size_t RemainingLog2 = 32 - ChunkSizeLog2 - 8 - 8;
		static constexpr const size_t NumInstructionsChunk = ChunkSize / 4;

	public:
		using ChunkOffset = std::array<uint32, NumInstructionsChunk>;

		struct patch final {
			uptr base_address;
			uptr_guest target;
			enum class types : uint8
			{
				indeterminate
			} type;

			bool set_pc : 1;

		};
	private:

		struct Chunk final {
			ChunkOffset * __restrict m_chunk_offset = nullptr;
			uint8 * __restrict m_data = nullptr;
			std::optional<instruction_t> last_chunk_last_instruction;
			std::vector<patch> patches;
			uint32 m_offset = 0;
			uint32 m_datasize = 0;
#if 0
			// TODO there are better ways to handle this that don't require reconfiguring the entire chunk.
			bool m_has_fixups = false;
#endif

			// TODO : destructor
			_nothrow void release() noexcept;
		};

		template <typename T>
		struct identity_hash final {
			[[nodiscard]]
			_func_const
			_forceinline _nothrow T operator()(T v) const __restrict noexcept {
				return v;
			}
		};

		struct chunk_data;
		static void initialize_chunk(chunk_data& __restrict chunk);

		static basic_allocator<ChunkOffset> m_chunkoffset_allocator;

		struct chunk_data final {
		private:
			struct chunk_deleter final
			{
				static _nothrow void operator()(Chunk* __restrict chunk) noexcept
				{
					chunk->release();
					delete chunk;
				}
			};

		public:
			std::unique_ptr<Chunk, chunk_deleter> chunk;
			ChunkOffset* __restrict offset = nullptr;
			
			_nothrow chunk_data() noexcept = default;
			chunk_data(const chunk_data&) = delete;
			_nothrow chunk_data(chunk_data && data) noexcept : chunk(std::move(data.chunk)), offset(data.offset) {
				data.offset = nullptr;
			}

			chunk_data& operator = (const chunk_data &) = delete;
			_nothrow chunk_data& operator = (chunk_data&& data) noexcept {
				chunk = std::move(data.chunk);
				offset = data.offset;
				data.offset = nullptr;
				return *this;
			}

			_nothrow ~chunk_data() noexcept;

			_forceinline _nothrow void initialize() noexcept {
				initialize_chunk(*this);
			}

			_nothrow void release() noexcept;

			[[nodiscard]]
			_pure
			_forceinline _nothrow bool contained() const __restrict {
				return static_cast<bool>(chunk);
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
			[[nodiscard]]
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
			[[nodiscard]]
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
			[[nodiscard]]
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
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgcc-compat"
		using jit_instructionexec_t = uint64 (VEMIPS_JIT_ABI_INFIX * VEMIPS_JIT_ABI) (uint64 instruction, uint64 processor, uint32 pc_runner, uint64, uint64, uint64);
#pragma clang diagnostic pop

	private:

#if USE_LEVELED_MAP
		using jit_map_t = directory_map<chunk_data, 1 << RemainingLog2, 256, 256>;
#else
		using jit_map_t = MapLevel3;
#endif
		std::unique_ptr<jit_map_t> jit_map_ = std::make_unique<jit_map_t>();
		std::vector<Chunk*> chunks_;
		using instruction_cache_t = sentinel_associate_cache<uint32, jit1::jit_instructionexec_t, 0, uintptr(-1)>;
		_no_unique
		instruction_cache_t lookup_cache_;
		processor& __restrict processor_;
#if USE_CACHE
		struct cache_element final
		{
			Chunk * __restrict chunk_ = nullptr;
			ChunkOffset * __restrict chunk_offset_ = nullptr;
			uptr_guest chunk_address_ = uptr_guest(-1);
		};
		std::array<cache_element, 6> last_cached;
		uint32 last_cached_index = 0;
#endif
		std::pair<const char*, size_t> largest_instruction_ = { nullptr, 0 };
		std::shared_ptr<char[]> global_exec_data;

		void populate_chunk(ChunkOffset & __restrict chunk_offset, Chunk & __restrict chunk, uptr_guest start_address, bool update);
	public:
		jit1(processor & __restrict _processor);
		~jit1();

		void execute_instruction(uptr_guest address);
		[[nodiscard]]
		VEMIPS_JIT_ABI jit_instructionexec_t VEMIPS_JIT_ABI_INFIX get_instruction(uptr_guest address);
		[[nodiscard]]
		jit_instructionexec_t fetch_instruction(uptr_guest address);
		[[nodiscard]]
		Chunk* get_chunk(uptr_guest address) const;
		[[nodiscard]]
		Chunk* get_chunk_by_pointer(uptr ptr) const noexcept;

		[[nodiscard]]
		_pure
		_nothrow std::pair<const char*, size_t> get_max_instruction_size() const __restrict noexcept {
			return largest_instruction_;
		}

		[[nodiscard]]
		_func_const
		// ReSharper disable once CppMemberFunctionMayBeStatic
		_nothrow uint32 get_chunk_size() const __restrict noexcept {
			return ChunkSize;
		}

		[[nodiscard]]
		_func_const
		static _nothrow uint32 get_static_chunk_size() noexcept
		{
			return ChunkSize;
		}

		[[nodiscard]]
		_pure
		_nothrow _forceinline processor& get_processor() const noexcept
		{
			return processor_;
		}

	private:
		const void* call_instruction_hazard_ptr_ = nullptr;
	};
}
