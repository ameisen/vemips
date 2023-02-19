#pragma once

#include <common.hpp>
#include "mips/system.hpp"

namespace mips {
	class system_vemix final : public system {
	private:
		virtual uint32 handle_exception(const CPU_Exception & __restrict ex) override final;
	public:
		system_vemix(const options & __restrict init_options, const elf::binary & __restrict binary);
		virtual ~system_vemix() override final = default;

		virtual void clock(uint64 clocks = 0) __restrict override final;
	};
}
