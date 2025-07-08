#pragma once

#include "pch.hpp"

#include "processor/processor_support.hpp"

#if defined(_MSC_VER)
#  define FPU_EXCEPTION_SUPPORT 1
#endif
#ifdef EMSCRIPTEN
#	pragma message("improve rounding support when using emscripten")
#endif

namespace mips::instructions
{

	// Is this value a real value - not infinity or nan?
	template <typename T>
	static bool is_real(T val)
	{
		return !isinf(val) && !isnan(val);
	}

	// Exception Bits in MIPS format
	enum class ExceptBits : uint32_t
	{
		Inexact = 0b1,
		Underflow = 0b10,
		Overflow = 0b100,
		DivZero = 0b1000,
		InvalidOp = 0b10000,
		UnsupportedOp = 0b100000,
		All = 0b111111,
	};

	// Convert 'ExceptBits' into bits used by <cfloat> functions
	constexpr uint32 ConvertSignalToPlatform(ExceptBits bits)
	{
#if FPU_EXCEPTION_SUPPORT
		 if constexpr (
			 uint32(ExceptBits::Inexact) == _EM_INEXACT &&
			 uint32(ExceptBits::Underflow) == _EM_UNDERFLOW &&
			 uint32(ExceptBits::Overflow) == _EM_OVERFLOW &&
			 uint32(ExceptBits::DivZero) == _EM_ZERODIVIDE &&
			 uint32(ExceptBits::InvalidOp) == _EM_INVALID
			)
			 return uint32(bits);
		 else
			return
					(((uint32(bits) & uint32(ExceptBits::Inexact)) != 0) ? _EM_INEXACT : 0) |
					(((uint32(bits) & uint32(ExceptBits::Underflow)) != 0) ? _EM_UNDERFLOW : 0) |
					(((uint32(bits) & uint32(ExceptBits::Overflow)) != 0) ? _EM_OVERFLOW : 0) |
					(((uint32(bits) & uint32(ExceptBits::DivZero)) != 0) ? _EM_ZERODIVIDE : 0) |
					(((uint32(bits) & uint32(ExceptBits::InvalidOp)) != 0) ? _EM_INVALID : 0);
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
	inline RoundingState GetRoundingStateFromFCSR(coprocessor1::coprocessor1::FCSR state)
	{
		switch (state.RoundingMode)
		{
			case 0:
				return RoundingState::ToNearest;
			case 1:
				return RoundingState::ToZero;
			case 2:
				return RoundingState::Up;
			case 3:
				return RoundingState::Down;
			default:
				xassert(false);
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

	// Gets the default value, as per MIPS specification, when an exception is thrown but not signalled.
	template <typename F>
	F GetDefaultExceptionFloat(const ExceptionReason & __restrict reason, F value)
	{
		if (reason.InvalidOp)
		{
			return std::numeric_limits<F>::quiet_NaN();
		}
		else if (reason.DivZero)
		{
			if (value < 0)
			{
				return -std::numeric_limits<F>::infinity();
			}
			else
			{
				return std::numeric_limits<F>::infinity();
			}
		}
		else if (reason.Underflow)
		{
			// is this right?
			return value;
		}
		else if (reason.Precision)
		{
			// Not sure how to detect this correctly.
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
			coprocessor1::FCSR _fcsr = (get_current_coprocessor<coprocessor1>())->get_FCSR();
			switch (_fcsr.RoundingMode)
			{
				case 0:
					if (value < 0)
					{
						return -std::numeric_limits<F>::infinity();
					}
					else
					{
						return std::numeric_limits<F>::infinity();
					}
				case 1:
					if (value < 0)
					{
						return -std::numeric_limits<F>::max();
					}
					else
					{
						return std::numeric_limits<F>::max();
					}
				case 2:
					if (value < 0)
					{
						return -std::numeric_limits<F>::max();
					}
					else
					{
						return std::numeric_limits<F>::infinity();
					}
				case 3:
					if (value < 0)
					{
						return -std::numeric_limits<F>::infinity();
					}
					else
					{
						return std::numeric_limits<F>::max();
					}
			}

			xassert(false);
		}
		return value;
	}

	// Format bits which are specified by MIPS COP1 instructions, in their correct place
	enum class FormatBits : uint32
	{
		Single = uint32(16) << 21U,
		Double = uint32(17) << 21U,
		Word = uint32(20) << 21U,
		Long = uint32(21) << 21U,
		None = uint32(0)
	};

	constexpr FormatBits operator | (FormatBits a, FormatBits b)
	{
		return FormatBits(uint32(a) | uint32(b));
	}

	constexpr FormatBits operator & (FormatBits a, FormatBits b)
	{
		return FormatBits(uint32(a) & uint32(b));
	}

	// Returns a 'DenormalState' value based upon an instruction's flags.
	template <OpFlags Flags>
	inline constexpr DenormalState GetDenormalStateFromFlags()
	{
		return
			(uint32(Flags & OpFlags::Denorm_Flush) != 0) ? DenormalState::Flush :
			(uint32(Flags & OpFlags::Denorm_Save) != 0) ? DenormalState::Save :
			DenormalState::None;
	}

	inline DenormalState GetDenormalStateFromFCSR(coprocessor1::FCSR state)
	{
		return state.FlushZero ? DenormalState::Flush : DenormalState::Save;;
	}

	// Returns whether any denormalization flags were set for an instruction.
	template <OpFlags Flags>
	constexpr bool HasDenormalBits()
	{
		return uint32(Flags & (OpFlags::Denorm_Flush | OpFlags::Denorm_Save | OpFlags::Denorm_None)) ? true : false;
	}

	// Coprocessor 1-specific logic used for instruction table initialization
	struct MaskType
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

		static constexpr instruction_tuple get_instruction_type(
			FormatBits format_bits,
			instructionexec_t exec_f, instructionexec_t exec_d, instructionexec_t exec_w, instructionexec_t exec_l
		) {
			switch (format_bits) {
				case FormatBits::Single:
					return { exec_f, instruction_type::single_fp };
				case FormatBits::Double:
					return { exec_d, instruction_type::double_fp };
				case FormatBits::Word:
					return { exec_w, instruction_type::word_fp };
				case FormatBits::Long:
					return { exec_l, instruction_type::long_fp };
				case FormatBits::None:
					return { exec_f, instruction_type::void_fp };
				default:
					xassert(false);
			}
		}

	public:
		_instruction_initializer(
			const char *name,
			uint32 instructionMask,
			uint32 instructionRef,
			std::span<const MaskType>&& referenceMasks,
			instructionexec_t exec_f, instructionexec_t exec_d, instructionexec_t exec_w, instructionexec_t exec_l,
			uint32 OpFlags,
			bool control
		)
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
		FPRegister(const instruction_t instruction, coprocessor1& _processor)
			: parent(instruction, _processor) {}

		FPRegister(const instruction_t instruction, coprocessor1* const _processor)
			: parent(instruction, *_processor)
		{
			xassert(_processor != nullptr);
		}

		FPRegister(const instruction_t instruction, processor& _processor)
			: parent(instruction, _processor.get_fpu_coprocessor()) {}

		FPRegister(const instruction_t instruction, processor* const _processor)
			: parent(instruction, _processor ? _processor->get_fpu_coprocessor() : nullptr)
		{
			xassert(_processor != nullptr);
		}

		FPRegister(const uint32 _register)
			: parent(_register) {}

		template <typename format_t>
		format_t value_upper() const
		{
			xassert(parent::m_Processor != nullptr);

			xassert(sizeof(coprocessor1::register_type) / 2 == sizeof(format_t));
			return parent::m_Processor->template get_register_upper<format_t>(parent::m_Register);
		}

		template <typename format_t>
		format_t set_upper(format_t value)
		{
			xassert(parent::m_Processor != nullptr);

			xassert(sizeof(coprocessor1::register_type) / 2 == sizeof(format_t));
			parent::m_Processor->template set_register_upper<format_t>(parent::m_Register, value);
			return value;
		}

		_forceinline constexpr bool is_constant() const
		{
			return false;
		}

		_forceinline constexpr std::optional<double> get_constant() const
		{
			xassert(!is_constant());

			return {};
		}

		_forceinline int16 get_offset() const
		{
			return _RegisterBase::get_offset_fp();
		}
	};

	template <uint32 offset, uint32 size>
	class FPCRegister : public _Register<offset, size, coprocessor1>
	{
		using parent = _Register<offset, size, coprocessor1>;

	public:
		FPCRegister(instruction_t instruction, coprocessor1 & __restrict _processor) :
			parent(instruction, _processor) {}

		template <typename format_t>
		format_t value() const
		{
			xassert(parent::m_Processor != nullptr);

			xassert(sizeof(uint32) >= sizeof(format_t));
			// Strict Aliasing rules apply.
			union
			{
				uint32 valSrc;
				format_t valDst;
			} Caster;
			switch (parent::m_Register)
			{
				case 0:
					Caster.valSrc = (uint32)parent::m_Processor->get_FIR(); break;
				case 31:
					Caster.valSrc = (uint32)parent::m_Processor->get_FCSR(); break;
				case 26:
					Caster.valSrc = parent::m_Processor->get_FCSR().get_FEXR(); break;
				case 28:
					Caster.valSrc = parent::m_Processor->get_FCSR().get_FENR(); break;;
				default: [[unlikely]]
					CPU_Exception::throw_helper(CPU_Exception::Type::RI, get_current_processor()->get_program_counter());
			}
			return Caster.valDst;
		}

		template <typename format_t>
		format_t set(format_t value)
		{
			xassert(parent::m_Processor != nullptr);

			xassert(sizeof(uint32) >= sizeof(format_t));
			// Strict Aliasing rules apply.
			union
			{
				uint32 valDst;
				format_t valSrc;
			} Caster;
			Caster.valSrc = value;
			switch (parent::m_Register)
			{
				case 0:
					(uint32 &)parent::m_Processor->get_FIR() = Caster.valDst; break;
				case 31:
					(uint32 &)parent::m_Processor->get_FCSR() = Caster.valDst; break;
				case 26:
					parent::m_Processor->get_FCSR().set_FEXR(Caster.valDst); break;
				case 28:
					parent::m_Processor->get_FCSR().set_FENR(Caster.valDst); break;
				default: [[unlikely]]
					CPU_Exception::throw_helper(CPU_Exception::Type::RI, get_current_processor()->get_program_counter());
			}
			return value;
		}
	};

	template <bool enable = true>
	class ScopedFloatDenormalState final
	{
		DenormalState m_SetState;
		unsigned m_State;

	public:
		ScopedFloatDenormalState(DenormalState state) : m_SetState(state)
		{
#if FPU_EXCEPTION_SUPPORT
			if (enable && state != DefaultSysDenormal)
			{
				m_State = _controlfp(0, 0);
				_controlfp(
					(unsigned)state,
					_MCW_DN
				);
			}
#endif
		}

		~ScopedFloatDenormalState()
		{
#if FPU_EXCEPTION_SUPPORT
			if (enable && m_SetState != DefaultSysDenormal)
			{
				_controlfp(
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
		RoundingState m_SetState;
		unsigned m_State;

	public:
		ScopedFloatRoundingState(RoundingState state) : m_SetState(state)
		{
#if FPU_EXCEPTION_SUPPORT
			if (enable && state != DefaultSysRounding)
			{
				m_State = _controlfp(0, 0);
				if (state != RoundingState::None)
				{
					_controlfp(
						(unsigned)state,
						_MCW_RC
					);
				}
			}
#endif
		}

		~ScopedFloatRoundingState()
		{
#if FPU_EXCEPTION_SUPPORT
			if (enable && m_SetState != DefaultSysRounding)
			{
				_controlfp(
					m_State,
					_MCW_RC
				);
			}
#endif
		}
	};

	template <typename D, typename S>
	D Cast(S value)
	{
		static_assert(sizeof(D) >= sizeof(S), "Cast Size Mismatch");
		union
		{
			S Source;
			D Dest;
		};
		Source = value;
		return Dest;
	}

	template <typename T>
	struct COP1DefaultValueGenerator;

	// TODO strict aliasing
	template <>
	struct COP1DefaultValueGenerator<float>
	{
		static float Get(coprocessor1 & __restrict, float result, ExceptionReason reason) { return GetDefaultExceptionFloat<float>(reason, result); }
	};
	template <>
	struct COP1DefaultValueGenerator<double>
	{
		static double Get(coprocessor1 & __restrict, double result, ExceptionReason reason) { return GetDefaultExceptionFloat<double>(reason, result); }
	};

	template <>
	struct COP1DefaultValueGenerator<int32>
	{
		static int32 Get(coprocessor1 & __restrict, int32 result, ExceptionReason reason) { return result; }
	};
	template <>
	struct COP1DefaultValueGenerator<int64>
	{
		static int64 Get(coprocessor1 & __restrict, int64 result, ExceptionReason reason) { return result; }
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
	static bool is_signalling_nan(T val)
	{
		const T snan = std::numeric_limits<T>::signaling_NaN();
		const typename uint_equiv<T>::type snan_int = *(const typename uint_equiv<T>::type *)&snan;
		return *(typename uint_equiv<T>::type *)&val == snan_int;
	}

	template <OpFlags Flags>
	inline constexpr RoundingState GetRoundingStateFromFlags()
	{
		return
			uint32(Flags & OpFlags::Round_ToNearest) ? RoundingState::ToNearest :
			uint32(Flags & OpFlags::Round_ToZero) ? RoundingState::ToZero :
			uint32(Flags & OpFlags::Round_Up) ? RoundingState::Up :
			uint32(Flags & OpFlags::Round_Down) ? RoundingState::Down :
			RoundingState::None;
	}

	template <OpFlags Flags>
	constexpr bool HasRoundingBits()
	{
		return uint32_t(Flags & (OpFlags::Round_ToNearest | OpFlags::Round_ToZero | OpFlags::Round_Up | OpFlags::Round_Down | OpFlags::Round_None)) != 0;
	}

	// TODO set cause bits

	template <OpFlags Flags>
	void throw_signal(uint32 exStatus)
	{
#if FPU_EXCEPTION_SUPPORT
		if (uint32(Flags & OpFlags::Signals_All))
		{
			auto fpu = (coprocessor1 * __restrict)get_current_coprocessor();

			// Set the cause bits
			if (exStatus & _SW_INEXACT && uint32(Flags & OpFlags::Signals_Inexact))
			{
				fpu->get_FCSR().Flags |= uint32(ExceptBits::Inexact);
				fpu->get_FCSR().Cause |= uint32(ExceptBits::Inexact);
			}
			if (exStatus & _SW_UNDERFLOW && uint32(Flags & OpFlags::Signals_Underflow))
			{
				fpu->get_FCSR().Flags |= uint32(ExceptBits::Underflow);
				fpu->get_FCSR().Cause |= uint32(ExceptBits::Underflow);
			}
			if (exStatus & _SW_OVERFLOW && uint32(Flags & OpFlags::Signals_Overflow))
			{
				fpu->get_FCSR().Flags |= uint32(ExceptBits::Overflow);
				fpu->get_FCSR().Cause |= uint32(ExceptBits::Overflow);
			}
			if (exStatus & _SW_ZERODIVIDE && uint32(Flags & OpFlags::Signals_DivZero))
			{
				fpu->get_FCSR().Flags |= uint32(ExceptBits::DivZero);
				fpu->get_FCSR().Cause |= uint32(ExceptBits::DivZero);
			}
			if (exStatus & _SW_INVALID && uint32(Flags & OpFlags::Signals_InvalidOp))
			{
				fpu->get_FCSR().Flags |= uint32(ExceptBits::InvalidOp);
				fpu->get_FCSR().Cause |= uint32(ExceptBits::InvalidOp);
			}

			if (fpu->get_FCSR().Enables) {
				if (exStatus & _SW_INEXACT && uint32(Flags & OpFlags::Signals_Inexact))
				{
					if (fpu->get_FCSR().Enables & uint32_t(ExceptBits::Inexact)) [[unlikely]]
					{
						CPU_Exception::throw_helper(CPU_Exception::Type::FPE, get_current_processor()->get_program_counter(), uint32_t(fpu->get_FCSR().Cause));
					}
				}
				if (exStatus & _SW_UNDERFLOW && uint32(Flags & OpFlags::Signals_Underflow))
				{
					if (fpu->get_FCSR().Enables & uint32_t(ExceptBits::Underflow)) [[unlikely]]
					{
						CPU_Exception::throw_helper(CPU_Exception::Type::FPE, get_current_processor()->get_program_counter(), uint32_t(fpu->get_FCSR().Cause));
					}
				}
				if (exStatus & _SW_OVERFLOW && uint32(Flags & OpFlags::Signals_Overflow))
				{
					if (fpu->get_FCSR().Enables & uint32_t(ExceptBits::Overflow)) [[unlikely]]
					{
						CPU_Exception::throw_helper(CPU_Exception::Type::FPE, get_current_processor()->get_program_counter(), uint32_t(fpu->get_FCSR().Cause));
					}
				}
				if (exStatus & _SW_ZERODIVIDE && uint32(Flags & OpFlags::Signals_DivZero))
				{
					if (fpu->get_FCSR().Enables & uint32_t(ExceptBits::DivZero)) [[unlikely]]
					{
						CPU_Exception::throw_helper(CPU_Exception::Type::FPE, get_current_processor()->get_program_counter(), uint32_t(fpu->get_FCSR().Cause));
					}
				}
				if (exStatus & _SW_INVALID && uint32(Flags & OpFlags::Signals_InvalidOp)) [[unlikely]]
				{
					if (fpu->get_FCSR().Enables & uint32_t(ExceptBits::InvalidOp))
					{
						CPU_Exception::throw_helper(CPU_Exception::Type::FPE, get_current_processor()->get_program_counter(), uint32_t(fpu->get_FCSR().Cause));
					}
				}
			}
		}
#endif
	}

	inline void raise_signal(ExceptBits exception)
	{
		if _unlikely((get_current_coprocessor<coprocessor1>())->get_FCSR().Enables & uint32_t(exception)) [[unlikely]]
		{
			CPU_Exception::throw_helper(CPU_Exception::Type::FPE, get_current_processor()->get_program_counter(), uint32_t(exception));
		}
	}

	inline void raise_any_signals()
	{
			const auto fpu = (coprocessor1 * __restrict)get_current_coprocessor();
			if (!fpu->get_FCSR().Cause) {
				return;
			}
			if _unlikely(fpu->get_FCSR().Enables) [[unlikely]] {
				for (auto ex : { ExceptBits::Inexact, ExceptBits::Underflow, ExceptBits::Overflow, ExceptBits::DivZero, ExceptBits::InvalidOp }) {
					if ((fpu->get_FCSR().Enables & uint32_t(ex)) && (fpu->get_FCSR().Cause & uint32_t(ex))) {
						CPU_Exception::throw_helper(CPU_Exception::Type::FPE, get_current_processor()->get_program_counter(), uint32_t(ex));
					}
				}
			}
			if ((fpu->get_FCSR().Cause & uint32_t(ExceptBits::UnsupportedOp))) [[unlikely]]
			{
				CPU_Exception::throw_helper(CPU_Exception::Type::FPE, get_current_processor()->get_program_counter(), uint32_t(ExceptBits::UnsupportedOp));
			}
	}

	template <typename reg_t, typename format_t, OpFlags Flags>
	static _forceinline bool _write_result(reg_t & __restrict dest, format_t value)
	{
#if FPU_EXCEPTION_SUPPORT
		const uint32 exStatus = _statusfp();
		if (!ScriptMode && exStatus & (_SW_INEXACT | _SW_UNDERFLOW | _SW_OVERFLOW | _SW_ZERODIVIDE | _SW_INVALID))
		{
			throw_signal<Flags>(exStatus);

			if (uint32(Flags & OpFlags::NoWriteOnSignal)) {
				return true;
			}

			if (uint32(Flags & OpFlags::CustomDefault))
			{
				dest.template set<format_t>(value);
				return false;
			}

			const ExceptionReason reason = {
				.InvalidOp = (exStatus & _SW_INVALID) != 0,
				.DivZero = (exStatus & _SW_ZERODIVIDE) != 0,
				.Overflow = (exStatus & _SW_OVERFLOW) != 0,
				.Underflow = (exStatus & _SW_UNDERFLOW) != 0,
				.Precision = (exStatus & _SW_INEXACT) != 0,
			};

			value = COP1DefaultValueGenerator<format_t>::Get(*(raw_ptr<coprocessor1>(get_current_coprocessor())), value, reason);

			dest.template set<format_t>(value);
			return true;
		}
		else
#endif
		{
			dest.template set<format_t>(value);
		}
		return true;
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
		static void Execute(instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
		{
			try
			{
				set_current_coprocessor(processor.get_coprocessor(1));

				if constexpr (uint32(InsT::Flags & OpFlags::ControlInstruction))
				{
					if _unlikely(processor.get_no_cti()) [[unlikely]]
					{
						CPU_Exception::throw_helper(CPU_Exception::Type::RI, processor.get_program_counter());
					}
				}

				if constexpr (uint32(InsT::Flags & OpFlags::ClearsCause))
				{
					coprocessor.get_FCSR().Cause = 0;
				}

				RoundingState roundingState = HasRoundingBits<InsT::Flags>() ?
					GetRoundingStateFromFlags<InsT::Flags>() :
					GetRoundingStateFromFCSR(coprocessor.get_FCSR());

				DenormalState denormalState = HasDenormalBits<InsT::Flags>() ?
					GetDenormalStateFromFlags<InsT::Flags>() :
					GetDenormalStateFromFCSR(coprocessor.get_FCSR());

				constexpr bool ShouldSetDenormal = !HasDenormalBits<InsT::Flags>() ||
					(GetDenormalStateFromFlags<InsT::Flags>() != DefaultSysDenormal && GetDenormalStateFromFlags<InsT::Flags>() != DenormalState::None);
				constexpr bool ShouldSetRounding = !HasRoundingBits<InsT::Flags>() ||
					(GetRoundingStateFromFlags<InsT::Flags>() != DefaultSysRounding && GetRoundingStateFromFlags<InsT::Flags>() != RoundingState::None);

#if FPU_EXCEPTION_SUPPORT
				ScopedFloatDenormalState<ShouldSetDenormal> _denormalState{ denormalState };
				ScopedFloatRoundingState<ShouldSetRounding> _roundState{ roundingState };

				if (!ScriptMode && uint32(InsT::Flags & OpFlags::Signals_All))
				{
					_clearfp();
				}
#endif
				InsT::template SubExecute<format_t>(instruction, processor, coprocessor);
			}
			catch (const CPU_Exception &ex)
			{
				if (processor.get_jit_type() == JitType::Jit) [[likely]]
				{
					processor.set_trapped_exception(ex);
				}
				else
				{
					throw;
				}
			}
		}
	};

	template <size_t N>
	constexpr FormatBits GetFormatBitsFromExtMask(const MaskType(&Masks)[N], size_t idx = 0)
	{
		return (idx >= N) ? FormatBits::None : (Masks[idx].Type | GetFormatBitsFromExtMask(Masks, idx + 1));
	}
}

#define Cop1InstructionDef(InsInstruction, InsOperFlags, InsOpMask, InsOpRef, ...)																													\
struct COP1_ ## InsInstruction																																																							\
{																																																																						\
	COP1_ ## InsInstruction() = delete;																																																				\
	static_assert(std::is_same<decltype(InsOperFlags), OpFlags>::value, "Operation Flags field must be of type 'OpFlags'");										\
	static constexpr const uint32 OpMask = InsOpMask;																																																\
	static constexpr const OpFlags Flags = InsOperFlags | OpFlags::COP1;																																						\
	static constexpr const MaskType ExtMasks[] = {__VA_ARGS__};																																											\
	static constexpr const FormatBits Formats = GetFormatBitsFromExtMask(ExtMasks);																																	\
																																																																						\
	template <typename format_t>																																				  																		\
	static _forceinline bool SubExecute(instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor);	\
																																																																						\
	template <typename reg_t, typename format_t>																																															\
	static _forceinline bool write_result(reg_t &dest, format_t value)																																				\
	{																																													  																							\
		return _write_result<reg_t, format_t, Flags>(dest, value);																																							\
	}																																													  																							\
};																																																																					\
namespace COP1_ ## InsInstruction ## _NS																																																		\
{																																														  																							\
	template <typename format_t>																																																							\
	_used uint64 Execute(instruction_t instruction, processor & __restrict processor)																													\
	{																																													  																							\
		COP1_Helper::Execute<format_t, COP1_ ## InsInstruction >(																																								\
			instruction, processor, *(coprocessor1 * __restrict)processor.get_coprocessor(1)																											\
		);																																																																			\
		return 0;																																																																\
	}																																													  																							\
	class _StaticInit : public _instruction_initializer																																												\
	{																																													  																							\
		template <typename T, bool null, bool valid>																																														\
		struct functor;																																																													\
																																																																						\
		template <typename T, bool null>																																																				\
		struct functor<T, null, true>																																																						\
		{																																												  																							\
			static constexpr auto _functor = &Execute<T>;																																													\
		};																																																																			\
																																																																						\
		template <typename T>																																																										\
		struct functor<T, false, false>																																																					\
		{																																												  																							\
			static constexpr auto _functor = &Execute<void>;																																											\
		};																																																																			\
																																																																						\
		template <typename T>																																																										\
		struct functor<T, true, false>																																																					\
		{																																												  																							\
			static constexpr auto _functor = nullptr;																																															\
		};																																																																			\
		\
	public:																																																																		\
		_StaticInit() :																																																													\
			_instruction_initializer(																																																							\
				"COP1_" #InsInstruction,																																																						\
				COP1_ ## InsInstruction::OpMask,																																																		\
				InsOpRef,																																																														\
				COP1_ ## InsInstruction::ExtMasks, \
				functor<float, false, uint32(COP1_ ## InsInstruction::Formats & FormatBits::Single) != 0>::_functor,																\
				functor<double, false, uint32(COP1_ ## InsInstruction::Formats & FormatBits::Double) != 0>::_functor,																\
				functor<int32,																																																											\
					uint32(COP1_ ## InsInstruction::Formats) != 0,																																										\
					(COP1_ ## InsInstruction::Formats & FormatBits::Word) == FormatBits::Word>::_functor,																							\
				functor<int64,																																																											\
					uint32(COP1_ ## InsInstruction::Formats) != 0,																																										\
					(COP1_ ## InsInstruction::Formats & FormatBits::Long) == FormatBits::Long>::_functor,																							\
				uint32(COP1_ ## InsInstruction::Flags),																																															\
				uint32(COP1_ ## InsInstruction::Flags & OpFlags::ControlInstruction) != 0																														\
			)																																																																			\
		{}																																																																			\
	} static _StaticInitObj;																																																									\
}																																																																						\
template <typename format_t> _forceinline bool COP1_ ## InsInstruction::SubExecute
