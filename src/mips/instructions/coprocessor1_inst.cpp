#include "pch.hpp"
#include "coprocessor1_inst.hpp"
#include <common.hpp>

#include "mips/processor/processor.hpp"
#include "mips/coprocessor/coprocessor1/coprocessor1.hpp"

#include <bit>
#include <cmath>
#include <cfloat>
#include <limits>

#include "coprocessor1_support.hpp"
#include "mips/exception.hpp"
#include "mips/mips_common.hpp"


// SSE most certainly does flush operands.
//#pragma message("validate that x86 will honor denormal state in emulator for operands")

using namespace mips;

// TODO cause bits OR into flag bits on no exception

namespace mips::instructions
{
	namespace
	{
		template <uint32 Bits, CPU_Exception::Type EException>
		requires(std::has_single_bit(Bits))
		static _forceinline uint32 handle_misaligned_address(
			const processor& __restrict processor,
			const uint32 address
		)
		{
			static constexpr const uint32 mask = mips::make_bitmask<uint32>((Bits - 1U) / 8U);

			const auto misaligned_address_handler = processor.get_misaligned_address_handling();
			if ((address & mask) != 0) [[unlikely]]
			{
				switch (misaligned_address_handler)
				{
					using enum processor::misaligned_address_handling;
					case exception:
						CPU_Exception::throw_helper<EException>(processor.get_program_counter(), address);

					case align:
						return address & ~mask;

					case keep: [[likely]]
						return address;

					default: [[unlikely]]
						xunreachable("Unknown Misaligned Address Handler");
				}
			}

			return address;
		}
	}

	// TODO : make sure any executors here that cannot throw are _nothrow/noexcept

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgcc-compat"

	Cop1InstructionDef(
		ABS,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000000101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const format_t result = std::fabs(fs_val);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		ADD,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000000000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(fs_val + ft_val);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		CLASS,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000011011,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();

		union
		{
			struct result_type
			{
				uint32 signal_nan : 1;
				uint32 quiet_nan : 1;
				uint32 neg_infinity : 1;
				uint32 neg_normal : 1;
				uint32 neg_subnormal : 1;
				uint32 neg_zero : 1;
				uint32 pos_infinity : 1;
				uint32 pos_normal : 1;
				uint32 pos_subnormal : 1;
				uint32 pos_zero : 1;
			} result_mask;
			format_t as_format;
		} result {
			.as_format = {}
		};

		// int_equiv

		constexpr format_t signal_nan = std::numeric_limits<format_t>::signaling_NaN();
		constexpr format_t quiet_nan = std::numeric_limits<format_t>::quiet_NaN();
		result.result_mask.signal_nan = BitCastToInt(fs_val) == BitCastToInt(signal_nan);
		result.result_mask.quiet_nan = BitCastToInt(fs_val) == BitCastToInt(quiet_nan);

		const int classification = std::fpclassify(fs_val);

		if (fs_val < 0)
		{
			result.result_mask.neg_infinity = classification == FP_INFINITE;
			result.result_mask.neg_normal = classification == FP_NORMAL;
			result.result_mask.neg_zero = classification == FP_ZERO;
			result.result_mask.neg_subnormal = classification == FP_SUBNORMAL;
		}
		else
		{
			result.result_mask.pos_infinity = classification == FP_INFINITE;
			result.result_mask.pos_normal = classification == FP_NORMAL;
			result.result_mask.pos_zero = classification == FP_ZERO;
			result.result_mask.pos_subnormal = classification == FP_SUBNORMAL;
		}

		write_result(coprocessor, fd, result.as_format);
	}

	// TODO CVT commands need to handle the fixed-point types as well.
	// TODO : CVT commands are supposed to round according to the FCSR rounding mode

	Cop1InstructionDef(
		CVT_D,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000100001,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double },
		{ FormatBits::Word, FormatBits::Word },
		{ FormatBits::Long, FormatBits::Long }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const double fs_val = double(fs.value<format_t>());

		write_result(coprocessor, fd, fs_val);
	}

	Cop1InstructionDef(
		CVT_S,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Underflow | OpFlags::Signals_Overflow),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000100000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double },
		{ FormatBits::Word, FormatBits::Word },
		{ FormatBits::Long, FormatBits::Long }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const float fs_val = float(fs.value<format_t>());

		write_result(coprocessor, fd, fs_val);
	}

	Cop1InstructionDef(
		CVT_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000100000000000000000000100100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// TODO make sure no result is written on exception.
		const format_t fs_value = fs.value<format_t>();
		int32 fs_val = int32(fs_value);

#if FPU_EXCEPTION_SUPPORT
		if (
			const uint32 exception_status = _statusfp();
			exception_status & (_SW_INVALID | _SW_INEXACT)
		) [[unlikely]]
		{
			if ((exception_status & _SW_INVALID) && coprocessor.get_FCSR().set_flag(std::to_underlying(ExceptBits::InvalidOp))) [[unlikely]]
			{
				// TODO update cause?
				CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(processor.get_program_counter(), std::to_underlying(ExceptBits::InvalidOp));
			}
			if ((exception_status & _SW_INEXACT) && coprocessor.get_FCSR().set_flag(std::to_underlying(ExceptBits::Inexact))) [[unlikely]]
			{
				// TODO update cause?
				CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(processor.get_program_counter(), std::to_underlying(ExceptBits::Inexact));
			}
			if (std::isnan(fs_value))
			{
				fd.set(format_t(0));
			}
			else if (std::signbit(fs_value))
			{
				// Spec says 2^63 - 1, but that makes no sense.
				fd.set(std::numeric_limits<int32>::lowest()); // TODO : -max? Spec is unclear
			}
			else
			{
				// Spec says 2^63 - 1, but that makes no sense.
				fd.set(std::numeric_limits<int32>::max());
			}
		}
		else
#endif
		{
			fd.set(fs_val);
		}
	}

	Cop1InstructionDef(
		CVT_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000100000000000000000000100101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// TODO make sure no result is written on exception.
		const format_t fs_value = fs.value<format_t>();
		int64 fs_val = int64(fs_value);

#if FPU_EXCEPTION_SUPPORT
		if (
			const uint32 exception_status = _statusfp();
			exception_status & (_SW_INVALID | _SW_INEXACT)
		) [[unlikely]]
		{
			if ((exception_status & _SW_INVALID) && coprocessor.get_FCSR().set_flag(std::to_underlying(ExceptBits::InvalidOp))) [[unlikely]]
			{
				// TODO update cause?
				CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(processor.get_program_counter(), std::to_underlying(ExceptBits::InvalidOp));
			}
			if ((exception_status & _SW_INEXACT) && coprocessor.get_FCSR().set_flag(std::to_underlying(ExceptBits::Inexact))) [[unlikely]]
			{
				// TODO update cause?
				CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(processor.get_program_counter(), std::to_underlying(ExceptBits::Inexact));
			}
			if (std::isnan(fs_value))
			{
				fd.set(format_t(0));
			}
			else if (std::signbit(fs_value))
			{
				fd.set(std::numeric_limits<int64>::lowest()); // TODO : -max? Spec is unclear
			}
			else
			{
				fd.set(std::numeric_limits<int64>::max());
			}
		}
		else
#endif
		{
			fd.set(fs_val);
		}
	}

	Cop1InstructionDef(
		DIV,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_All),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000000011,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(fs_val / ft_val);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		MADDF,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const double fd_val = double(fd.value<format_t>());
		const double product = fs_val * ft_val;
		const format_t result = format_t(fd_val + product);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		MSUBF,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011001,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const double fd_val = double(fd.value<format_t>());
		const double product = fs_val * ft_val;
		const format_t result = format_t(fd_val - product);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		MAX,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011110,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t ft_val = ft.value<format_t>();
		const format_t fs_val = fs.value<format_t>();

		const bool ft_real = is_real(ft_val);
		const bool fs_real = is_real(fs_val);

		if (!fs_real || !ft_real) [[unlikely]]
		{
			raise_signal(coprocessor, ExceptBits::InvalidOp);
		}

		format_t result;

		if (!ft_real) [[unlikely]]
		{
			result = fs_val;
		}
		else if (!fs_real) [[unlikely]]
		{
			result = ft_val;
		}
		else
		{
			// MSVC doesn't inline std::signbit for some reason
			const bool fs_sign = std::signbit(fs_val);
			const bool ft_sign = std::signbit(ft_val);

			if (fs_sign && !ft_sign)
			{
				result = ft_val;
			}
			else if (!fs_sign && ft_sign)
			{
				result = fs_val;
			}
			else
			{
				result = (ft_val >= fs_val) ? ft_val : fs_val;
			}
		}

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		MIN,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t ft_val = ft.value<format_t>();
		const format_t fs_val = fs.value<format_t>();

		const bool ft_real = is_real(ft_val);
		const bool fs_real = is_real(fs_val);

		if (!fs_real || !ft_real) [[unlikely]]
		{
			raise_signal(coprocessor, ExceptBits::InvalidOp);
		}

		format_t result;

		if (!ft_real) [[unlikely]]
		{
			result = fs_val;
		}
		else if (!fs_real) [[unlikely]]
		{
			result = ft_val;
		}
		else
		{
			// MSVC doesn't inline std::signbit for some reason
			const bool fs_sign = std::signbit(fs_val);
			const bool ft_sign = std::signbit(ft_val);

			if (fs_sign && !ft_sign)
			{
				result = fs_val;
			}
			else if (!fs_sign && ft_sign)
			{
				result = ft_val;
			}
			else
			{
				result = (ft_val < fs_val) ? ft_val : fs_val;
			}
		}

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		MAXA,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011111,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t ft_val = ft.value<format_t>();
		const format_t fs_val = fs.value<format_t>();

		const bool ft_real = is_real(ft_val);
		const bool fs_real = is_real(fs_val);

		if (!fs_real || !ft_real) [[unlikely]]
		{
			raise_signal(coprocessor, ExceptBits::InvalidOp);
		}

		format_t result;

		if (!ft_real) [[unlikely]]
		{
			result = fs_val;
		}
		else if (!fs_real) [[unlikely]]
		{
			result = ft_val;
		}
		else if (std::fabs(ft_val) > std::fabs(fs_val))
		{
			result = ft_val;
		}
		else if (std::fabs(ft_val) < std::fabs(fs_val))
		{
			result = fs_val;
		}
		else
		{
			// MSVC doesn't inline std::signbit for some reason
			const bool fs_sign = std::signbit(fs_val);
			const bool ft_sign = std::signbit(ft_val);

			if (fs_sign && !ft_sign)
			{
				result = ft_val;
			}
			else if (!fs_sign && ft_sign)
			{
				result = fs_val;
			}
			else
			{
				result = (ft_val >= fs_val) ? ft_val : fs_val;
			}
		}

		xassert(!std::isinf(result));  // NOLINT(clang-diagnostic-assume)

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		MINA,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t ft_val = ft.value<format_t>();
		const format_t fs_val = fs.value<format_t>();

		const bool ft_real = is_real(ft_val);
		const bool fs_real = is_real(fs_val);

		if (!fs_real || !ft_real) [[unlikely]]
		{
			raise_signal(coprocessor, ExceptBits::InvalidOp);
		}

		format_t result;

		if (!ft_real) [[unlikely]]
		{
			result = fs_val;
		}
		else if (!fs_real) [[unlikely]]
		{
			result = ft_val;
		}
		else if (std::fabs(ft_val) < std::fabs(fs_val))
		{
			result = ft_val;
		}
		else if (std::fabs(ft_val) > std::fabs(fs_val))
		{
			result = fs_val;
		}
		else 
		{
			// MSVC doesn't inline std::signbit for some reason
			const bool fs_sign = std::signbit(fs_val);
			const bool ft_sign = std::signbit(ft_val);

			if (fs_sign && !ft_sign)
			{
				result = fs_val;
			}
			else if (!fs_sign && ft_sign)
			{
				result = ft_val;
			}
			else
			{
				result = (ft_val < fs_val) ? ft_val : fs_val;
			}
		}

		xassert(!std::isinf(result));  // NOLINT(clang-diagnostic-assume)

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		MOV,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000000110,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();

		write_result(coprocessor, fd, fs_val);
	}

	Cop1InstructionDef(
		MUL,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000000010,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(fs_val * ft_val);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		NEG,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000000111,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const format_t result = -fs_val;

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		RECIP,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_All),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000010101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation does not specify that this must happen in infinite precision, but I see
		// little reason not to.
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(1.0 / fs_val);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		RINT,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000011010,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const format_t result = std::rint(fs_val);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		RSQRT,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_All),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000010110,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation does not specify that this must happen in infinite precision, but I see
		// little reason not to.
		const double fs_val = double(fs.value<format_t>());
		const double sq_root = std::sqrt(fs_val);
		const format_t result = format_t(1.0 / sq_root);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		SQRT,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000000100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation does not specify that this must happen in infinite precision, but I see
		// little reason not to.
		const double fs_val = double(fs.value<format_t>());
		const double sq_root = std::sqrt(fs_val);
		const format_t result = format_t(sq_root);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		SEL,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000010000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const format_t ft_val = ft.value<format_t>();
		const auto fd_val = fd.value<typename uint_equiv<format_t>::type>();
		const format_t result = (fd_val & 1) ? ft_val : fs_val;

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		SELEQZ,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000010100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const auto ft_val = ft.value<typename uint_equiv<format_t>::type>();
		const format_t result = (ft_val & 1) ? 0 : fs_val;

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		SELNEZ,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000010111,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const auto ft_val = ft.value<typename uint_equiv<format_t>::type>();
		const format_t result = (ft_val & 1) ? fs_val : 0;

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		SUB,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000000001,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(fs_val - ft_val);

		write_result(coprocessor, fd, result);
	}

	Cop1InstructionDef(
		CTC1,
		(OpFlags::None | OpFlags::Throws | OpFlags::Signals_All | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::ReadsGPRegister),
		0b11111111111000000000011111111111,
		0b01000100110000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		const GPRegister<16, 5> rt{instruction, processor};
		FPCRegister<11, 5> fs{instruction, coprocessor};

		const uint32 rt_val = rt.value<uint32>();
		fs.set<uint32>(rt_val);

		raise_any_signals(coprocessor);
	}

	Cop1InstructionDef(
		CFC1,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::WritesGPRegister),
		0b11111111111000000000011111111111,
		0b01000100010000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		GPRegister<16, 5> rt{instruction, processor};
		const FPCRegister<11, 5> fs{instruction, coprocessor};

		const uint32 fs_val = fs.value<uint32>();
		rt.set<uint32>(fs_val);
	}

	Cop1InstructionDef(
		MTC1,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::ReadsGPRegister),
		0b11111111111000000000011111111111,
		0b01000100100000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const GPRegister<16, 5> rt{instruction, processor};
		FPRegister<11, 5> fs{instruction, coprocessor};

		const uint32 rt_val = rt.value<uint32>();
		fs.set<uint32>(rt_val);
	}

	Cop1InstructionDef(
		MFC1,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::WritesGPRegister),
		0b11111111111000000000011111111111,
		0b01000100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		GPRegister<16, 5> rt{instruction, processor};
		const FPRegister<11, 5> fs{instruction, coprocessor};

		const uint32 fs_val = fs.value<uint32>();
		rt.set<uint32>(fs_val);
	}

	Cop1InstructionDef(
		MTHC1,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::ReadsGPRegister),
		0b11111111111000000000011111111111,
		0b01000100111000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const GPRegister<16, 5> rt{instruction, processor};
		FPRegister<11, 5> fs{instruction, coprocessor};

		const uint32 rt_val = rt.value<uint32>();
		fs.set_upper<uint32>(rt_val);
	}

	Cop1InstructionDef(
		MFHC1,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::WritesGPRegister),
		0b11111111111000000000011111111111,
		0b01000100011000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		GPRegister<16, 5> rt{instruction, processor};
		const FPRegister<11, 5> fs{instruction, coprocessor};

		const uint32 fs_val = fs.value_upper<uint32>();
		rt.set<uint32>(fs_val);
	}

	namespace {
		template <typename float_t>
		static _forceinline void COP1_CMP_condn_common(const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
		{
#if !VEMIPS_TABLEGEN
			const FPRegister<16, 5> ft{ instruction, coprocessor };
			const FPRegister<11, 5> fs{ instruction, coprocessor };
			FPRegister<6, 5> fd{ instruction, coprocessor };
			const uint32 condition = instruction & Bits(5);

			const float_t fs_val = fs.value<float_t>();
			const float_t ft_val = ft.value<float_t>();

			const bool is_unordered = condition & 1;
			const bool throws = (condition >> 3) & 1;
			const uint32 relation = ((condition >> 1) & 0b11);
			const uint32 negated = (condition >> 4) & 1;

			if (negated)
			{
				if ((condition & 0b111) == 0 || (condition & 0b100) != 0) [[unlikely]]
				{
					CPU_Exception::throw_helper<CPU_Exception::Type::RI>(processor.get_program_counter());
				}
			}

			if (
				(
					is_signalling_nan(fs_val) ||
					is_signalling_nan(ft_val) ||
					(
						throws &&
						std::isunordered(fs_val, ft_val)
					)
				) &&
				coprocessor.get_FCSR().set_flag(std::to_underlying(ExceptBits::InvalidOp))
			) [[unlikely]]
			{
				// TODO update cause?
				CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(processor.get_program_counter(), std::to_underlying(ExceptBits::InvalidOp));
			}


			// Handle predicates
			bool test_greater_than = false;
			bool test_less_than = (relation & 0b10) != 0;
			bool test_equal = (relation & 0b01) != 0;
			bool test_unordered = is_unordered;

			if (negated) {
				test_greater_than = !test_greater_than;
				test_less_than = !test_less_than;
				test_equal = !test_equal;
				test_unordered = !test_unordered;
			}

			bool result = false;
			if (test_unordered) {
				result = result || std::isunordered(fs_val, ft_val);
			}
			if (test_equal) {
				if (test_greater_than) {
					result = result || (fs_val >= ft_val);
				}
				if (test_less_than) {
					result = result || (fs_val <= ft_val);
				}
				else {
					result = result || (fs_val == ft_val);  // NOLINT(clang-diagnostic-float-equal)
				}
			}
			else {
				if (test_greater_than) {
					result = result || (fs_val > ft_val);
				}
				if (test_less_than) {
					result = result || (fs_val < ft_val);
				}
			}

			using result_t = uint_equiv<float_t>::type;
			fd.set<result_t>(result ? result_t(-1) : 0);
#endif
		}
	}

	Cop1InstructionDef(
		CMP_condn_s,
		(OpFlags::None | OpFlags::Throws | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::ModifiesFPUState), // Signals_All is implicit, as it is thrown directly
		0b11111111111000000000000000100000,
		0b01000110100000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_CMP_condn_common<float>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		CMP_condn_d,
		(OpFlags::None | OpFlags::Throws | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::ModifiesFPUState), // Signals_All is implicit, as it is thrown directly
		0b11111111111000000000000000100000,
		0b01000110101000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_CMP_condn_common<double>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		BC1EQZ,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::SetNoCTI),
		0b11111111111000000000000000000000,
		0b01000101001000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const int32 offset = TinyInt<18>(instruction << 2U).sextend<int32>();

		const uint32 ft_val = ft.value<uint32>();

		if (const bool set = (ft_val & 1) != 0; !set)
		{
			processor.delay_branch(
				processor.get_program_counter() + 4 + offset
			);
		}
		else
		{
			processor.set_flags(processor::flag::no_cti);
		}
	}

	Cop1InstructionDef(
		BC1NEZ,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::SetNoCTI ),
		0b11111111111000000000000000000000,
		0b01000101101000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor) COP1_NOEXCEPT(true)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const int32 offset = TinyInt<18>(instruction << 2U).sextend<int32>();

		if (const uint32 ft_val = ft.value<uint32>(); (ft_val & 1) != 0)
		{
			processor.delay_branch(
				processor.get_program_counter() + 4 + offset
			);
		}
		else
		{
			processor.set_flags(processor::flag::no_cti);
		}
	}

	namespace {
		/* All of the rounding code uses _the same_ logic, so I will just write one procedure. */
		template <typename format_t, typename fixed_t, OpFlags round_state>
		static _forceinline 
		void COP1_ROUND_Common(const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
		{
			const FPRegister<11, 5> fs{ instruction, coprocessor };
			FPRegister<6, 5> ft{ instruction, coprocessor };

			fixed_t result;
			const format_t fs_val = fs.value<format_t>();

			const auto get_exception_result = [&]_forceinline_lambda(()) -> fixed_t
			{
				if (coprocessor.get_FCSR().set_flag(std::to_underlying(ExceptBits::InvalidOp))) [[unlikely]]
				{
					// TODO update cause?
					CPU_Exception::throw_helper<CPU_Exception::Type::FPE>(processor.get_program_counter(), std::to_underlying(ExceptBits::InvalidOp));
				}

				if (std::isnan(fs_val))
				{
					return 0;
				}
				else if (std::signbit(fs_val))
				{
					return std::numeric_limits<fixed_t>::lowest(); // TODO : -max? Spec is unclear
				}
				else
				{
					return std::numeric_limits<fixed_t>::max();
				}
			};

#if !FPU_EXCEPTION_SUPPORT
			if (
				!std::isfinite(fs_val) ||
				[fs_val] // out-of-range check
				{
					// TODO : this is very rough and not exactly accurate :(

					constexpr fixed_t min_fixed = std::numeric_limits<fixed_t>::lowest();
					constexpr fixed_t max_fixed = std::numeric_limits<fixed_t>::max();

					switch (round_state)  // NOLINT(clang-diagnostic-switch-enum)
					{
					case OpFlags::Round_ToNearest:
						return (fs_val <= (format_t(min_fixed) - format_t(0.5))) || (fs_val >= (format_t(max_fixed) + format_t(0.5)));
					case OpFlags::Round_Up:
						return (fs_val <= (format_t(min_fixed) - 1)) || (fs_val > (format_t(max_fixed)));
					case OpFlags::Round_Down:
						return (fs_val < (format_t(min_fixed))) || (fs_val >= (format_t(max_fixed) + 1));
					case OpFlags::Round_ToZero:
						return (fs_val <= (format_t(min_fixed) - 1)) || (fs_val >= (format_t(max_fixed) + 1));
					default:
						xunreachable();
					}
				}()
			)
			{
				result = get_exception_result();
				ft.set<fixed_t>(result);
				return;
			}
#endif

			switch (round_state)  // NOLINT(clang-diagnostic-switch-enum)
			{
			case OpFlags::Round_ToNearest:
				result = fixed_t(std::round(fs_val)); break;
			case OpFlags::Round_Up:
				result = fixed_t(std::ceil(fs_val)); break;
			case OpFlags::Round_Down:
				result = fixed_t(std::floor(fs_val)); break;
			case OpFlags::Round_ToZero:
				result = fixed_t(fs_val); break;
			default:
				xunreachable("Unknown Rounding Mode");
			}

#if FPU_EXCEPTION_SUPPORT
			if (
				const uint32 exception_status = _statusfp();
				exception_status & _SW_INVALID
			) [[unlikely]]
			{
				result = get_exception_result();
			}
#endif

			ft.set<fixed_t>(result);
		}
	}

	Cop1InstructionDef(
		ROUND_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_ROUND_Common<format_t, int64, OpFlags::Round_ToNearest>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		ROUND_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_ROUND_Common<format_t, int32, OpFlags::Round_ToNearest>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		CEIL_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001010,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_ROUND_Common<format_t, int64, OpFlags::Round_Up>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		CEIL_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001110,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_ROUND_Common<format_t, int32, OpFlags::Round_Up>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		FLOOR_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001011,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_ROUND_Common<format_t, int64, OpFlags::Round_Down>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		FLOOR_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001111,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_ROUND_Common<format_t, int32, OpFlags::Round_Down>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		TRUNC_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001001,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_ROUND_Common<format_t, int64, OpFlags::Round_ToZero>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		TRUNC_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		COP1_ROUND_Common<format_t, int32, OpFlags::Round_ToZero>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		LDC1,
		(OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::Load | OpFlags::ReadsGPRegister | OpFlags::ThrowsMisaligned),
		0b11111100000000000000000000000000,
		0b11010100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		FPRegister<16, 5> ft{instruction, coprocessor};
		const GPRegister<21, 5> base{instruction, processor};
		const int32 offset = TinyInt<16>(instruction).sextend<int32_t>();

		const uint32 base_val = base.value<uint32>();

		uint32 address = base_val + offset;

		address = handle_misaligned_address<64, CPU_Exception::Type::AdEL>(processor, address);

		const uint64 result = processor.mem_fetch<uint64>(address);

		ft.set<uint64>(result);
	}

	Cop1InstructionDef(
		LWC1,
		(OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::Load | OpFlags::ReadsGPRegister | OpFlags::ThrowsMisaligned),
		0b11111100000000000000000000000000,
		0b11000100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		FPRegister<16, 5> ft{instruction, coprocessor};
		const GPRegister<21, 5> base{instruction, processor};
		const int32 offset = TinyInt<16>(instruction).sextend<int32_t>();

		const uint32 base_val = base.value<uint32>();

		uint32 address = base_val + offset;

		address = handle_misaligned_address<32, CPU_Exception::Type::AdEL>(processor, address);

		const uint32 result = processor.mem_fetch<uint32>(address);

		ft.set<uint32>(result);
	}

	Cop1InstructionDef(
		SDC1,
		(OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::Store | OpFlags::ReadsGPRegister | OpFlags::ThrowsMisaligned),
		0b11111100000000000000000000000000,
		0b11110100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const GPRegister<21, 5> base{instruction, processor};
		const int32 offset = TinyInt<16>(instruction).sextend<int32_t>();

		const uint32 base_val = base.value<uint32>();
		const uint64 ft_val = ft.value<uint64>();

		uint32 address = base_val + offset;

		address = handle_misaligned_address<64, CPU_Exception::Type::AdES>(processor, address);

		processor.mem_write<uint64_t>(address, ft_val);
	}

	Cop1InstructionDef(
		SWC1,
		(OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::Store | OpFlags::ReadsGPRegister | OpFlags::ThrowsMisaligned),
		0b11111100000000000000000000000000,
		0b11100100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const GPRegister<21, 5> base{instruction, processor};
		const int32 offset = TinyInt<16>(instruction).sextend<int32_t>();

		const uint32 base_val = base.value<uint32>();
		const uint32 ft_val = ft.value<uint32>();

		uint32 address = base_val + offset;

		address = handle_misaligned_address<32, CPU_Exception::Type::AdES>(processor, address);

		processor.mem_write<uint32_t>(address, ft_val);
	}
}

#pragma clang diagnostic pop
