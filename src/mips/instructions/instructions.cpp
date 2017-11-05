#include "pch.hpp"
#include "instructions.hpp"
#include "mips/processor/processor.hpp"
#include "mips/coprocessor/coprocessor.hpp"

#include <cassert>

#include <unordered_map>
#include <unordered_set>

using namespace mips;

namespace mips::instructions
{
   // A significant amount of logic used to do lookups on instructions.
#if !USE_STATIC_INSTRUCTION_SEARCH
   StaticInitVars * __restrict StaticInitVarsPtr = nullptr;

   void populate_map(MapOrInfo &current_map, std::vector<FullProcInfo> &proc_infos)
   {
      std::vector<FullProcInfo> next_tier;
   
      uint32 ThisSigMask = uint32(-1);

      const auto MaskFilter = [&](uint32 mask) -> bool
      {
         bool MaskFiltered = false;

         for (auto iter = proc_infos.begin(); iter != proc_infos.end();)
         {
            const FullProcInfo &procInfo = *iter;
            if (procInfo.InstructionMask == ThisSigMask)
            {
               MaskFiltered = true;

               auto &next_map = current_map.Map[(procInfo.RefMask)];
               next_map = new MapOrInfo;
               next_map->Info = procInfo.ProcInfo;

               iter = proc_infos.erase(iter);
            }
            else
            {
               ++iter;
            }
         }

         if (MaskFiltered)
         {
            current_map.Mask = ThisSigMask;//mask;
            if (proc_infos.size())
            {
               next_tier = proc_infos;
               current_map.Default = new MapOrInfo;
               current_map.Default->init_map();
               populate_map(*current_map.Default, next_tier);
            }
            return true;
         }
         return false;
      };

      // Filter out anything matching this mask.
      if (MaskFilter(ThisSigMask))
      {
         return;
      }

#if WIDE_INSTRUCTION_BRANCHES
      ThisSigMask = 0;

      // Filter out by an OR mask
      for (const auto &procInfo : proc_infos)
      {
         ThisSigMask |= procInfo.InstructionMask;
      }

      // Filter out anything matching this mask.
      if (MaskFilter(ThisSigMask))
      {
         return;
      }

      ThisSigMask = uint32(-1);
#endif

      for (auto iter = proc_infos.begin(); iter != proc_infos.end(); ++iter)
      {
         const FullProcInfo &procInfo = *iter;

         ThisSigMask &= procInfo.InstructionMask;
      }
      assert(ThisSigMask != 0);

      current_map.Mask = ThisSigMask;

      const auto FindGreatestMask = [&]() -> const FullProcInfo &
      {
         size_t bits = 0;
         const FullProcInfo *match = nullptr;
         for (const auto &procInfo : proc_infos)
         {
            size_t this_bits = 0;
            uint32 mask = procInfo.InstructionMask;
            for (size_t i = 0; i < 32; ++i)
            {
               this_bits += ((mask >> i) & 1) ? 1 : 0;
            }
            if (this_bits > bits)
            {
               bits = this_bits;
               match = &procInfo;
            }
         }
         return *match;
      };

      // if ALL masks match this, we need to subdivide and use default again.
      {
         uint32 NewMask = uint32(-1);
         bool SetNewMask = false;
         bool AllMatch = true;
         for (const auto &procInfo : proc_infos)
         {
            if (!SetNewMask || NewMask == (procInfo.RefMask & ThisSigMask))
            {
               NewMask = (procInfo.RefMask & ThisSigMask);
               SetNewMask = true;
            }
            else
            {
               AllMatch = false;
            }
         }
         if (AllMatch)
         {
            // Use the first one as a match.
            const uint32 NewMask2 = FindGreatestMask().InstructionMask;

            for (auto iter = proc_infos.begin(); iter != proc_infos.end();)
            {
               const FullProcInfo &procInfo = *iter;
               if (procInfo.InstructionMask == NewMask2)
               {
                  auto &next_map = current_map.Map[(procInfo.RefMask)];
                  next_map = new MapOrInfo;
                  next_map->Info = procInfo.ProcInfo;

                  iter = proc_infos.erase(iter);
               }
               else
               {
                  ++iter;
               }
            }

            current_map.Mask = NewMask2;//mask;
            if (proc_infos.size())
            {
               next_tier = proc_infos;
               current_map.Default = new MapOrInfo;
               current_map.Default->init_map();
               populate_map(*current_map.Default, next_tier);
            }
            return;
         }
      }

      while (proc_infos.size())
      {
         uint32 NewMask = uint32(-1);
         bool SetNewMask = false;

         {
            const FullProcInfo &procInfo = FindGreatestMask();
            // get iter
            if (!SetNewMask || NewMask == (procInfo.RefMask & ThisSigMask))
            {
               NewMask = (procInfo.RefMask & ThisSigMask);
               SetNewMask = true;
               next_tier.push_back(procInfo);
               for (auto iter = proc_infos.begin(); iter != proc_infos.end();)
               {
                  if (iter->InstructionMask == procInfo.InstructionMask && iter->RefMask == procInfo.RefMask)
                  {
                     proc_infos.erase(iter);
                     break;
                  }
                  ++iter;
               }
            }
         }

         for (auto iter = proc_infos.begin(); iter != proc_infos.end();)
         {
            const FullProcInfo &procInfo = *iter;

            if (!SetNewMask || NewMask == (procInfo.RefMask & ThisSigMask))
            {
               NewMask = (procInfo.RefMask & ThisSigMask);
               SetNewMask = true;
               next_tier.push_back(procInfo);
               iter = proc_infos.erase(iter);
            }
            else
            {
               ++iter;
            }
         }

         if (next_tier.size())
         {
            auto &next_map = current_map.Map[NewMask];
            next_map = new MapOrInfo;
            if (next_tier.size() == 1)
            {
               next_map->Info = next_tier.front().ProcInfo;
            }
            else
            {
               next_map->init_map();
               populate_map(*next_map, next_tier);
            }
            next_tier.clear();
         }
      }
   }

   void finish_map_build()
   {
      static bool AlreadyBuilt = false;
      if (AlreadyBuilt)
      {
         return;
      }
      AlreadyBuilt = true;

      std::vector<FullProcInfo> procInfos = instructions::StaticInitVarsPtr->g_ProcInfos;
      instructions::StaticInitVarsPtr->g_LookupMap.init_map();
      populate_map(instructions::StaticInitVarsPtr->g_LookupMap, procInfos);
   }
#endif
}

#if !TABLEGEN
extern const mips::instructions::InstructionInfo * __restrict get_instruction(instruction_t i);
extern bool execute_instruction(instruction_t i, mips::processor & __restrict p);
#endif

namespace mips
{
   const instructions::InstructionInfo * __restrict FindExecuteInstruction(instruction_t instruction)
   {
#if USE_STATIC_INSTRUCTION_SEARCH
      return ::get_instruction(instruction);
#else
      const auto * __restrict current_map = &instructions::StaticInitVarsPtr->g_LookupMap;
      while (current_map)
      {
         if (current_map->IsMap)
         {
            const uint32 mask = current_map->Mask;
            const uint32 masked_instruction = instruction & mask;
            auto fiter = current_map->Map.find(masked_instruction);
            if (fiter != current_map->Map.end())
            {
               current_map = fiter->second;;
            }
            else
            {
               if (current_map->Default)
               {
                  current_map = current_map->Default;
               }
               else
               {
                  current_map = nullptr;
               }
            }
         }
         else
         {
            return &current_map->Info;
         }
      }
   
      return nullptr;
#endif
   }
}
