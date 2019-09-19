#pragma once

#include <unordered_map>
#include <array>
#include "mips_common.hpp"
#include <cassert>
#include <list>

namespace mips
{
   namespace instructions
   {
      struct InstructionInfo;
   };

   template <typename T>
   inline uint8 byte_assert(T value)
   {
      assert(value <= 127);
			__assume(value <= 127);
      return uint8(value);
   }

   template <typename T>
   inline uint32 dword_assert(T value)
   {
      assert(value <= 2147483647);
			__assume(value <= 2147483647);
      return uint32(value);
   }

   template <typename T>
   inline int16 sword_assert(T _value)
   {
      const typename std::make_signed<T>::type value = _value;
      assert(value >= -32768 && value <= 32767);
			__assume(value >= -32768 && value <= 32767);
      return int16(value);
   }

   template <typename T>
   inline int8 sbyte_assert(T _value)
   {
      const typename std::make_signed<T>::type value = _value;
			assert(value >= -128 && value <= 127);
			__assume(value >= -128 && value <= 127);
      return int8(value);
   }

   template<typename T, typename U> inline constexpr size_t offset_of(U T::*member)
   {
       return (char*)&((T*)nullptr->*member) - (char*)nullptr;
   }

   class processor;
   class jit1
   {
      friend class Jit1_CodeGen;

      template <size_t x>
      struct log2 { enum { value = 1 + log2<x/2>::value }; };
  
      template <> struct log2<1> { enum { value = 1 }; };

      static constexpr uint32 ChunkSize = 0x100; // ChunkSize represents the size for MIPS memory.
      static constexpr size_t MaxChunkRealSize = (ChunkSize / 0x100) * 8192;
      static constexpr size_t ChunkSizeLog2 = log2<ChunkSize - 1>::value;
      static constexpr size_t RemainingLog2 = 32 - ChunkSizeLog2 - 8 - 8;
      static constexpr size_t NumInstructionsChunk = ChunkSize / 4;
      using ChunkOffset = std::array<uint32, NumInstructionsChunk>;
      struct Chunk
      {
         uint32 m_offset;
         ChunkOffset * __restrict m_chunk_offset;
         const uint8 * __restrict m_data;
         uint32 m_datasize;
         // TODO there are better ways to handle this that don't require reconfiguring the entire chunk.
         bool m_has_fixups = false;
         struct patch
         {
            uint32 offset;
            uint32 target;
         };
         std::list<patch> m_patches;
      };

      template <typename T>
      struct identity_hash
      {
         __forceinline T operator()(T v) const __restrict
         {
            return v;
         }
      };

      processor & __restrict m_processor;

      class MapLevel1
      {
         std::array<Chunk * __restrict, 256> m_Data;
         std::array<ChunkOffset * __restrict, 256> m_DataOffsets;

      public:
         __forceinline MapLevel1()
         {
            memset(m_Data.data(), 0, m_Data.size() * sizeof(Chunk *));
            memset(m_DataOffsets.data(), 0, m_DataOffsets.size() * sizeof(ChunkOffset *));
         }
         __forceinline ~MapLevel1();
         __forceinline const bool contains(uint32 idx) const __restrict
         {
            return m_Data[idx] != nullptr;
         }
         __forceinline Chunk & __restrict operator [] (uint32 idx) __restrict;
         __forceinline ChunkOffset & __restrict get_offsets(uint32 idx) __restrict;
      };

      class MapLevel2
      {
         std::array<MapLevel1 * __restrict, 256> m_Data;
      public:
         __forceinline MapLevel2()
         {
            memset(m_Data.data(), 0, m_Data.size() * sizeof(MapLevel1 *));
         }
         __forceinline ~MapLevel2()
         {
            for (auto * __restrict ptr : m_Data)
            {
               delete ptr;
            }
         }
         __forceinline const bool contains(uint32 idx) const __restrict
         {
            return m_Data[idx] != nullptr;
         }
         __forceinline MapLevel1 & __restrict operator [] (uint32 idx) __restrict
         {
            MapLevel1 * __restrict result = m_Data[idx];
            if (!result)
            {
               m_Data[idx] = result = new MapLevel1;
            }
            return *result;
         }
      };

      class MapLevel3
      {
         std::array<MapLevel2 * __restrict, (1 << RemainingLog2)> m_Data;
      public:
         __forceinline MapLevel3()
         {
            memset(m_Data.data(), 0, m_Data.size() * sizeof(MapLevel2 *));
         }
         __forceinline ~MapLevel3()
         {
            for (auto * __restrict ptr : m_Data)
            {
               delete ptr;
            }
         }
         __forceinline const bool contains(uint32 idx) const __restrict
         {
            return m_Data[idx] != nullptr;
         }
         __forceinline MapLevel2 & __restrict operator [] (uint32 idx) __restrict
         {
            MapLevel2 * __restrict result = m_Data[idx];
            if (!result)
            {
               m_Data[idx] = result = new MapLevel2;
            }
            return *result;
         }
      };

      MapLevel3 m_JitMap;

      Chunk * __restrict m_LastChunk = nullptr;
      ChunkOffset * __restrict m_LastChunkOffset = nullptr;
      uint32 m_LastChunkAddress = uint32(-1);
      std::vector<Chunk * __restrict> m_Chunks;
      uint32 m_CurrentExecutingChunkAddress;
      uint32 m_FlushAddress;
      Chunk * __restrict m_FlushChunk;
      size_t m_largest_instruction = 0;

      void populate_chunk(ChunkOffset & __restrict chunk_offset, Chunk & __restrict chunk, uint32 start_address, bool update) __restrict;
   public:
      using jit_instructionexec_t = uint64 (*)  (uint64 instruction, uint64 processor, uint32 pc_runner, uint64, uint64, uint64);

      jit1(processor & __restrict _processor);
      ~jit1();

      __forceinline void execute_instruction(uint32 address) __restrict;
      __forceinline jit_instructionexec_t get_instruction(uint32 address) __restrict;
      Chunk * get_chunk(uint32 address) __restrict;
      bool memory_touched(uint32 address) __restrict;

      size_t get_max_instruction_size() const __restrict
      {
         return m_largest_instruction;
      }
   };
}
