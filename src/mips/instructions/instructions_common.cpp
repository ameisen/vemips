#include "pch.hpp"
#include "instructions_common.hpp"
#include "mips/processor/processor.hpp"
#include "mips/coprocessor/coprocessor.hpp"


namespace mips::instructions
{
	_nothrow int8 _RegisterBase::get_offset_gp() const noexcept
	{
		//xassert(m_Register != 0);

		static constexpr const int8 offset = processor::recompiler_offsets<>::get<int8>().registers;

		const uint32 index = m_Register/* - 1*/;

		const int8 result = value_assert<int8>(offset + (sizeof(processor::register_type) * index));

		return result;
	}

	_nothrow int16 _RegisterBase::get_offset_fp() const noexcept
	{
		static constexpr const int16 offset = coprocessor1::recompiler_offsets<>::get<int16>().registers;

		const uint32 index = m_Register;

		const int16 result = value_assert<int16>(offset + (sizeof(coprocessor1::register_type) * index));

		return result;
	}
}
