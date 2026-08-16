#pragma once

#include <common.hpp>

#include <bit>
#include <cfloat>
#include <cmath>
#include <limits>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>

#include "instructions_common.hpp"
#include "mips/exception.hpp"
#include "mips/mips_common.hpp"
#include "mips/coprocessor/coprocessor1/coprocessor1.hpp"
#include "mips/processor/processor.hpp"


#if defined(_MSC_VER)
#	define FPU_EXCEPTION_SUPPORT 1
#endif
#ifdef EMSCRIPTEN
#	pragma message("improve rounding support when using emscripten")
#endif

namespace mips::instructions
{

	// Is this value a real value - not infinity or nan?
	template <typename T>
	static _nothrow bool is_real(T val) noexcept
	{
		return !std::isinf(val) && !std::isnan(val);
	}

	// Exception Bits in MIPS format
	enum class ExceptBits : uint32
	{
		None          = 0,
		Inexact       = 1U << 0,
		Underflow     = 1U << 1,
		Overflow      = 1U << 2,
		DivZero       = 1U << 3,
		InvalidOp     = 1U << 4,
		UnsupportedOp = 1U << 5,
		All           = (UnsupportedOp << 1) - 1,
	};

	static constexpr _nothrow ExceptBits operator &(const ExceptBits lhs, const ExceptBits rhs) noexcept
	{
		return static_cast<ExceptBits>(std::to_underlying(lhs) & std::to_underlying(rhs));
	}

	// Convert 'ExceptBits' into bits used by <cfloat> functions
	static constexpr _nothrow uint32 ConvertSignalToPlatform(const ExceptBits bits) noexcept
	{
#if FPU_EXCEPTION_SUPPORT
		if constexpr (
			std::to_underlying(ExceptBits::Inexact) == _EM_INEXACT &&
			std::to_underlying(ExceptBits::Underflow) == _EM_UNDERFLOW &&
			std::to_underlying(ExceptBits::Overflow) == _EM_OVERFLOW &&
			std::to_underlying(ExceptBits::DivZero) == _EM_ZERODIVIDE &&
			std::to_underlying(ExceptBits::InvalidOp) == _EM_INVALID
		)
		{
			return uint32(bits);
		}
		else
		{
			return
				(((bits & ExceptBits::Inexact) != ExceptBits::None) ? _EM_INEXACT : 0) |
				(((bits & ExceptBits::Underflow) != ExceptBits::None) ? _EM_UNDERFLOW : 0) |
				(((bits & ExceptBits::Overflow) != ExceptBits::None) ? _EM_OVERFLOW : 0) |
				(((bits & ExceptBits::DivZero) != ExceptBits::None) ? _EM_ZERODIVIDE : 0) |
				(((bits & ExceptBits::InvalidOp) != ExceptBits::None) ? _EM_INVALID : 0);
		}
#else
		return 0;
#endif
	}

	// Possible denormal states. 'None' means "don't care".
	enum class DenormalState : uint32
	{
#if FPU_EXCEPTION_SUPPORT
		Flush = _DN_FLUSH,
		Save = _DN_SAVE,
#else
		Flush,
		Save,
#endif
		None
	};

	// Possible rounding states. 'None' means "don't care".
	enum class RoundingState : uint32
	{
#if FPU_EXCEPTION_SUPPORT
		ToNearest = _RC_NEAR,
		ToZero = _RC_CHOP,
		Up = _RC_UP,
		Down = _RC_DOWN,
#else
		ToNearest,
		ToZero,
		Up,
		Down,
#endif
		None = 0xFFFFFFFF,
	};

	static constexpr DenormalState DefaultSysDenormal = DenormalState::Flush;
	static constexpr RoundingState DefaultSysRounding = RoundingState::ToNearest;

	// Extracts the rounding state from the Coprocessor1 coprocessor1::FCSR register
	inline _nothrow RoundingState GetRoundingStateFromFCSR(const coprocessor1::FCSR state) noexcept
	{
		switch (state.RoundingMode)
		{
			using enum coprocessor1::RoundingMode;

			case ToNearest:
				return RoundingState::ToNearest;
			case ToZero:
				return RoundingState::ToZero;
			case ToPositive:
				return RoundingState::Up;
			case ToNegative:
				return RoundingState::Down;
			default:
				xunreachable("Unknown Rounding State");
				return RoundingState::None;
		}
	}

	// TODO remove this to simplify
	// Used for passing exeptions between functions to know what was thrown
	struct ExceptionReason
	{
		unsigned InvalidOp : 1;
		unsigned /*Denormal*/ : 1;
		unsigned DivZero : 1;
		unsigned Overflow : 1;
		unsigned Underflow : 1;
		unsigned Precision : 1;
	};

	template <typename T> requires std::is_floating_point_v<T>
	static _forceinline _nothrow T apply_sign(const T value, const bool sign)
	{
		return sign ? -value : value;
	}

	// Gets the default value, as per MIPS specification, when an exception is thrown but not signalled.
	template <typename F>
	static _nothrow F GetDefaultExceptionFloat(const coprocessor1& __restrict coprocessor, const ExceptionReason reason, F value) noexcept
	{
		static_assert(sizeof(reason) <= 8, "Pass `reason` by reference");

		if (reason.InvalidOp)
		{
			return std::numeric_limits<F>::quiet_NaN();
		}
		else if (reason.DivZero)
		{
			return apply_sign(std::numeric_limits<F>::infinity(), value < 0);
		}
		else if (reason.Underflow)
		{
			// TODO : is this right?
			return value;
		}
		else if (reason.Precision)
		{
			// TODO : Not sure how to detect this correctly.
			if (reason.Overflow)
			{
				return value;
			}
			return value;
		}
		else if (reason.Overflow)
		{
			// Depends on rounding mode.
			// O (RN) // Supplies an infinity with the sign of the intermediate result.
			// 1 (RZ) // Supplies the format's largest finite number with the sign of the intermediate result.
			// 2 (RP) // For positive overflow values, supplies positive infinity. For negative overflow values, supplies the format's most negative finite number.
			// 3 (RM) // For positive overflow values, supplies the format's largest finite number. For negative overflow values, supplies minus infinity.
			coprocessor1::FCSR _fcsr = coprocessor.get_FCSR();
			switch (_fcsr.RoundingMode)
			{
				using enum coprocessor1::RoundingMode;

				case ToNearest:
					return apply_sign(std::numeric_limits<F>::infinity(), value < 0);
				case ToZero:
					return apply_sign(std::numeric_limits<F>::max(), value < 0);
				case ToPositive:
					if (value < 0)
					{
						return -std::numeric_limits<F>::max();
					}
					else
					{
						return std::numeric_limits<F>::infinity();
					}
				case ToNegative:
					if (value < 0)
					{
						return -std::numeric_limits<F>::infinity();
					}
					else
					{
						return std::numeric_limits<F>::max();
					}
				default:
					xunreachable("rounding mode does not exist");
			}

			xassert(false);
		}
		return value;
	}

	// Format bits which are specified by MIPS COP1 instructions, in their correct place
	enum class FormatBits : uint32
	{
		Single = 16U << 21U,
		Double = 17U << 21U,
		Word   = 20U << 21U,
		Long   = 21U << 21U,
		None   = 0U
	};

	static constexpr _nothrow FormatBits operator |(const FormatBits a, const FormatBits b) noexcept
	{
		return static_cast<FormatBits>(std::to_underlying(a) | std::to_underlying(b));
	}

	static constexpr _nothrow FormatBits operator &(const FormatBits a, const FormatBits b) noexcept
	{
		return static_cast<FormatBits>(std::to_underlying(a) & std::to_underlying(b));
	}

	// Returns a 'DenormalState' value based upon an instruction's flags.
	template <OpFlags Flags>
	static constexpr const DenormalState GetDenormalStateFromFlags = 
		((Flags & OpFlags::Denorm_Flush) != OpFlags::None) ? DenormalState::Flush :
			((Flags & OpFlags::Denorm_Save) != OpFlags::None) ? DenormalState::Save :
				DenormalState::None;

	inline _nothrow DenormalState GetDenormalStateFromFCSR(const coprocessor1::FCSR state) noexcept
	{
		return state.FlushZero ? DenormalState::Flush : DenormalState::Save;;
	}

	// Returns whether any denormalization flags were set for an instruction.
	template <OpFlags Flags>
	static constexpr const bool HasDenormalBits =
		((Flags & (OpFlags::Denorm_Flush | OpFlags::Denorm_Save | OpFlags::Denorm_None)) != OpFlags::None) ? true : false;

	// Coprocessor 1-specific logic used for instruction table initialization
	struct MaskType final
	{
		const FormatBits Mask;
		const FormatBits Type;
	};

	class _instruction_initializer
	{
		struct instruction_tuple final {
			const instructionexec_t executor;
			const instruction_type type;
		};

		static constexpr _nothrow instruction_tuple get_instruction_type(
			const FormatBits format_bits,
			const instructionexec_t exec_f,
			const instructionexec_t exec_d,
			const instructionexec_t exec_w,
			const instructionexec_t exec_l
		) noexcept {
			switch (format_bits) {
				case FormatBits::Single:
					return { .executor = exec_f, .type = instruction_type::single_fp };
				case FormatBits::Double:
					return { .executor = exec_d, .type = instruction_type::double_fp };
				case FormatBits::Word:
					return { .executor = exec_w, .type = instruction_type::word_fp };
				case FormatBits::Long:
					return { .executor = exec_l, .type = instruction_type::long_fp };
				case FormatBits::None:
					return { .executor = exec_f, .type = instruction_type::void_fp };
				default:
					xunreachable("Invalid Format Bits");
			}
		}

	public:
#if USE_STATIC_INSTRUCTION_SEARCH
		_nothrow
#endif
		_instruction_initializer(
			const char* const __restrict name,
			const uint32 instructionMask,
			const uint32 instructionRef,
			const std::span<const MaskType>&& referenceMasks,
			const instructionexec_t exec_f,
			const instructionexec_t exec_d,
			const instructionexec_t exec_w,
			const instructionexec_t exec_l,
			const OpFlags OpFlags,
			const bool control
		)
#if USE_STATIC_INSTRUCTION_SEARCH
		noexcept
#endif
		{
#if !USE_STATIC_INSTRUCTION_SEARCH
			auto& staticInitVars = GetStaticInitVars();
			staticInitVars.g_InstructionMasks.insert(instructionMask);
			for (const MaskType reference_mask : referenceMasks)
			{
				uint32 referenceMask = uint32(reference_mask.Mask) | instructionRef;

				const auto instruction_data = get_instruction_type(reference_mask.Type, exec_f, exec_d, exec_w, exec_l);

				InstructionInfo Procs{ name, 1, instruction_data.executor, OpFlags, { .control = control }, instruction_data.type };
				FullProcInfo FullProc {
					.InstructionMask = instructionMask,
					.RefMask = referenceMask,
					.ProcInfo = std::move(Procs)
				};
				staticInitVars.g_ProcInfos.push_back(std::move(FullProc));
			}
#endif
		}
	};

	template <uint32 offset = 0, uint32 size = 0>
	class FPRegister : public _Register<offset, size, coprocessor1>
	{
		using parent = _Register<offset, size, coprocessor1>;

	public:
		_nothrow FPRegister(const instruction_t instruction, coprocessor1& _processor) noexcept
			: parent(instruction, _processor) {}

		_nothrow FPRegister(const instruction_t instruction, coprocessor1* const _processor) noexcept
			: parent(instruction, *_processor)
		{
			xassert(_processor != nullptr);
		}

		_nothrow FPRegister(const instruction_t instruction, processor& _processor) noexcept
			: parent(instruction, _processor.get_fpu_coprocessor()) {}

		_nothrow FPRegister(const instruction_t instruction, processor* const _processor) noexcept
			: parent(instruction, _processor ? _processor->get_fpu_coprocessor() : nullptr)
		{
			xassert(_processor != nullptr);
		}

		_nothrow FPRegister(const uint32 _register) noexcept
			: parent(_register) {}

		template <typename format_t>
		_nothrow format_t value_upper() const noexcept
		{
			xassert(parent::m_Processor != nullptr);

			xassert(sizeof(coprocessor1::register_type) / 2 == sizeof(format_t));
			return parent::m_Processor->template get_register_upper<format_t>(parent::m_Register);
		}

		template <typename format_t>
		_nothrow format_t set_upper(format_t value) noexcept
		{
			xassert(parent::m_Processor != nullptr);

			xassert(sizeof(coprocessor1::register_type) / 2 == sizeof(format_t));
			parent::m_Processor->template set_register_upper<format_t>(parent::m_Register, value);
			return value;
		}

		// ReSharper disable once CppMemberFunctionMayBeStatic
		_forceinline _nothrow constexpr bool is_constant() const noexcept
		{
			return false;
		}

		_forceinline _nothrow constexpr std::optional<double> get_constant() const noexcept
		{
			xassert(!is_constant());

			return {};
		}

		_forceinline _nothrow int16 get_offset() const noexcept
		{
			return parent::get_offset_fp();
		}
	};

	template <uint32 offset, uint32 size>
	class FPCRegister : public _Register<offset, size, coprocessor1>
	{
		using parent = _Register<offset, size, coprocessor1>;

	public:
		_nothrow FPCRegister(instruction_t instruction, coprocessor1 & __restrict _processor) noexcept :
			parent(instruction, _processor) {}

		template <typename format_t>
		_nothrow format_t value() const noexcept
		{
			xassert(parent::m_Processor != nullptr);

			static_assert(sizeof(uint32) >= sizeof(format_t));
			static_assert(sizeof(coprocessor1::cr_type) == sizeof(uint32));
			// Strict Aliasing rules apply.
			union
			{
				uint32 valSrc;
				format_t valDst;
			} Caster;
			switch (parent::m_Register)
			{
				case 0:
					Caster.valSrc = static_cast<uint32>(parent::m_Processor->get_FIR()); break;
				// case 5: user write of 1 to FRE
				// case 6: user write of 0 to FRE
				case 31:
					Caster.valSrc = static_cast<uint32>(parent::m_Processor->get_FCSR()); break;
				// case 25: FCCR
				case 26:
					Caster.valSrc = parent::m_Processor->get_FCSR().get_FEXR(); break;
				case 28:
					Caster.valSrc = parent::m_Processor->get_FCSR().get_FENR(); break;;
				default: [[unlikely]]
					CPU_Exception::throw_helper<CPU_Exception::Type::RI>(parent::m_Processor->get_processor().get_program_counter());
			}
			return Caster.valDst;
		}

		template <typename format_t>
		_nothrow format_t set(format_t value) noexcept
		{
			xassert(parent::m_Processor != nullptr);

			static_assert(sizeof(uint32) >= sizeof(format_t));
			static_assert(sizeof(coprocessor1::cr_type) == sizeof(uint32));
			// Strict Aliasing rules apply.
			union
			{
				uint32 valDst;
				format_t valSrc;
			} Caster;
			Caster.valSrc = value;

			//parent::m_Processor->set_needs_clock();

			switch (parent::m_Register)
			{
#if 0
				case 0:
					reinterpret_cast<uint32 &>(parent::m_Processor->get_FIR()) = Caster.valDst; break;
#endif
				// case 5: user write of 1 to FRE
				// case 6: user write of 0 to FRE
				case 31:
					reinterpret_cast<uint32 &>(parent::m_Processor->get_FCSR()) = Caster.valDst; break;
				// case 25: FCCR
				case 26:
					parent::m_Processor->get_FCSR().set_FEXR(Caster.valDst); break;
				case 28:
					parent::m_Processor->get_FCSR().set_FENR(Caster.valDst); break;
				default: [[unlikely]]
					CPU_Exception::throw_helper<CPU_Exception::Type::RI>(parent::m_Processor->get_processor().get_program_counter());
			}
			return value;
		}
	};

	template <bool enable = true>
	class ScopedFloatDenormalState final
	{
#if FPU_EXCEPTION_SUPPORT
		DenormalState m_SetState;
		unsigned m_State;
#endif

	public:
		explicit _nothrow ScopedFloatDenormalState(const DenormalState state) noexcept
#if FPU_EXCEPTION_SUPPORT
		: m_SetState(state)
#endif
		{
#if FPU_EXCEPTION_SUPPORT
			if (enable && state != DefaultSysDenormal)
			{
				m_State = ::_controlfp(0, 0);
				::_controlfp(
					std::to_underlying(state),
					_MCW_DN
				);
			}
#endif
		}

		_nothrow ~ScopedFloatDenormalState() noexcept
		{
#if FPU_EXCEPTION_SUPPORT
			if (enable && m_SetState != DefaultSysDenormal)
			{
				::_controlfp(
					m_State,
					_MCW_DN
				);
			}
#endif
		}
	};

	template <bool enable = true>
	class ScopedFloatRoundingState final
	{
#if FPU_EXCEPTION_SUPPORT
		RoundingState m_SetState;
		unsigned m_State;
#endif

	public:
		explicit _nothrow ScopedFloatRoundingState(const RoundingState state) noexcept
#if FPU_EXCEPTION_SUPPORT
		: m_SetState(state)
#endif
		{
#if FPU_EXCEPTION_SUPPORT
			if (enable && state != DefaultSysRounding)
			{
				m_State = ::_controlfp(0, 0);
				if (state != RoundingState::None)
				{
					::_controlfp(
						std::to_underlying(state),
						_MCW_RC
					);
				}
			}
#endif
		}

		_nothrow ~ScopedFloatRoundingState() noexcept
		{
#if FPU_EXCEPTION_SUPPORT
			if (enable && m_SetState != DefaultSysRounding)
			{
				::_controlfp(
					m_State,
					_MCW_RC
				);
			}
#endif
		}
	};

	template <typename T>
	struct COP1DefaultValueGenerator;

	// TODO strict aliasing
	template <>
	struct COP1DefaultValueGenerator<float> final
	{
		static _nothrow _forceinline float Get(const coprocessor1& __restrict coprocessor, const float result, const ExceptionReason reason) noexcept { return GetDefaultExceptionFloat<float>(coprocessor, reason, result); }
	};
	template <>
	struct COP1DefaultValueGenerator<double> final
	{
		static _nothrow _forceinline double Get(const coprocessor1& __restrict coprocessor, const double result, const ExceptionReason reason) noexcept { return GetDefaultExceptionFloat<double>(coprocessor, reason, result); }
	};

	template <>
	struct COP1DefaultValueGenerator<int32> final
	{
		static _nothrow _forceinline int32 Get(const coprocessor1& __restrict, const int32 result, [[maybe_unused]] const ExceptionReason reason) noexcept { return result; }
	};
	template <>
	struct COP1DefaultValueGenerator<int64> final
	{
		static _nothrow _forceinline int64 Get(const coprocessor1& __restrict, const int64 result, [[maybe_unused]] const ExceptionReason reason) noexcept { return result; }
	};

	template <typename T> struct int_equiv;
	template<> struct int_equiv<float> { using type = int32; };
	template<> struct int_equiv<double> { using type = int64; };

	template <typename T> using int_equiv_t = typename int_equiv<T>::type;

	template <typename T> struct uint_equiv;
	template<> struct uint_equiv<float> { using type = uint32; };
	template<> struct uint_equiv<double> { using type = uint64; };

	template <typename T> using uint_equiv_t = typename uint_equiv<T>::type;

	template <typename T>
	static constexpr _nothrow _forceinline int_equiv_t<T> BitCastToInt(const T value) noexcept
	{
		return std::bit_cast<int_equiv_t<T>>(value);
	}

	template <typename T>
	static _nothrow _forceinline bool is_signalling_nan(T val) noexcept
	{
		constexpr const T snan = std::numeric_limits<T>::signaling_NaN();
		return BitCastToInt(val) == BitCastToInt(snan);
	}

	template <OpFlags Flags>
	static constexpr const RoundingState GetRoundingStateFromFlags =
		std::to_underlying(Flags & OpFlags::Round_ToNearest) ? RoundingState::ToNearest :
			std::to_underlying(Flags & OpFlags::Round_ToZero) ? RoundingState::ToZero :
				std::to_underlying(Flags & OpFlags::Round_Up) ? RoundingState::Up :
					std::to_underlying(Flags & OpFlags::Round_Down) ? RoundingState::Down :
						RoundingState::None;

	template <OpFlags Flags>
	static constexpr const bool HasRoundingBits =
		(Flags & (OpFlags::Round_ToNearest | OpFlags::Round_ToZero | OpFlags::Round_Up | OpFlags::Round_Down | OpFlags::Round_None)) != OpFlags::None;

	// TODO set cause bits

	template <OpFlags Flags>
	static
#if !FPU_EXCEPTION_SUPPORT
	_nothrow
#endif
	void throw_signal(coprocessor1& __restrict coprocessor, const uint32 exStatus)
#if !FPU_EXCEPTION_SUPPORT
	noexcept
#endif
	{
#if FPU_EXCEPTION_SUPPORT
		if constexpr (!HasAnyFlags(Flags, OpFlags::Signals_All))
		{
			return;
		}

		// Set the cause bits
		const auto SetCauseBits = [exStatus, &coprocessor]<OpFlags OpFlag, uint32 StatusFlag, ExceptBits ExceptBit>()
		{
			if (HasFlag(Flags, OpFlag) && (exStatus & StatusFlag))
			{
				coprocessor.set_needs_clock();
				coprocessor.get_FCSR().Flags |= std::to_underlying(ExceptBit);
				coprocessor.get_FCSR().Cause |= std::to_underlying(ExceptBit);
			}
		};
		
		SetCauseBits.template operator()<OpFlags::Signals_Inexact,   _SW_INEXACT,    ExceptBits::Inexact>();
		SetCauseBits.template operator()<OpFlags::Signals_Underflow, _SW_UNDERFLOW,  ExceptBits::Underflow>();
		SetCauseBits.template operator()<OpFlags::Signals_Overflow,  _SW_OVERFLOW,   ExceptBits::Overflow>();
		SetCauseBits.template operator()<OpFlags::Signals_DivZero,   _SW_ZERODIVIDE, ExceptBits::DivZero>();
		SetCauseBits.template operator()<OpFlags::Signals_InvalidOp, _SW_INVALID,    ExceptBits::InvalidOp>();

		if (coprocessor.get_FCSR().Enables) {
			const auto CheckAndThrow = [exStatus, &coprocessor]<OpFlags OpFlag, uint32 StatusFlag, ExceptBits ExceptBit>()
			{
				if (HasFlag(Flags, OpFlag) && (exStatus & StatusFlag))
				{
					if (coprocessor.get_FCSR().Enables & std::to_underlying(ExceptBit)) [[unlikely]]
					{
						CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(coprocessor.get_processor().get_program_counter(), coprocessor.get_FCSR().Cause);
					}
				}
			};

			CheckAndThrow.template operator()<OpFlags::Signals_Inexact,   _SW_INEXACT,    ExceptBits::Inexact>();
			CheckAndThrow.template operator()<OpFlags::Signals_Underflow, _SW_UNDERFLOW,  ExceptBits::Underflow>();
			CheckAndThrow.template operator()<OpFlags::Signals_Overflow,  _SW_OVERFLOW,   ExceptBits::Overflow>();
			CheckAndThrow.template operator()<OpFlags::Signals_DivZero,   _SW_ZERODIVIDE, ExceptBits::DivZero>();
			CheckAndThrow.template operator()<OpFlags::Signals_InvalidOp, _SW_INVALID,    ExceptBits::InvalidOp>();
		}
#endif
	}

	inline void raise_signal(coprocessor1& __restrict coprocessor, const ExceptBits exception)
	{
		if _unlikely(coprocessor.get_FCSR().Enables & std::to_underlying(exception)) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(coprocessor.get_processor().get_program_counter(), std::to_underlying(exception));
		}
	}

	inline void raise_any_signals(coprocessor1& __restrict coprocessor)
	{
		if (!coprocessor.get_FCSR().Cause) {
			return;
		}
		if _unlikely(coprocessor.get_FCSR().Enables) [[unlikely]] {
			for (const auto ex : {
				ExceptBits::Inexact,
				ExceptBits::Underflow,
				ExceptBits::Overflow,
				ExceptBits::DivZero,
				ExceptBits::InvalidOp 
			}) {
				if (
					(coprocessor.get_FCSR().Enables & std::to_underlying(ex)) &&
					(coprocessor.get_FCSR().Cause & std::to_underlying(ex))
				) {
					CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(coprocessor.get_processor().get_program_counter(), std::to_underlying(ex));
				}
			}
		}
		if ((coprocessor.get_FCSR().Cause & uint32(ExceptBits::UnsupportedOp))) [[unlikely]]
		{
			CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(coprocessor.get_processor().get_program_counter(), std::to_underlying(ExceptBits::UnsupportedOp));
		}
	}

	template <typename reg_t, typename format_t, OpFlags Flags>
	static _forceinline
#if !FPU_EXCEPTION_SUPPORT
	_nothrow
#endif
	void _write_result(coprocessor1& __restrict coprocessor, reg_t& __restrict dest, format_t value)
#if !FPU_EXCEPTION_SUPPORT
	noexcept
#endif
	{
#if FPU_EXCEPTION_SUPPORT
		const uint32 exStatus = _statusfp();
		if (!ScriptMode && exStatus & (_SW_INEXACT | _SW_UNDERFLOW | _SW_OVERFLOW | _SW_ZERODIVIDE | _SW_INVALID)) [[unlikely]]
		{
			throw_signal<Flags>(coprocessor, exStatus);

			if constexpr (HasFlag(Flags, OpFlags::NoWriteOnSignal)) {
				return;
			}

			if constexpr (HasFlag(Flags, OpFlags::CustomDefault))
			{
				dest.template set<format_t>(value);
				return;
			}

			const ExceptionReason reason = {
				.InvalidOp = (exStatus & _SW_INVALID) != 0,
				.DivZero = (exStatus & _SW_ZERODIVIDE) != 0,
				.Overflow = (exStatus & _SW_OVERFLOW) != 0,
				.Underflow = (exStatus & _SW_UNDERFLOW) != 0,
				.Precision = (exStatus & _SW_INEXACT) != 0,
			};

			value = COP1DefaultValueGenerator<format_t>::Get(coprocessor, value, reason);
		}
#endif

		dest.template set<format_t>(value);
	}

	template <typename T, bool supported>
	struct format_type { using type = T; };

	// TODO restrict the COP functions that don't support int32/int64 to just void.

	template <typename T>
	struct format_type<T, false> { using type = void; };

	class COP1_Helper final
	{
	public:
		COP1_Helper() = delete;

		template <typename format_t, typename InsT>
		static _forceinline void Execute(const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
		{
			if constexpr (HasFlag(InsT::Flags, OpFlags::ControlInstruction))
			{
				if _unlikely(processor.get_no_cti()) [[unlikely]]
				{
					CPU_Exception::throw_helper<CPU_Exception::Type::RI>(processor.get_program_counter());
				}
			}

			if constexpr (HasFlag(InsT::Flags, OpFlags::ModifiesFPUState))
			{
				coprocessor.set_needs_clock();
			}

			if constexpr (HasFlag(InsT::Flags, OpFlags::ClearsCause))
			{
				coprocessor.get_FCSR().Cause = 0;
			}

#if FPU_EXCEPTION_SUPPORT
			const RoundingState roundingState = HasRoundingBits<InsT::Flags> ?
				GetRoundingStateFromFlags<InsT::Flags> :
				GetRoundingStateFromFCSR(coprocessor.get_FCSR());

			const DenormalState denormalState = HasDenormalBits<InsT::Flags> ?
				GetDenormalStateFromFlags<InsT::Flags> :
				GetDenormalStateFromFCSR(coprocessor.get_FCSR());

			constexpr bool ShouldSetDenormal = !HasDenormalBits<InsT::Flags> ||
				(GetDenormalStateFromFlags<InsT::Flags> != DefaultSysDenormal && GetDenormalStateFromFlags<InsT::Flags> != DenormalState::None);
			constexpr bool ShouldSetRounding = !HasRoundingBits<InsT::Flags> ||
				(GetRoundingStateFromFlags<InsT::Flags> != DefaultSysRounding && GetRoundingStateFromFlags<InsT::Flags> != RoundingState::None);

			ScopedFloatDenormalState<ShouldSetDenormal> _denormalState{ denormalState };
			ScopedFloatRoundingState<ShouldSetRounding> _roundState{ roundingState };

			if constexpr (!ScriptMode && HasAnyFlags(InsT::Flags, OpFlags::Signals_All))
			{
				::_clearfp();
			}
#endif

			InsT::template SubExecute<format_t>(instruction, processor, coprocessor);

			if constexpr (HasFlag(InsT::Flags, OpFlags::WritesGPRegister))
			{
				// Clear zero register
				processor.set_register(0, 0U);
			}
		}
	};

	template <size_t N>
	constexpr _nothrow FormatBits GetFormatBitsFromExtMask(const MaskType(&Masks)[N], size_t idx = 0) noexcept
	{
		return (idx >= N) ? FormatBits::None : (Masks[idx].Type | GetFormatBitsFromExtMask(Masks, idx + 1));
	}
}

// Workaround for Visual C++ not properly handling templated noexcept when the declaration 
// uses a bool for noexcept.
#if !__clang__ && _MSC_VER
#	define COP1_NOEXCEPT(...)
#else
#	define COP1_NOEXCEPT(...) noexcept(__VA_ARGS__)
#endif

#define Cop1InstructionDef(InsInstruction, InsOperFlags, InsOpMask, InsOpRef, ...)                    \
struct COP1_ ## InsInstruction                                                                        \
{                                                                                                     \
	COP1_ ## InsInstruction() = delete;                                                               \
	static_assert(                                                                                    \
		std::is_same_v<decltype(InsOperFlags), OpFlags>,                                              \
		"Operation Flags field must be of type 'OpFlags'"                                             \
	);                                                                                                \
	static constexpr const instruction_t OpMask = InsOpMask;                                          \
	static constexpr const OpFlags Flags = InsOperFlags | OpFlags::COP1;                              \
	static constexpr const MaskType ExtMasks[] = {__VA_ARGS__};                                       \
	static constexpr const FormatBits Formats = GetFormatBitsFromExtMask(ExtMasks);                   \
	static constexpr bool CanThrowWrite =                                                             \
		(Flags & OpFlags::Signals_All) != OpFlags::None;                                              \
	static constexpr bool CanThrowInner = CanThrowWrite ||                                            \
		(Flags & (OpFlags::Throws | OpFlags::ThrowsMisaligned)) != OpFlags::None;                     \
	static constexpr bool CanThrow = CanThrowInner ||                                                 \
		(Flags & OpFlags::ControlInstruction) != OpFlags::None;                                       \
                                                                                                      \
	template <typename format_t>                                                                      \
	static _forceinline void SubExecute(                                                              \
		instruction_t instruction,                                                                    \
		processor& __restrict processor,                                                              \
		coprocessor1& __restrict coprocessor                                                          \
	) COP1_NOEXCEPT(!CanThrowInner);                                                                  \
                                                                                                      \
	template <typename reg_t, typename format_t>                                                      \
	static _forceinline                                                                               \
	void write_result(                                                                                \
		coprocessor1& __restrict coprocessor,                                                         \
		reg_t& dest,                                                                                  \
		const format_t value                                                                          \
	) noexcept(!CanThrowWrite)                                                                        \
	{                                                                                                 \
		_write_result<reg_t, format_t, Flags>(coprocessor, dest, value);                              \
	}                                                                                                 \
};                                                                                                    \
namespace COP1_ ## InsInstruction ## _NS                                                              \
{                                                                                                     \
	template <typename format_t>                                                                      \
	VEMIPS_JIT_ABI _used _flatten _forceinline void VEMIPS_JIT_ABI_INFIX Execute(                     \
		const instruction_t instruction,                                                              \
		processor& __restrict processor                                                               \
	) COP1_NOEXCEPT(!(COP1_ ## InsInstruction::CanThrow))                                             \
	{                                                                                                 \
		_block_forceinline                                                                            \
		COP1_Helper::Execute<format_t, COP1_ ## InsInstruction >(                                     \
			instruction,                                                                              \
			processor,                                                                                \
			static_cast<coprocessor1& __restrict>(*processor.get_coprocessor(1))                      \
		);                                                                                            \
		if constexpr(                                                                                 \
			(COP1_ ## InsInstruction::Flags & OpFlags::AlwaysThrows) != OpFlags::None                 \
		)                                                                                             \
		{                                                                                             \
			xunreachable("Instruction is marked as AlwaysThrows but returned");                       \
		}                                                                                             \
	}                                                                                                 \
	class _StaticInit : public _instruction_initializer                                               \
	{                                                                                                 \
		template <typename T, bool null, bool valid>                                                  \
		struct functor;                                                                               \
                                                                                                      \
		template <typename T, bool null>                                                              \
		struct functor<T, null, true>                                                                 \
		{                                                                                             \
			static constexpr auto _functor = &Execute<T>;                                             \
		};                                                                                            \
                                                                                                      \
		template <typename T>                                                                         \
		struct functor<T, false, false>                                                               \
		{                                                                                             \
			static constexpr auto _functor = &Execute<void>;                                          \
		};                                                                                            \
                                                                                                      \
		template <typename T>                                                                         \
		struct functor<T, true, false>                                                                \
		{                                                                                             \
			static constexpr auto _functor = nullptr;                                                 \
		};                                                                                            \
                                                                                                      \
	public:                                                                                           \
		_nothrow _StaticInit() noexcept :                                                             \
			_instruction_initializer(                                                                 \
				"COP1_" #InsInstruction,                                                              \
				COP1_ ## InsInstruction::OpMask,                                                      \
				InsOpRef,                                                                             \
				COP1_ ## InsInstruction::ExtMasks,                                                    \
				functor<float,                                                                        \
					false,                                                                            \
					uint32(COP1_ ## InsInstruction::Formats & FormatBits::Single) != 0                \
				>::_functor,                                                                          \
				functor<double,                                                                       \
					false,                                                                            \
					uint32(COP1_ ## InsInstruction::Formats & FormatBits::Double) != 0                \
				>::_functor,                                                                          \
				functor<int32,                                                                        \
					uint32(COP1_ ## InsInstruction::Formats) != 0,                                    \
					(COP1_ ## InsInstruction::Formats & FormatBits::Word) == FormatBits::Word         \
				>::_functor,                                                                          \
				functor<int64,                                                                        \
					uint32(COP1_ ## InsInstruction::Formats) != 0,                                    \
					(COP1_ ## InsInstruction::Formats & FormatBits::Long) == FormatBits::Long         \
				>::_functor,                                                                          \
				(COP1_ ## InsInstruction::Flags),                                                     \
				uint32(COP1_ ## InsInstruction::Flags & OpFlags::ControlInstruction) != 0             \
			)                                                                                         \
		{}                                                                                            \
	} static _StaticInitObj;                                                                          \
}                                                                                                     \
template <typename format_t> _forceinline void COP1_ ## InsInstruction::SubExecute
