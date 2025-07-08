#pragma once

#include <common.hpp>
#include "mips/system.hpp"

namespace mips {
	class system_vemix final : public system {
	private:
		virtual uint32 handle_exception(const CPU_Exception & __restrict ex) override;

		_forceinline
		void set_syscall_result(bool success, uint32 result);
		_forceinline
		uint32 handle_syscall(const CPU_Exception & __restrict ex);
		_noinline _cold
		uint32 handle_unknown_syscall(uint32 code, uint32 address);
		_noinline _cold
		uint32 handle_sys_exception(const CPU_Exception & __restrict ex);
	public:
		system_vemix(const options & __restrict init_options, const elf::binary & __restrict binary);
		virtual ~system_vemix() override = default;

		virtual void clock(uint64 clocks = 0) __restrict override;
	};
}
