#pragma once

#include "mips/config.hpp"
#include <common.hpp>
#include <mips/mips_common.hpp>

#include <cassert>
#include <map>
#include <unordered_map>
#include <unordered_set>

namespace mips
{
	class processor;
	class coprocessor;
	namespace instructions
	{
		// Flags which operations can specify
		enum class OpFlags : uint32
		{
			None = 0,
			ClearsCause = (1U << 0),
			Throws = (1U << 1),
			Signals_InvalidOp = (1U << 2),
			Signals_DivZero = (1U << 3),
			Signals_Overflow = (1U << 4),
			Signals_Underflow = (1U << 5),
			Signals_Inexact = (1U << 6),
			Signals_All = (Signals_InvalidOp | Signals_DivZero | Signals_Overflow | Signals_Underflow | Signals_Inexact),
			CustomDefault = (1U << 7),
			NoWriteOnSignal = (1U << 8),
			Round_ToNearest = (1U << 9),
			Round_ToZero = (1U << 10),
			Round_Up = (1U << 11),
			Round_Down = (1U << 12),
			Round_None = (1U << 13),
			Denorm_Flush = (1U << 14),
			Denorm_Save = (1U << 15),
			Denorm_None = (1U << 16),
			ControlInstruction = (1U << 17),
			DelayBranch = (1U << 18),
			CompactBranch = (1U << 19),
			SetNoCTI = (1U << 20),
				 Load = (1U << 21),
				 Store = (1U << 22),
				 COP1 = (1U << 23),
		};

		constexpr OpFlags operator | (OpFlags a, OpFlags b)
		{
			return OpFlags(uint32(a) | uint32(b));
		}

		constexpr OpFlags operator & (OpFlags a, OpFlags b)
		{
			return OpFlags(uint32(a) & uint32(b));
		}

		static constexpr uint32_t Bits(uint32_t NumBits)
		{
			return (1 << (NumBits)) - 1;
		}

		static constexpr uint32_t HighBits(uint32_t NumBits)
		{
			return ((1 << (NumBits)) - 1) << (32 - NumBits);
		}

		template <size_t BitSize>
		struct TinyInt
		{
			int32_t m_Value : BitSize;

			template <typename T>
			TinyInt(const T &val) : m_Value(val)
			{
			}

			template <typename T>
			T sextend() const
			{
				return T(m_Value);
			}

			template <typename T>
			T zextend() const
			{
				return T(uint32_t(m_Value) & Bits(BitSize));
			}
		};

		typedef uint64(*instructionexec_t) (instruction_t instruction, processor & __restrict processor);

		struct InstructionInfo
		{
			const char * __restrict Name;
			instructionexec_t Proc;
			uint32 CoprocessorIdx;
			uint32 OpFlags;
			bool ControlInstruction;
			char Type;

			InstructionInfo() = default;
			InstructionInfo(const char * __restrict _Name, bool _CT, uint32 coprocessor, instructionexec_t proc, uint32 _OpFlags, char type = 'n') :
				Name(_Name), Proc(proc), CoprocessorIdx(coprocessor), ControlInstruction(_CT), OpFlags(_OpFlags), Type(type) {}
		};

#if !USE_STATIC_INSTRUCTION_SEARCH

		// A hash operation to use that just uses the value as a hash
		struct identity_hash
		{
			uint32 operator()(uint32 v) const __restrict
			{
				return v;
			}
		};

#if USE_GOOGLE_DENSE_HASH_MAP
		template <typename T> using identity_map = google::dense_hash_map<uint32, T>;
#elif USE_GOOGLE_SPARSE_HASH_MAP
		template <typename T> using identity_map = google::sparse_hash_map<uint32, T>;
#elif USE_HASH_MAP
		template <typename T> using identity_map = std::unordered_map<uint32, T, identity_hash>;
#else
		template <typename T> using identity_map = std::map<uint32, T>;
#endif
		using identity_set = std::unordered_set<uint32>;

		struct MapOrInfo
		{
			bool IsMap;
			uint32 Mask;
			union
			{
				InstructionInfo Info;
				struct
				{
					identity_map<MapOrInfo *> Map;
					MapOrInfo *Default;
				};
			};

			void init_map()
			{
				IsMap = true;
				new (&Map) identity_map<MapOrInfo *>;
#if USE_GOOGLE_SPARSE_HASH_MAP
				Map.set_deleted_key(uint32(-1));
#elif USE_GOOGLE_DENSE_HASH_MAP
				Map.set_empty_key(uint32(-1));
				Map.set_deleted_key(uint32(-2));
#endif
				Default = nullptr;
			}

			MapOrInfo() : IsMap(false) {}
			~MapOrInfo()
			{
				if (IsMap)
				{
					for (auto &map : Map)
					{
						delete map.second;
					}
					Map.~identity_map<MapOrInfo *>();
					delete Default;
				}
			}
		};
		struct FullProcInfo
		{
			uint32 InstructionMask;
			uint32 RefMask;
			InstructionInfo ProcInfo;
		};

		struct StaticInitVars
		{
			MapOrInfo g_LookupMap;
			std::unordered_set<uint32, identity_hash> g_InstructionMasks;
			std::vector<FullProcInfo> g_ProcInfos;
		};
		extern StaticInitVars * __restrict StaticInitVarsPtr;
#endif

		template <uint32 offset, uint32 size, typename T>
		class _Register
		{
		protected:
			template<uint32, uint32, typename> friend class _Register;
			using processor_t = T;
			const uint32 m_Register;
			processor_t & __restrict m_Processor;
			static uint32 _get_register(instruction_t instruction) { return (instruction >> offset) & Bits(size); }
		public:
			_Register(instruction_t instruction, processor_t & __restrict processor) :
				m_Register(_get_register(instruction)), m_Processor(processor) {}

			template <uint32 _offset, uint32 _size>
			bool operator == (const _Register<_offset, _size, T> & __restrict reg) const
			{
				return m_Register == reg.m_Register;
			}

			template <uint32 _offset, uint32 _size>
			bool operator != (const _Register<_offset, _size, T> & __restrict reg) const
			{
				return m_Register != reg.m_Register;
			}

			uint32 get_register() const
			{
				return m_Register;
			}

			template <typename format_t>
			format_t value() const
			{
				return m_Processor.template get_register<format_t>(m_Register);
			}

			template <typename format_t>
			format_t set(format_t value)
			{
				assert(sizeof(typename processor_t::register_type) >= sizeof(format_t));
				m_Processor.template set_register<format_t>(m_Register, value);
				return value;
			}
		};

		template <uint32 offset, uint32 size>
		class GPRegister : public _Register<offset, size, processor>
		{
		public:
			GPRegister(instruction_t instruction, processor & __restrict _processor) :
				_Register<offset, size, processor>(instruction, _processor) {}
		};

	}
}
