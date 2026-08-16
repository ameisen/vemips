#pragma once

#include <common.hpp>

#include <string>
#include <type_traits>
#include <utility>
#include <vector>


namespace mips {
	class processor;

	class coprocessor {
	protected:
		processor& __restrict m_processor;

	private:
		bool needs_clock_ = false;

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

		_func_const _nothrow
		static constexpr bool is_valid(const types type) noexcept
		{
			// convert from enum as enum range is constrained - out-of-range enum values could be UB
			return is_valid(std::to_underlying(type));
		}

		_func_const _nothrow
		static constexpr bool is_valid(const std::underlying_type_t<types> index) noexcept
		{
			return (
				index >= std::to_underlying(types::_first) &&
				index <= std::to_underlying(types::_max)
			);
		}

	public:
		_nothrow coprocessor(processor& __restrict processor) noexcept : m_processor(processor) {}
		virtual _nothrow ~coprocessor() noexcept = default;

		virtual _nothrow void clock() __restrict noexcept = 0;

		_nothrow _pure
		processor& get_processor() const __restrict noexcept
		{
			return m_processor;
		}

		_nothrow
		bool needs_clock() const __restrict noexcept {
			return needs_clock_;
		}

		_nothrow
		void set_needs_clock() __restrict noexcept
		{
			needs_clock_ = true;
		}

		_pure
		virtual _nothrow std::vector<std::string> compare(const coprocessor& __restrict other) const noexcept = 0;
	};
}

#include "coprocessor1/coprocessor1.hpp"
