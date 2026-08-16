#pragma once

#include "mips/config.hpp"
#include <common.hpp>
#include <mips/mips_common.hpp>

#include <bit>
#include <limits>
#include <optional>
#include <utility>

#if !USE_STATIC_INSTRUCTION_SEARCH
#	include <map>
#	include <unordered_map>
#	include <unordered_set>
#endif

#include "mips/coprocessor/coprocessor.hpp"

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
			ReadsGPRegister = (1U << 23),
			WritesGPRegister = (1U << 24),
			COP1 = (1U << 25),
			Hazard = (1U << 26),
			InstructionHazard = (1U << 27),
			ThrowsMisaligned = (1U << 28),
			RequiresCoP0 = (1U << 29),
			ModifiesFPUState = (1U << 30),
			AlwaysThrows = (1U << 31),
		};

		[[nodiscard]]
		_func_const
		constexpr _forceinline _nothrow OpFlags operator | (const OpFlags a, const OpFlags b) noexcept
		{
			return OpFlags(std::to_underlying(a) | std::to_underlying(b));
		}

		[[nodiscard]]
		_func_const
		constexpr _forceinline _nothrow OpFlags operator & (const OpFlags a, const OpFlags b) noexcept
		{
			return OpFlags(std::to_underlying(a) & std::to_underlying(b));
		}

		[[nodiscard]]
		_func_const
		constexpr _forceinline _nothrow bool HasAllFlags(const OpFlags ref, const OpFlags flags) noexcept
		{
			return (ref & flags) == flags;
		}

		[[nodiscard]]
		_func_const
		constexpr _forceinline _nothrow bool HasAnyFlags(const OpFlags ref, const OpFlags flags) noexcept
		{
			return (ref & flags) != OpFlags::None;
		}

		[[nodiscard]]
		_func_const
		constexpr _forceinline _nothrow bool HasFlag(const OpFlags ref, const OpFlags flag) noexcept
		{
			return HasAnyFlags(ref, flag);
		}

		[[nodiscard]]
		_func_const
		static constexpr _forceinline _nothrow uint32 Bits(const uint32 NumBits) noexcept
		{
			return (1 << (NumBits)) - 1;
		}

		[[nodiscard]]
		_func_const
		static constexpr _forceinline _nothrow uint32 HighBits(const uint32 NumBits) noexcept
		{
			return ((1 << (NumBits)) - 1) << (32 - NumBits);
		}

		template <typename TFrom, typename TTo>
		[[nodiscard]]
		_func_const
		static constexpr _forceinline _nothrow bool in_range(const TFrom value) noexcept
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
			_forceinline _nothrow TinyInt(const T &val) noexcept : m_Value(val)
			{
			}

			template <typename T>
			[[nodiscard]]
			_pure
			_forceinline _nothrow T sextend() const noexcept
			{
				// todo: value range check
				return T(m_Value);
			}

			template <typename T>
			[[nodiscard]]
			_pure
			_forceinline _nothrow T zextend() const noexcept
			{
				// todo: value range check
				const T result = T(uint32(m_Value) & Bits(BitSize));
				return result;
			}
		};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgcc-compat"
		using instructionexec_t = void(VEMIPS_JIT_ABI_INFIX * VEMIPS_JIT_ABI)(instruction_t instruction, processor & __restrict processor);
#pragma clang diagnostic pop

		enum class instruction_type : uint8 {
			normal = 0,
			single_fp = 1,
			double_fp = 2,
			word_fp = 3,
			long_fp = 4,
			void_fp = 5,

			_max,
			_bits = std::bit_width(_max - 1U)
		};

		struct instruction_flags final {
			bool control : 1;

			static constexpr const size_t Bits = 1;
		};

		//_Pragma("pack(push, 8)")
		struct InstructionInfo final
		{
			const char* __restrict Name;
			instructionexec_t Proc;
			OpFlags OpFlags;
			uint8 CoprocessorIdx : std::bit_width(coprocessor::max - 1U);
			instruction_type Type : std::to_underlying(instruction_type::_bits);
			instruction_flags Flags/* : instruction_flags::Bits*/;

			_nothrow InstructionInfo(
				const char* const __restrict name,
				const uint8 coprocessor,
				const instructionexec_t proc,
				const instructions::OpFlags op_flags,
				const instruction_flags flags,
				const instruction_type type = instruction_type::normal
			) noexcept :
				Name(name),
				Proc(proc),
				OpFlags(op_flags),
				CoprocessorIdx(coprocessor),
				Type(type),
				Flags(flags)
			{
				xassert(coprocessor::is_valid(coprocessor));
			}

			_nothrow InstructionInfo(const InstructionInfo&) noexcept = default;
			_nothrow InstructionInfo(InstructionInfo&&) noexcept = default;
			_nothrow ~InstructionInfo() noexcept = default;

			_nothrow InstructionInfo& operator=(const InstructionInfo&) noexcept = default;
			_nothrow InstructionInfo& operator=(InstructionInfo&&) noexcept = default;
		};
		//_Pragma("pack(pop)")

#if !USE_STATIC_INSTRUCTION_SEARCH
		// A hash operation to use that just uses the value as a hash
		struct identity_hash
		{
			_nothrow uint32 operator()(uint32 v) const __restrict noexcept
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
		template <typename T> using identity_set_t = std::unordered_set<T>;
#else
		template <typename T> using identity_map = std::map<uint32, T>;
		template <typename T> using identity_set_t = std::set<T>;
#endif
		using identity_set = identity_set_t<uint32>;

		// TODO : Use std::unique_ptr for value
		struct MapOrInfo
		{
			bool IsMap = false;
			uint32 Mask = 0U;
			// TODO : use std::variant
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
				std::construct_at(&Map);
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
					for (const auto& element : Map)
					{
						delete element.second;
					}
					std::destroy_at(&Map);
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
		extern _nothrow StaticInitVars& GetStaticInitVars() noexcept;
#endif

		class _RegisterBase
		{
		protected:
			const uint32 m_Register;

		protected:
			_nothrow _RegisterBase(const uint32 _register) noexcept
				: m_Register(_register)
			{}

		public:

			[[nodiscard]]
			_pure
			_nothrow int8 get_offset_gp() const noexcept;
			[[nodiscard]]
			_pure
			_nothrow int16 get_offset_fp() const noexcept;
		};

		template <uint32 offset, uint32 size, typename T>
		class _Register : public _RegisterBase
		{
		protected:
			template<uint32, uint32, typename> friend class _Register;
			using processor_t = T;
			processor_t * __restrict m_Processor = nullptr;
			using _RegisterBase::m_Register;
			[[nodiscard]]
			_pure
			static _forceinline _nothrow uint32 _get_register(const instruction_t instruction) noexcept
			{
				return (instruction >> offset) & Bits(size);
			}
		public:
			_forceinline _nothrow _Register(const instruction_t instruction, processor_t& processor) noexcept :
				_RegisterBase(_get_register(instruction)), 
				m_Processor(&processor) {}

			_forceinline _nothrow _Register(const instruction_t instruction, processor_t* const processor) noexcept :
				_RegisterBase(_get_register(instruction)), 
				m_Processor(processor) {}

			_forceinline _nothrow _Register(const uint32 _register) noexcept :
				_RegisterBase(_register) {}

			template <uint32 _offset, uint32 _size>
			[[nodiscard]]
			_pure
			_forceinline _nothrow bool operator == (const _Register<_offset, _size, T> & __restrict reg) const noexcept
			{
				return get_register() == reg.get_register();
			}

			template <uint32 _offset, uint32 _size>
			[[nodiscard]]
			_pure
			_forceinline _nothrow bool operator != (const _Register<_offset, _size, T> & __restrict reg) const noexcept
			{
				return get_register() != reg.get_register();
			}

			template <uint32 _offset, uint32 _size>
			[[nodiscard]]
			_pure
			_forceinline _nothrow auto operator <=> (const _Register<_offset, _size, T> & __restrict reg) const noexcept
			{
				return get_register() <=> reg.get_register();
			}

			[[nodiscard]]
			_pure
			_forceinline _nothrow uint32 get_register() const noexcept
			{
				xassert(m_Register < 32);

				return m_Register;
			}

			[[nodiscard]]
			_pure
			_forceinline _nothrow uint32 get_index() const noexcept
			{
				xassert(m_Register != 0 && m_Register < 32);

				return m_Register /*- 1*/;
			}

			template <typename format_t>
			[[nodiscard]]
			_pure
			_forceinline _nothrow format_t value() const noexcept
			{
				xassert(m_Processor != nullptr);

				return m_Processor->template get_register<format_t>(m_Register);
			}

			template <typename format_t>
			_forceinline _nothrow format_t set(format_t value) noexcept
			{
				xassert(m_Processor != nullptr);

				xassert(sizeof(typename processor_t::register_type) >= sizeof(format_t));
				m_Processor->template set_register<format_t>(m_Register, value);
				return value;
			}
		};

		struct GPRegisterInfo final
		{
			uint32 register_;
			int8 offset_;
			std::optional<int32> constant_;

			_pure
			_forceinline _nothrow bool is_zero() const noexcept
			{
				return register_ == 0U;
			}

			_pure
			_forceinline _nothrow bool is_constant() const noexcept
			{
				return constant_.has_value();
			}

			_pure
			_forceinline _nothrow const std::optional<int32>& get_constant() const noexcept
			{
				return constant_;
			}

			_pure
			_forceinline _nothrow int8 get_offset() const noexcept
			{
				return offset_;
			}

			_pure
			_forceinline _nothrow uint32 get_index() const noexcept
			{
				xassert(register_ != 0);

				return register_ /*- 1*/;
			}

			_pure
			_forceinline _nothrow uint32 get_register() const noexcept
			{
				xassert(register_ < 32);
				return register_;
			}
		};

		template <uint32 offset = 0, uint32 size = 0>
		class GPRegister : public _Register<offset, size, processor>
		{
			using base = _Register<offset, size, processor>;

		public:
			_forceinline _nothrow GPRegister(const instruction_t instruction, processor& _processor) noexcept :
				base(instruction, _processor) {}

			_forceinline _nothrow GPRegister(const instruction_t instruction, processor* const _processor) noexcept :
				base(instruction, _processor)
			{
				xassert(_processor != nullptr);
			}

			_forceinline _nothrow GPRegister(const uint32 _register) noexcept :
				base(_register)
			{}

			[[nodiscard]]
			_pure
			// ReSharper disable once CppMemberFunctionMayBeStatic
			_forceinline _nothrow bool is_zero() const noexcept
			{
				return _RegisterBase::m_Register == 0U;
			}

			[[nodiscard]]
			_pure
			_forceinline _nothrow bool is_constant() const noexcept
			{
				return is_zero();
			}

			[[nodiscard]]
			_pure
			_forceinline _nothrow std::optional<int32> get_constant() const noexcept
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

			[[nodiscard]]
			_pure
			_forceinline _nothrow int8 get_offset(const bool force = false) const noexcept
			{
				if (!force)
				{
					xassert(!is_zero());
				}

				return _RegisterBase::get_offset_gp();
			}

			[[nodiscard]]
			_pure
			_forceinline _nothrow operator GPRegisterInfo() const noexcept
			{
				return {
					.register_ = base::get_register(),
					.offset_ = get_offset(true),
					.constant_ = get_constant()
				};
			}
		};

	}
}
