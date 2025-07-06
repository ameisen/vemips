#pragma once

#include "common.hpp"

namespace mips {
	class processor;
	class coprocessor {
	protected:
		processor & __restrict m_processor;

	public:
		enum class types : uint8
		{
			_first = 0,

			control = _first,
			floating_point = 1,
			coprocessor_2,
			coprocessor_3,

			_max
		};

		static constexpr const size_t max = std::to_underlying(types::_max);

		static constexpr bool is_valid(const types type)
		{
			// convert from enum as enum range is constrained - out-of-range enum values could be UB
			return is_valid(std::to_underlying(type));
		}

		static constexpr bool is_valid(const std::underlying_type_t<types> index)
		{
			return (
				index >= std::to_underlying(types::_first) &&
				index <= std::to_underlying(types::_max)
			);
		}

	public:
		_nothrow coprocessor(processor & __restrict processor) noexcept : m_processor(processor) {}
		virtual _nothrow ~coprocessor() noexcept = default;

		virtual _nothrow void clock() __restrict noexcept = 0;
	};
}

#include "coprocessor1/coprocessor1.hpp"
