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

		static constexpr uint32 Bits(uint32 NumBits)
		{
			return (1 << (NumBits)) - 1;
		}

		static constexpr uint32 HighBits(uint32 NumBits)
		{
			return ((1 << (NumBits)) - 1) << (32 - NumBits);
		}

		template <typename TFrom, typename TTo>
		static constexpr bool in_range(const TFrom value)
		{
			return
				value >= std::numeric_limits<TTo>::lowest() &&
				value <= std::numeric_limits<TTo>::max();
		}

		template <size_t BitSize>
		struct TinyInt
		{
			int32 m_Value : BitSize;

			template <typename T>
			TinyInt(const T &val) : m_Value(val)
			{
			}

			template <typename T>
			T sextend() const
			{
				// todo: value range check
				return T(m_Value);
			}

			template <typename T>
			T zextend() const
			{
				// todo: value range check
				return T(uint32(m_Value) & Bits(BitSize));
			}
		};

		typedef uint64(*instructionexec_t) (instruction_t instruction, processor & __restrict processor);

		enum class instruction_type : uint8 {
			normal = 0,
			single_fp = 1,
			double_fp = 2,
			word_fp = 3,
			long_fp = 4,
			void_fp = 5,
		};

		struct instruction_flags final {
			bool control : 1;
		};

		struct InstructionInfo
		{
			const char * Name;
			instructionexec_t Proc;
			uint32 OpFlags;
			uint8 CoprocessorIdx;
			struct {
				instruction_type Type : 3;
				instruction_flags Flags;
			};

			InstructionInfo(
				const char* __restrict name,
				uint8 coprocessor,
				instructionexec_t proc,
				uint32 op_flags,
				instruction_flags flags ,
				instruction_type type = instruction_type::normal
			) :
				Name(name),
				Proc(proc),
				OpFlags(op_flags),
				CoprocessorIdx(coprocessor),
				Type(type),
				Flags(flags)
			{}

			InstructionInfo(const InstructionInfo&) = default;
		};

		static constexpr const size_t foo = sizeof(InstructionInfo);

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

		class _RegisterBase
		{
		protected:
			const uint32 m_Register;

		protected:
			_RegisterBase(const uint32 _register)
				: m_Register(_register)
			{}

		public:

			int8 get_offset_gp() const;
			int16 get_offset_fp() const;
		};

		template <uint32 offset, uint32 size, typename T>
		class _Register : public _RegisterBase
		{
		protected:
			template<uint32, uint32, typename> friend class _Register;
			using processor_t = T;
			processor_t * __restrict m_Processor = nullptr;
			using _RegisterBase::m_Register;
			static uint32 _get_register(instruction_t instruction) { return (instruction >> offset) & Bits(size); }
		public:
			_Register(instruction_t instruction, processor_t& processor) :
				_RegisterBase(_get_register(instruction)), 
				m_Processor(&processor) {}

			_Register(instruction_t instruction, processor_t* processor) :
				_RegisterBase(_get_register(instruction)), 
				m_Processor(processor) {}

			_Register(const uint32 _register) :
				_RegisterBase(_register) {}

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

			template <uint32 _offset, uint32 _size>
			auto operator <=> (const _Register<_offset, _size, T> & __restrict reg) const
			{
				return m_Register <=> reg.m_Register;
			}

			uint32 get_register() const
			{
				return m_Register;
			}

			uint32 get_index() const
			{
				xassert(m_Register != 0);

				return m_Register /*- 1*/;
			}

			template <typename format_t>
			format_t value() const
			{
				xassert(m_Processor != nullptr);

				return m_Processor->template get_register<format_t>(m_Register);
			}

			template <typename format_t>
			format_t set(format_t value)
			{
				xassert(m_Processor != nullptr);

				xassert(sizeof(typename processor_t::register_type) >= sizeof(format_t));
				m_Processor->template set_register<format_t>(m_Register, value);
				return value;
			}
		};

		template <uint32 offset = 0, uint32 size = 0>
		class GPRegister : public _Register<offset, size, processor>
		{
		public:
			GPRegister(instruction_t instruction, processor& _processor) :
				_Register<offset, size, processor>(instruction, _processor) {}

			GPRegister(instruction_t instruction, processor* _processor) :
				_Register<offset, size, processor>(instruction, _processor)
			{
				xassert(_processor != nullptr);
			}

			GPRegister(const uint32 _register) :
				_Register<offset, size, processor>(_register) {}

			_forceinline bool is_zero() const
			{
				return _RegisterBase::m_Register == 0U;
			}

			_forceinline bool is_constant() const
			{
				return is_zero();
			}

			_forceinline std::optional<int32> get_constant() const
			{
				if (is_zero())
				{
					return 0;
				}
				else
				{
					xassert(!is_constant());

					return {};
				}
			}

			_forceinline int8 get_offset(const bool force = false) const
			{
				if (!force)
				{
					xassert(!is_zero());
				}

				return _RegisterBase::get_offset_gp();
			}
		};

	}
}
