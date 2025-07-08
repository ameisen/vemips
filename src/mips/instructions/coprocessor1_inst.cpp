#include "pch.hpp"
#include "coprocessor1_inst.hpp"
#include "mips/processor/processor.hpp"
#include "mips/coprocessor/coprocessor.hpp"

#include <cmath>
#include <cfloat>
#include <limits>
#include <cassert>
#include <cstdlib>

// SSE most certainly does flush operands.
//#pragma message("validate that x86 will honor denormal state in emulator for operands")

using namespace mips;

#include "coprocessor1_support.hpp"

// TODO cause bits OR into flag bits on no exception

namespace mips::instructions
{
	Cop1InstructionDef(
		ABS,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000000101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const format_t result = fabs(fs_val);

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		ADD,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000000000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(fs_val + ft_val);

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		CLASS,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000011011,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();

		union
		{
			struct result_type
			{
				uint32_t signal_nan : 1;
				uint32_t quiet_nan : 1;
				uint32_t neg_infinity : 1;
				uint32_t neg_normal : 1;
				uint32_t neg_subnormal : 1;
				uint32_t neg_zero : 1;
				uint32_t pos_infinity : 1;
				uint32_t pos_normal : 1;
				uint32_t pos_subnormal : 1;
				uint32_t pos_zero : 1;
			} result_mask;
			format_t as_format;
		} result {
			.as_format = {}
		};

		// int_equiv

		constexpr format_t signal_nan = std::numeric_limits<format_t>::signaling_NaN();
		constexpr format_t quiet_nan = std::numeric_limits<format_t>::quiet_NaN();
		result.result_mask.signal_nan = Cast<int_equiv_t<format_t>>(fs_val) == Cast<int_equiv_t<format_t>>(signal_nan);
		result.result_mask.quiet_nan = Cast<int_equiv_t<format_t>>(fs_val) == Cast<int_equiv_t<format_t>>(quiet_nan);

		if (fs_val < 0)
		{
			constexpr format_t zero_value = -format_t{};

			result.result_mask.neg_infinity = isinf(fs_val);
			result.result_mask.neg_normal = isnormal(fs_val);
			result.result_mask.neg_zero = (fs_val == zero_value);  // NOLINT(clang-diagnostic-float-equal)
			result.result_mask.neg_subnormal = !result.result_mask.neg_normal && !result.result_mask.signal_nan && !result.result_mask.quiet_nan && !result.result_mask.neg_infinity;
		}
		else
		{
			constexpr format_t zero_value = {};

			result.result_mask.pos_infinity = isinf(fs_val);
			result.result_mask.pos_normal = isnormal(fs_val);
			result.result_mask.pos_zero = (fs_val == zero_value);  // NOLINT(clang-diagnostic-float-equal)
			result.result_mask.pos_subnormal = !result.result_mask.pos_normal && !result.result_mask.signal_nan && !result.result_mask.quiet_nan && !result.result_mask.pos_infinity;
		}

		return write_result(fd, result.as_format);
	}

	// TODO CVT commands need to handle the fixed-point types as well.

	Cop1InstructionDef(
		CVT_D,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000100001,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double },
		{ FormatBits::Word, FormatBits::Word },
		{ FormatBits::Long, FormatBits::Long }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const double fs_val = double(fs.value<format_t>());

		return write_result(fd, fs_val);
	}

	Cop1InstructionDef(
		CVT_S,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000100000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double },
		{ FormatBits::Word, FormatBits::Word },
		{ FormatBits::Long, FormatBits::Long }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const float fs_val = float(fs.value<format_t>());

		return write_result(fd, fs_val);
	}

	Cop1InstructionDef(
		CVT_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000100000000000000000000100100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// TODO make sure no result is written on exception.
		const format_t fs_value = fs.value<format_t>();
		int32 fs_val = int32(fs_value);

#if FPU_EXCEPTION_SUPPORT
		if (const uint32 exception_status = _statusfp(); exception_status & (_SW_UNDERFLOW | _SW_OVERFLOW | _SW_ZERODIVIDE | _SW_INVALID))
		{
			if (isnan(fs_value))
			{
				fs_val = 0;
			}
			else if (signbit(fs_value))
			{
				fs_val = std::numeric_limits<int32>::lowest();
			}
			else
			{
				fs_val = std::numeric_limits<int32>::max();
			}
		}
#endif

		return write_result(fd, fs_val);
	}

	Cop1InstructionDef(
		CVT_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000100000000000000000000100101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// TODO make sure no result is written on exception.
		const format_t fs_value = fs.value<format_t>();
		int64 fs_val = int64(fs_value);

#if FPU_EXCEPTION_SUPPORT
		if (const uint32 exception_status = _statusfp(); exception_status & (_SW_UNDERFLOW | _SW_OVERFLOW | _SW_ZERODIVIDE | _SW_INVALID))
		{
			if (isnan(fs_value))
			{
				fs_val = 0;
			}
			else if (signbit(fs_value))
			{
				fs_val = std::numeric_limits<int32>::lowest();
			}
			else
			{
				fs_val = std::numeric_limits<int32>::max();
			}
		}
#endif

		return write_result(fd, fs_val);
	}

	Cop1InstructionDef(
		DIV,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_All),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000000011,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(fs_val / ft_val);

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		MADDF,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
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

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		MSUBF,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011001,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
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

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		MAX,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011110,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t ft_val = ft.value<format_t>();
		const format_t fs_val = fs.value<format_t>();

		const bool ft_real = is_real(ft_val);
		const bool fs_real = is_real(fs_val);

		if (!fs_real || !ft_real)
		{
			raise_signal(ExceptBits::InvalidOp);
		}

		format_t result;

		if (!ft_real)
		{
			result = fs_val;
		}
		else if (!fs_real)
		{
			result = ft_val;
		}
		else if (signbit(fs_val) && !signbit(ft_val))
		{
			result = ft_val;
		}
		else if (!signbit(fs_val) && signbit(ft_val))
		{
			result = fs_val;
		}
		else
		{
			result = (ft_val >= fs_val) ? ft_val : fs_val;
		}

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		MIN,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t ft_val = ft.value<format_t>();
		const format_t fs_val = fs.value<format_t>();

		const bool ft_real = is_real(ft_val);
		const bool fs_real = is_real(fs_val);

		if (!fs_real || !ft_real)
		{
			raise_signal(ExceptBits::InvalidOp);
		}

		format_t result;

		if (!ft_real)
		{
			result = fs_val;
		}
		else if (!fs_real)
		{
			result = ft_val;
		}
		else if (signbit(fs_val) && !signbit(ft_val))
		{
			result = fs_val;
		}
		else if (!signbit(fs_val) && signbit(ft_val))
		{
			result = ft_val;
		}
		else
		{
			result = (ft_val < fs_val) ? ft_val : fs_val;
		}

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		MAXA,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011111,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t ft_val = ft.value<format_t>();
		const format_t fs_val = fs.value<format_t>();

		const bool ft_real = is_real(ft_val);
		const bool fs_real = is_real(fs_val);

		if (!fs_real || !ft_real)
		{
			raise_signal(ExceptBits::InvalidOp);
		}

		format_t result;

		if (!ft_real)
		{
			result = fs_val;
		}
		else if (!fs_real)
		{
			result = ft_val;
		}
		else if (fabs(ft_val) > fabs(fs_val))
		{
			result = ft_val;
		}
		else if (fabs(ft_val) < fabs(fs_val))
		{
			result = fs_val;
		}
		else if (signbit(fs_val) && !signbit(ft_val))
		{
			result = ft_val;
		}
		else if (!signbit(fs_val) && signbit(ft_val))
		{
			result = fs_val;
		}
		else
		{
			result = (ft_val >= fs_val) ? ft_val : fs_val;
		}

		xassert(!isinf(result));  // NOLINT(clang-diagnostic-assume)

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		MINA,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000011101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t ft_val = ft.value<format_t>();
		const format_t fs_val = fs.value<format_t>();

		const bool ft_real = is_real(ft_val);
		const bool fs_real = is_real(fs_val);

		if (!fs_real || !ft_real)
		{
			raise_signal(ExceptBits::InvalidOp);
		}

		format_t result;

		if (!ft_real)
		{
			result = fs_val;
		}
		else if (!fs_real)
		{
			result = ft_val;
		}
		else if (fabs(ft_val) < fabs(fs_val))
		{
			result = ft_val;
		}
		else if (fabs(ft_val) > fabs(fs_val))
		{
			result = fs_val;
		}
		else if (signbit(fs_val) && !signbit(ft_val))
		{
			result = fs_val;
		}
		else if (!signbit(fs_val) && signbit(ft_val))
		{
			result = ft_val;
		}
		else
		{
			result = (ft_val < fs_val) ? ft_val : fs_val;
		}

		xassert(!isinf(result));  // NOLINT(clang-diagnostic-assume)

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		MOV,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000000110,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();

		return write_result(fd, fs_val);
	}

	Cop1InstructionDef(
		MUL,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000000010,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(fs_val * ft_val);

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		NEG,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000000111,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const format_t result = -fs_val;

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		RECIP,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_All),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000010101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation does not specify that this must happen in infinite precision, but I see
		// little reason not to.
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(1.0 / fs_val);

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		RINT,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000011010,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const format_t result = rint(fs_val);

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		RSQRT,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_All),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000010110,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation does not specify that this must happen in infinite precision, but I see
		// little reason not to.
		const double fs_val = double(fs.value<format_t>());
		const double sq_root = sqrt(fs_val);
		const format_t result = format_t(1.0 / sq_root);

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		SQRT,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000000100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation does not specify that this must happen in infinite precision, but I see
		// little reason not to.
		const double fs_val = double(fs.value<format_t>());
		const double sq_root = sqrt(fs_val);
		const format_t result = format_t(sq_root);

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		SEL,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000010000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const format_t ft_val = ft.value<format_t>();
		const auto fd_val = fd.value<typename uint_equiv<format_t>::type>();
		const format_t result = (fd_val & 1) ? ft_val : fs_val;

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		SELEQZ,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000010100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const auto ft_val = ft.value<typename uint_equiv<format_t>::type>();
		const format_t result = (ft_val & 1) ? 0 : fs_val;

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		SELNEZ,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000010111,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		const format_t fs_val = fs.value<format_t>();
		const auto ft_val = ft.value<typename uint_equiv<format_t>::type>();
		const format_t result = (ft_val & 1) ? fs_val : 0;

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		SUB,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_InvalidOp | OpFlags::Signals_Inexact | OpFlags::Signals_Overflow | OpFlags::Signals_Underflow),
		0b11111111111000000000000000111111,
		0b01000110000000000000000000000001,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const FPRegister<11, 5> fs{instruction, coprocessor};
		FPRegister<6, 5> fd{instruction, coprocessor};

		// MIPS documentation specifies that this operation occurs in the highest precision possible.
		// That's a double on this platform. We need to cast it from the original type, though, a value cast.
		const double ft_val = double(ft.value<format_t>());
		const double fs_val = double(fs.value<format_t>());
		const format_t result = format_t(fs_val - ft_val);

		return write_result(fd, result);
	}

	Cop1InstructionDef(
		CTC1,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000011111111111,
		0b01000100110000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		const GPRegister<16, 5> rt{instruction, processor};
		FPCRegister<11, 5> fs{instruction, coprocessor};

		const uint32 rt_val = rt.value<uint32>();
		fs.set<uint32>(rt_val);

		raise_any_signals();

		return true;
	}

	Cop1InstructionDef(
		CFC1,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_All | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000011111111111,
		0b01000100010000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		GPRegister<16, 5> rt{instruction, processor};
		const FPCRegister<11, 5> fs{instruction, coprocessor};

		const uint32 fs_val = fs.value<uint32>();
		rt.set<uint32>(fs_val);

		return true;
	}

	Cop1InstructionDef(
		MTC1,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000011111111111,
		0b01000100100000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		const GPRegister<16, 5> rt{instruction, processor};
		FPRegister<11, 5> fs{instruction, coprocessor};

		const uint32 rt_val = rt.value<uint32>();
		fs.set<uint32>(rt_val);

		return true;
	}

	Cop1InstructionDef(
		MFC1,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000011111111111,
		0b01000100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		GPRegister<16, 5> rt{instruction, processor};
		const FPRegister<11, 5> fs{instruction, coprocessor};

		const uint32 fs_val = fs.value<uint32>();
		rt.set<uint32>(fs_val);

		return true;
	}

	Cop1InstructionDef(
		MTHC1,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000011111111111,
		0b01000100111000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		const GPRegister<16, 5> rt{instruction, processor};
		FPRegister<11, 5> fs{instruction, coprocessor};

		const uint32 rt_val = rt.value<uint32>();
		fs.set_upper<uint32>(rt_val);

		return true;
	}

	Cop1InstructionDef(
		MFHC1,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000011111111111,
		0b01000100011000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		GPRegister<16, 5> rt{instruction, processor};
		const FPRegister<11, 5> fs{instruction, coprocessor};

		const uint32 fs_val = fs.value_upper<uint32>();
		rt.set<uint32>(fs_val);

		return true;
	}

	namespace {
		template <typename float_t>
		static bool COP1_CMP_condn_common(const instruction_t instruction, processor& __restrict processor, coprocessor1& __restrict coprocessor)
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
					CPU_Exception::throw_helper(CPU_Exception::Type::RI, processor.get_program_counter());
				}
			}

			if (is_signalling_nan(fs_val) || is_signalling_nan(ft_val) || (throws && (isnan(fs_val) || isnan(ft_val))))
			{
				if (coprocessor.get_FCSR().set_flag(uint32_t(ExceptBits::InvalidOp))) [[unlikely]]
				{
					// TODO update cause?
					CPU_Exception::throw_helper(CPU_Exception::Type::FPE, processor.get_program_counter(), uint32_t(ExceptBits::InvalidOp));
				}
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
				result = result || (isnan(fs_val) || isnan(ft_val));
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

			using result_t = typename uint_equiv<float_t>::type;
			fd.set<result_t>(result ? result_t(-1) : 0);
#endif

			return true;
		}
	}

	Cop1InstructionDef(
		CMP_condn_s,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000100000,
		0b01000110100000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_CMP_condn_common<float>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		CMP_condn_d,
		(OpFlags::None | OpFlags::Denorm_None | OpFlags::Round_None),
		0b11111111111000000000000000100000,
		0b01000110101000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_CMP_condn_common<double>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		BC1EQZ,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::SetNoCTI ),
		0b11111111111000000000000000000000,
		0b01000101001000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
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

		return true;
	}

	Cop1InstructionDef(
		BC1NEZ,
		(OpFlags::ControlInstruction | OpFlags::DelayBranch | OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::SetNoCTI ),
		0b11111111111000000000000000000000,
		0b01000101101000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
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

		return true;
	}

	namespace {
		/* All of the rounding code uses _the same_ logic, so I will just write one procedure. */
		template <typename format_t, typename fixed_t, OpFlags round_state>
		static bool COP1_ROUND_Common(const instruction_t instruction, processor& __restrict, coprocessor1& __restrict coprocessor)
		{
			const FPRegister<11, 5> fs{ instruction, coprocessor };
			FPRegister<6, 5> ft{ instruction, coprocessor };

			fixed_t result;
			const format_t fs_val = fs.value<format_t>();
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
				xassert(false);
			}

#if FPU_EXCEPTION_SUPPORT
			if (const uint32 exception_status = _statusfp(); exception_status & (_SW_UNDERFLOW | _SW_OVERFLOW | _SW_ZERODIVIDE | _SW_INVALID))
			{
				const uint64_t test_val = uint64_t(fabs(fs_val));
				const bool overflow = test_val > uint64_t(std::numeric_limits<fixed_t>::max());

				if (isinf(fs_val) || overflow)
				{
					result = fs_val < 0 ?
						std::numeric_limits<fixed_t>::lowest() :
						std::numeric_limits<fixed_t>::max();
				}

				if (isnan(fs_val))
				{
					result = 0;
				}
			}
#endif

			ft.set<fixed_t>(result);
			return true;
		}
	}

	Cop1InstructionDef(
		ROUND_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001000,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_ROUND_Common<format_t, int64, OpFlags::Round_ToNearest>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		ROUND_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001100,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_ROUND_Common<format_t, int32, OpFlags::Round_ToNearest>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		CEIL_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001010,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_ROUND_Common<format_t, int64, OpFlags::Round_Up>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		CEIL_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001110,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_ROUND_Common<format_t, int32, OpFlags::Round_Up>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		FLOOR_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001011,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_ROUND_Common<format_t, int64, OpFlags::Round_Down>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		FLOOR_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001111,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_ROUND_Common<format_t, int32, OpFlags::Round_Down>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		TRUNC_L,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001001,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_ROUND_Common<format_t, int64, OpFlags::Round_ToZero>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		TRUNC_W,
		(OpFlags::ClearsCause | OpFlags::Throws | OpFlags::Signals_Inexact | OpFlags::Signals_InvalidOp | OpFlags::CustomDefault),
		0b11111111111111110000000000111111,
		0b01000110000000000000000000001101,
		{ FormatBits::Single, FormatBits::Single },
		{ FormatBits::Double, FormatBits::Double }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		return COP1_ROUND_Common<format_t, int32, OpFlags::Round_ToZero>(instruction, processor, coprocessor);
	}

	Cop1InstructionDef(
		LDC1,
		(OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::Load),
		0b11111100000000000000000000000000,
		0b11010100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		FPRegister<16, 5> ft{instruction, coprocessor};
		const GPRegister<21, 5> base{instruction, processor};
		const int32 offset = TinyInt<16>(instruction).sextend<int32_t>();

		const uint32 base_val = base.value<uint32>();

		const uint32 address = base_val + offset;

		const uint64 result = processor.mem_fetch<uint64>(address);

		ft.set<uint64>(result);

		return true;
	}

	Cop1InstructionDef(
		LWC1,
		(OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::Load),
		0b11111100000000000000000000000000,
		0b11000100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		FPRegister<16, 5> ft{instruction, coprocessor};
		const GPRegister<21, 5> base{instruction, processor};
		const int32 offset = TinyInt<16>(instruction).sextend<int32_t>();

		const uint32 base_val = base.value<uint32>();

		const uint32 address = base_val + offset;

		const uint32 result = processor.mem_fetch<uint32>(address);

		ft.set<uint32>(result);

		return true;
	}

	Cop1InstructionDef(
		SDC1,
		(OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::Store),
		0b11111100000000000000000000000000,
		0b11110100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const GPRegister<21, 5> base{instruction, processor};
		const int32 offset = TinyInt<16>(instruction).sextend<int32_t>();

		const uint32 base_val = base.value<uint32>();
		const uint64 ft_val = ft.value<uint64>();

		const uint32 address = base_val + offset;

		processor.mem_write<uint64_t>(address, ft_val);

		return true;
	}

	Cop1InstructionDef(
		SWC1,
		(OpFlags::Denorm_None | OpFlags::Round_None | OpFlags::Store),
		0b11111100000000000000000000000000,
		0b11100100000000000000000000000000,
		{ FormatBits::None, FormatBits::None }
	) (const instruction_t instruction, processor & __restrict processor, coprocessor1 & __restrict coprocessor)
	{
		const FPRegister<16, 5> ft{instruction, coprocessor};
		const GPRegister<21, 5> base{instruction, processor};
		const int32 offset = TinyInt<16>(instruction).sextend<int32_t>();

		const uint32 base_val = base.value<uint32>();
		const uint32 ft_val = ft.value<uint32>();

		const uint32 address = base_val + offset;

		processor.mem_write<uint32_t>(address, ft_val);

		return true;
	}
}
