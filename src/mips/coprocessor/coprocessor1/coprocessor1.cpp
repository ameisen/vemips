#include "pch.hpp"
#include "coprocessor1.hpp"

using namespace mips;

_nothrow void coprocessor1::clock() noexcept {
	m_fir.clock();
	m_fcsr.clock();
}

namespace {
	static constexpr const coprocessor1::FCSR g_reference_fcsr_ones {
		.RoundingMode = 0,
		.Flags = 0,
		.Enables = 0,
		.Cause = 0,
		.NAN2008 = 1,
		.ABS2008 = 1,
		.Impl = 0,
		.FlushZero = 0
	};
	static constexpr const coprocessor1::FCSR g_reference_fcsr_zeros {
		.RoundingMode = 0b11,
		.Flags = 0b11111,
		.Enables = 0b11111,
		.Cause = 0b111111,
		.NAN2008 = 1,
		.ABS2008 = 1,
		.Impl = 0b11,
		.FlushZero = 1
	};
}

_nothrow void coprocessor1::FIR::clock() noexcept {
	// FIR is supposed to be read-only.
	*this = FIR{};
}

_nothrow void coprocessor1::FCSR::clock() noexcept {
	uint32 this_u32 = uint32(*this);
	this_u32 |= uint32(g_reference_fcsr_ones);
	this_u32 &= uint32(g_reference_fcsr_zeros);
	*this = std::bit_cast<FCSR>(this_u32);
}

_nothrow uint32 coprocessor1::FCSR::get_FEXR() const noexcept {
	return FCSR {
		.RoundingMode = 0,
		.Flags = Flags,
		.Enables = 0,
		.Cause = Cause,
		.NAN2008 = 0,
		.ABS2008 = 0,
		.Impl = 0,
		.FlushZero = 0
	};
}

_nothrow void coprocessor1::FCSR::set_FEXR(uint32 fexr) noexcept {
	const FCSR in_fexr = std::bit_cast<FCSR>(fexr);
	Flags = in_fexr.Flags;
	Cause = in_fexr.Cause;
}

_nothrow uint32 coprocessor1::FCSR::get_FENR() const noexcept {
	return FCSR {
		.RoundingMode = RoundingMode,
		.Flags = 0,
		.Enables = Enables,
		.Cause = 0,
		.NAN2008 = 0,
		.ABS2008 = 0,
		.Impl = 0,
		.FlushZero = FlushZero
	};
}

_nothrow void coprocessor1::FCSR::set_FENR(const uint32 fenr) noexcept {
	const FCSR in_fenr = std::bit_cast<FCSR>(fenr);
	RoundingMode = in_fenr.RoundingMode;
	Enables = in_fenr.Enables;
	FlushZero = in_fenr.FlushZero;
}
