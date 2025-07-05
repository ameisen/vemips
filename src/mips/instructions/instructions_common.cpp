#include "pch.hpp"
#include "instructions_common.hpp"
#include "mips/processor/processor.hpp"
#include "mips/coprocessor/coprocessor.hpp"



namespace mips::instructions
{
	int8 _RegisterBase::get_offset_gp() const
	{
		//xassert(m_Register != 0);

		static constexpr const intptr offset_raw = processor::get_address_offset_static(processor::offset_type::registers);
		static constexpr const int8 offset = value_assert<int8>(offset_raw - 128);

		const uint32 index = m_Register/* - 1*/;

		const int8 result = value_assert<int8>(offset + (sizeof(uint32) * index));

		return result;
	}

	int16 _RegisterBase::get_offset_fp() const
	{
		static constexpr const intptr offset_raw = coprocessor1::get_address_offset_static(coprocessor1::offset_type::registers);
		static constexpr const int16 offset = value_assert<int16>(offset_raw - 128);

		const uint32 index = m_Register;

		const int16 result = value_assert<int16>(offset + (sizeof(double) * index));

		return result;
	}
}
