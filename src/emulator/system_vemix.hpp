#pragma once

#include <common.hpp>

#include "mips/system.hpp"


namespace mips
{
	namespace elf
	{
		class binary;
	}

	struct CPU_Exception;
}

namespace vemips
{
	class system_vemix final : public mips::system {
		bool silent_ = false;

	private:
		virtual uint32 handle_exception(const mips::CPU_Exception& __restrict ex) override;

		_forceinline
		_nothrow void set_syscall_result(bool success, uptr_guest result) noexcept;
		_forceinline
		_nothrow void set_syscall_result_vemips(bool success, sptr_guest result0, sptr_guest result1 = 0) noexcept;
		_forceinline
		uint32 handle_syscall(const mips::CPU_Exception& __restrict ex);
		_noinline _cold
		uint32 handle_unknown_syscall(uint32 code, uint32 address);
		_noinline _cold
		uint32 handle_sys_exception(const mips::CPU_Exception& __restrict ex);
	public:
		system_vemix(const options& __restrict init_options, const mips::elf::binary& __restrict binary);
		system_vemix(const options& __restrict init_options, const mips::elf::binary& __restrict binary, bool silent);
		virtual ~system_vemix() override = default;

		virtual void clock(uint64 clocks = 0) __restrict override;
	};
}
