#include "pch.hpp"
#include "coprocessor1.hpp"
#include <common.hpp>

#include <bit>
#include <cstdio>
#include <cstring>
#include <fmt/format.h>

#include "mips/coprocessor/coprocessor.hpp"


using namespace mips;

namespace mips
{
	namespace
	{
		template <std::floating_point FloatT>
		using matching_uint = std::conditional_t<
			sizeof(FloatT) <= 4,
			uint32,
			std::conditional_t<
				sizeof(FloatT) <= 8,
				uint64,
				void
			>
		>;
	}
}

_nothrow void coprocessor1::clock() noexcept {
	fir_.clock();
	fcsr_.clock();
}

namespace {
	using enum coprocessor1::RoundingMode;

	static constexpr const coprocessor1::FCSR g_reference_fcsr_ones {
		.RoundingMode = Default, // 0b00
		.Flags = 0,
		.Enables = 0,
		.Cause = 0,
		.NAN2008 = 1,
		.ABS2008 = 1,
		.Impl = 0,
		.FlushZero = 0
	};
	static constexpr const coprocessor1::FCSR g_reference_fcsr_zeros {
		.RoundingMode = ToNegative, // 0b11
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
	xassert(*this == FIR{});
	//*this = FIR{};
}

_nothrow void coprocessor1::FCSR::clock() noexcept {
	cr_type this_u32 = cr_type(*this);
	this_u32 |= cr_type(g_reference_fcsr_ones);
	this_u32 &= cr_type(g_reference_fcsr_zeros);
	*this = std::bit_cast<FCSR>(this_u32);
}

_nothrow coprocessor1::cr_type coprocessor1::FCSR::get_FEXR() const noexcept {
	return FCSR {
		.RoundingMode = RoundingMode::Default,
		.Flags = Flags,
		.Enables = 0,
		.Cause = Cause,
		.NAN2008 = 0,
		.ABS2008 = 0,
		.Impl = 0,
		.FlushZero = 0
	};
}

_nothrow void coprocessor1::FCSR::set_FEXR(cr_type fexr) noexcept {
	const FCSR in_fexr = std::bit_cast<FCSR>(fexr);
	Flags = in_fexr.Flags;
	Cause = in_fexr.Cause;
}

_nothrow coprocessor1::cr_type coprocessor1::FCSR::get_FENR() const noexcept {
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

_nothrow void coprocessor1::FCSR::set_FENR(const cr_type fenr) noexcept {
	const FCSR fenr_value = std::bit_cast<FCSR>(fenr);
	RoundingMode = fenr_value.RoundingMode;
	Enables = fenr_value.Enables;
	FlushZero = fenr_value.FlushZero;
}

_nothrow std::vector<std::string> coprocessor1::compare(const coprocessor& __restrict other) const noexcept
{
	std::vector<std::string> result;

	const auto& __restrict other1 = static_cast<const coprocessor1& __restrict>(other);

	if (std::memcmp(registers_.data(), other1.registers_.data(), registers_.size()) != 0)
	{
		using uint_register = matching_uint<register_type>;

		for (size_t i = 0; i < registers_.size(); ++i) {
			if _unlikely(
				std::bit_cast<uint_register>(registers_[i]) != std::bit_cast<uint_register>(other1.registers_[i])
			) [[unlikely]] {
				fmt::println(stderr,
					"COP1 Register {} Mismatch: {} [{:016X}] != {} [{:016X}]",
					i,
					registers_[i], std::bit_cast<uint64>(registers_[i]),
					other1.registers_[i], std::bit_cast<uint64>(other1.registers_[i])
				);
				result.emplace_back(fmt::format(
					"cop1 register[{}] mismatch: {} [{:016X}] != {} [{:016X}]",
					i,
					registers_[i], std::bit_cast<uint64>(registers_[i]),
					other1.registers_[i], std::bit_cast<uint64>(other1.registers_[i])
				));
			}
		}
	}
	if _unlikely(fir_ != other1.fir_) [[unlikely]] {
		result.emplace_back(fmt::format(
			"cop1 fir mismatch: {:08X} != {:08X}",
			static_cast<cr_type>(fir_),
			static_cast<cr_type>(other1.fir_)
		));
	}
	if _unlikely(fcsr_ != other1.fcsr_) [[unlikely]] {
		result.emplace_back(fmt::format(
			"cop1 fir mismatch: {:08X} != {:08X}",
			static_cast<cr_type>(fcsr_),
			static_cast<cr_type>(other1.fcsr_)
		));
	}

	return result;
}
