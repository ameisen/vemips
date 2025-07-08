#include "pch.hpp"
#include "coprocessor1.hpp"

using namespace mips;

_nothrow void coprocessor1::clock() noexcept {
	m_fir.clock();
	m_fcsr.clock();
}

namespace {
	static const coprocessor1::FIR g_reference_fir;
	static const coprocessor1::FCSR g_reference_fcsr_ones = []() -> coprocessor1::FCSR {
		coprocessor1::FCSR value;
		memset(&value, 0x0, sizeof(value));
		value.NAN2008 = 1;
		value.ABS2008 = 1;
		return value;
	}();
	static const coprocessor1::FCSR g_reference_fcsr_zeros = []() -> coprocessor1::FCSR {
		coprocessor1::FCSR value;
		memset(&value, 0xFF, sizeof(value));
		value._z_padding0 = 0;
		value._z_padding1 = 0;
		value._z_padding2 = 0;
		return value;
	}();
}

_nothrow coprocessor1::FIR::FIR() noexcept {
	*(uint32 * __restrict)this = 0;

	Revision = 0b00000001;
	ProcessorID = 0b11111111;
	SinglePrecision = 0b1;
	DoublePrecision = 0b1;
	FixedPointW = 0b0;
	FixedPointL = 0b0;
	Float64 = 0b1;
	Has2008 = 0b1;
	FREP = 0b0;
}

_nothrow void coprocessor1::FIR::clock() noexcept {
	// FIR is supposed to be read-only.
	*this = g_reference_fir;
}

_nothrow coprocessor1::FCSR::FCSR() noexcept {
	*(uint32 * __restrict)this = 0;

	RoundingMode = 0;
	Flags = 0;
	Enables = 0;
	Cause = 0;
	NAN2008 = 1;
	ABS2008 = 1;
	Impl = 0;
	FlushZero = 0;
}

_nothrow void coprocessor1::FCSR::clock() noexcept {
	*(uint32 * __restrict)this |= uint32(g_reference_fcsr_ones);
	*(uint32 * __restrict)this &= uint32(g_reference_fcsr_zeros);
}

_nothrow uint32 coprocessor1::FCSR::get_FEXR() const noexcept {
	uint32 ret = 0;
	((FCSR & __restrict)ret).Flags = Flags;
	((FCSR & __restrict)ret).Cause = Cause;
	return ret;
}

_nothrow void coprocessor1::FCSR::set_FEXR(uint32 fexr) noexcept {
	Flags = ((const FCSR & __restrict)fexr).Flags;
	Cause = ((const FCSR & __restrict)fexr).Cause;
}

_nothrow uint32 coprocessor1::FCSR::get_FENR() const noexcept {
	uint32 ret = 0;
	((FCSR & __restrict)ret).RoundingMode = RoundingMode;
	((FCSR & __restrict)ret).Enables = Enables;
	((FCSR & __restrict)ret).FlushZero = FlushZero;
	return ret;
}

_nothrow void coprocessor1::FCSR::set_FENR(const uint32 fenr) noexcept {
	RoundingMode = ((const FCSR & __restrict)fenr).RoundingMode;
	Enables = ((const FCSR & __restrict)fenr).Enables;
	FlushZero = ((const FCSR & __restrict)fenr).FlushZero;
}
