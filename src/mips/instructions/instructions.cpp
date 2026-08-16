#include "pch.hpp"
#include "instructions.hpp"

#include <common.hpp>

#if !USE_STATIC_INSTRUCTION_SEARCH
#	include <limits>
#	include <mutex>
#	include <vector>
#endif

#include "instructions_common.hpp"
#include "mips/mips_common.hpp"
#if !VEMIPS_TABLEGEN
#	include "instructions_table.hpp"
#endif

using namespace mips;

namespace mips::instructions
{
	// A significant amount of logic used to do lookups on instructions.
#if !USE_STATIC_INSTRUCTION_SEARCH
	constexpr const bool reverse_mask_order = true;
	
	_nothrow StaticInitVars& GetStaticInitVars() noexcept
	{
		static StaticInitVars* const InitVarsPtr = new StaticInitVars;
		xassert(InitVarsPtr != nullptr);
		return *InitVarsPtr;
	}

	static void populate_map(
		MapOrInfo& current_map,
		std::vector<FullProcInfo>& proc_infos
	)
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
				if (!proc_infos.empty())
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

		for (const auto& procInfo : proc_infos)
		{
			ThisSigMask &= procInfo.InstructionMask;
		}
		xassert(ThisSigMask != 0);

		current_map.Mask = ThisSigMask;

		const auto FindGreatestMask = [&]() -> const FullProcInfo &
		{
			size_t bits = reverse_mask_order ? std::numeric_limits<size_t>::max() : 0;
			const FullProcInfo* match = nullptr;
			for (const auto& procInfo : proc_infos)
			{
				size_t this_bits = reverse_mask_order ? std::numeric_limits<size_t>::max() : 0;
				uint32 mask = procInfo.InstructionMask;
				for (size_t i = 0; i < std::numeric_limits<decltype(mask)>::digits; ++i)
				{
					const size_t mask_bits = ((mask >> i) & 1) ? 1 : 0;
					if (reverse_mask_order)
					{
						this_bits -= mask_bits;
					}
					else
					{
						this_bits += mask_bits;
					}
				}
				if (
					( reverse_mask_order && this_bits <= bits) ||
					(!reverse_mask_order && this_bits >  bits)
				)
				{
					bits = this_bits;
					match = &procInfo;
				}
			}
			xassert(match != nullptr);
			return *match;
		};

		// if ALL masks match this, we need to subdivide and use default again.
		{
			uint32 NewMask = uint32(-1);
			bool SetNewMask = false;
			bool AllMatch = true;
			for (const auto& procInfo : proc_infos)
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
					const FullProcInfo& procInfo = *iter;
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
				if (!proc_infos.empty())
				{
					next_tier = proc_infos;
					current_map.Default = new MapOrInfo;
					current_map.Default->init_map();
					populate_map(*current_map.Default, next_tier);
				}
				return;
			}
		}

		while (!proc_infos.empty())
		{
			uint32 NewMask = uint32(-1);
			bool SetNewMask = false;

			{
				const FullProcInfo& procInfo = FindGreatestMask();
				// get iter
				if (!SetNewMask || NewMask == (procInfo.RefMask & ThisSigMask))
				{
					NewMask = (procInfo.RefMask & ThisSigMask);
					SetNewMask = true;
					next_tier.push_back(procInfo);
					if (
						const auto iter = std::ranges::find_if(
							proc_infos,
							[&procInfo](const FullProcInfo& __restrict item)
							{
								return item.InstructionMask == procInfo.InstructionMask && item.RefMask == procInfo.RefMask;	
							}
						);
						iter != proc_infos.end()
					)
					{
						proc_infos.erase(iter);						
					}
				}
			}

			for (auto iter = proc_infos.begin(); iter != proc_infos.end();)
			{
				const FullProcInfo& procInfo = *iter;

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

			if (!next_tier.empty())
			{
				auto& next_map = current_map.Map[NewMask];
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
		static std::mutex AlreadyBuiltLock;
		{
			std::unique_lock lock{AlreadyBuiltLock};

			if (AlreadyBuilt)
			{
				return;
			}
			AlreadyBuilt = true;
		}

		auto& staticInitVars = GetStaticInitVars();
		std::vector<FullProcInfo> procInfos = staticInitVars.g_ProcInfos; // by-copy is intentional, if I recall
		staticInitVars.g_LookupMap.init_map();
		populate_map(staticInitVars.g_LookupMap, procInfos);
	}
#endif
}


namespace mips
{
	_nothrow const instructions::InstructionInfo* FindExecuteInstruction(const instruction_t instruction) noexcept
	{
#if USE_STATIC_INSTRUCTION_SEARCH
		return instructions::get_instruction(instruction);
#else
		const auto* __restrict current_map = &instructions::GetStaticInitVars().g_LookupMap;
		while (current_map)
		{
			if (current_map->IsMap)
			{
				const uint32 mask = current_map->Mask;
				const uint32 masked_instruction = instruction & mask;
				if (
					auto fiter = current_map->Map.find(masked_instruction);
					fiter != current_map->Map.end()
				)
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
