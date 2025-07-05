#pragma once

#include "common.hpp"

namespace mips {
	class processor;
	class coprocessor {
	protected:
		processor & __restrict m_processor;

	public:
		enum class types : uint32
		{
			control = 0,
			floating_point = 1,
			coprocessor_2,
			coprocessor_3,

			max
		};

		static constexpr bool is_valid(const types type)
		{
			if constexpr (std::is_signed_v<std::underlying_type_t<types>>)
			{
				if (std::to_underlying(type) < 0)
				{
					return false;
				}
			}

			return type < types::max;
		}

	public:
		_nothrow coprocessor(processor & __restrict processor) : m_processor(processor) {}
		virtual _nothrow ~coprocessor() = default;

		virtual _nothrow void clock() __restrict = 0;
	};
}

#include "coprocessor1/coprocessor1.hpp"
