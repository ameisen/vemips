#include "pch.hpp"

#include "system_vemix.hpp"

#include "../../common/inc/bits/syscall.h"

using namespace mips;

namespace {
	enum class map_bits : uint32 {
		shared = 0x01,
		private_map = 0x02,
		type = 0x0f,
		fixed = 0x10,
		anonymous = 0x20,
		no_reserve = 0x4000,
		grows_down = 0x0100,
		deny_write = 0x0800,
		executable = 0x1000,
		locked = 0x2000,
		populate = 0x8000,
		non_block = 0x10000,
		stack = 0x20000,
		huge_tlb = 0x40000,
		file = 0
	};

	enum class protect_bits : uint32 {
		none = 0,
		read = 1,
		write = 2,
		exec = 4,
		grows_down = 0x01000000,
		grows_up = 0x02000000
	};
}

system_vemix::system_vemix(const options & __restrict init_options, const elf::binary & __restrict binary) : system(init_options, binary) {}

void system_vemix::clock(const uint64 clocks) __restrict {
	system::clock(clocks);
}

uint32 system_vemix::handle_exception(const CPU_Exception & __restrict ex) {
	if (ex.m_ExceptionType == CPU_Exception::Type::Sys) {
		// handle system call

		// 4
		// 5
		// 6
		// 7
		// new calls (musl)
		const uint32 code = processor_->get_register<uint32>(2);
		switch (code) {
			case __NR_futex: {
				// do nothing substantial for now, though we do need to actually handle this.
				processor_->set_register<uint32>(2, uint32(-1));
			} break;
			case __NR_sbrk: { // sbrk
				// todo move to system object
				static uint32 system_break = system_break_;

				const uint32 increment = processor_->get_register<uint32>(4);

				if (!increment) {
					processor_->set_register<uint32>(2, system_break);
					break;
				}

				// make sure it just makes sense, first.
				const uint32 end_address = system_break + increment;
				if (end_address < system_break) {
					processor_->set_register<uint32>(2, std::numeric_limits<uint32>::max());
					break;
				}

				// now let's check if we blow our memory constraints.
				if (const uint32 max_data_seg = options_.total_memory - options_.stack_memory; end_address > max_data_seg) {
					processor_->set_register<uint32>(2, std::numeric_limits<uint32>::max());
					break;
				}

				// Now let's just make sure it doesn't go past stack.
				if (const uint32 stack_ptr = processor_->get_register<uint32>(29); end_address > stack_ptr) {
					processor_->set_register<uint32>(2, std::numeric_limits<uint32>::max());
					break;
				}

				// Otherwise, we can allocate away.
				const uint32 ret_value = system_break;

				system_break += increment;
				system_break_ = system_break;
				processor_->set_register<uint32>(7, 0);
				processor_->set_register<uint32>(2, ret_value);
			} break;
			case __NR_mmap2: {
				// what... what do we do with memory mapping?
				/*
				const uint32 addr = processor_->get_register<uint32>(4);
				const uint32 length = processor_->get_register<uint32>(5);
				const uint32 prot = processor_->get_register<uint32>(6);
				const uint32 flags = processor_->get_register<uint32>(7);
				const int fd = processor_->mem_fetch<int>(processor_->get_register<uint32>(29) + 16);
				const int offset = processor_->mem_fetch<int>(processor_->get_register<uint32>(29) + 20);
				*/

				// we ignore 'addr', because the hint is meaningless to us.
				// THe only thing we are going to do is keep track of what has already
				// been mapped, and try to make it so we don't map things that would
				// pass the stack pointer ($29)
				processor_->set_register<uint32>(2, uint32(-1));
			} break;
			case __NR_rt_sigaction: {
				// do nothing substantial for now, though we do need to actually handle this.
				processor_->set_register<uint32>(2, uint32(-1));
			} break;
			case SYS_rt_sigprocmask: {
				// do nothing substantial for now, though we do need to actually handle this.
				processor_->set_register<uint32>(2, uint32(-1));
			} break;
			case __NR_writev: {
				const uint32 fd = processor_->get_register<uint32>(4);
				const uint32 iov = processor_->get_register<uint32>(5);
				const uint32 iovcnt = processor_->get_register<uint32>(6);
				FILE * __restrict fp;
				switch (fd) {
					case 0:
						fp = stdin; break;
					case 1:
						fp = stdout; break;
					case 2:
						fp = stderr; break;
					default:
						fp = nullptr;
				}
				if (fp == nullptr) {
					// TODO set errno
					processor_->set_register<uint32>(2, uint32(-1));
					break;
				}

				__pragma(pack(1)) struct iovec {
					uint32 iov_base;
					uint32 iov_len;
				};

				uint32 iov_ptr = iov;
				for (uint32 i = 0; i < iovcnt; ++i) {
					const iovec _iovec = processor_->mem_fetch<iovec>(iov_ptr);
						
					// TODO this can be done far more quickly.
					for (uint32 t = 0; t < _iovec.iov_len; ++t) {
						const char c = processor_->mem_fetch<char>(_iovec.iov_base + t);
						fwrite(&c, 1, 1, fp);
					}

					iov_ptr += sizeof(iovec);
				}
			} break;
			case __NR_truncate: {
				// ignore, we lack file ops right now.
			} break;
			case __NR_ioctl: {
				/*
				const uint32 fd = processor_->get_register<uint32>(4);
				const uint32 request = processor_->get_register<uint32>(5);
				const uint32 arg1 = processor_->get_register<uint32>(6);
				const uint32 arg2 = processor_->get_register<uint32>(7);
				*/

				//switch (request)
				//{
				//case 0x40087468: // guessing that this is TIOCGWINSZ.
				//{
				//	__pragma(pack(1)) struct winsize
				//	{
				//		unsigned short ws_row;
				//		unsigned short ws_col;
				//		//unsigned short ws_xpixel;
				//		//unsigned short ws_ypixel;
				//	} ws;
				//	ws.ws_row = 25;
				//	ws.ws_col = 80;
				//	//ws.ws_xpixel = 640;
				//	//ws.ws_ypixel = 480;
				//	mem_write<winsize>(arg1, ws);
				//} break;
				//default:
				processor_->set_register<uint32>(2, uint32(-1)); // don't know this ioctl
																				  //break;
																				  //}
				processor_->set_register<uint32>(2, 0);
			}  break;
			case __NR_exit_group:
			case __NR_exit:
				execution_success_ = true;
				execution_complete_ = true;
				if (processor_->get_jit_type() != JitType::None) {
					return 1;
				}
				throw ExecutionCompleteException();
				break;
			case __NR_set_thread_area:
				// m_user_value
				processor_->user_value_ = processor_->get_register<uint32>(4);
				processor_->set_register<uint32>(2, 0);
				break; //do nothing.
			case __NR_set_tid_address: {
				// pretend to do something meaningful.
				/*
				const uint32 ptr = processor_->get_register<uint32>(4);
				*/
				//processor_->mem_write<uint32>(ptr, 0);
				processor_->set_register<uint32>(2, 0);
			}  break;
			case __NR_gettid: {
				processor_->set_register<uint32>(2, 0);
			}  break;
			case __NR_tkill: {

			}  break;
			default:
				fmt::println("** Unknown System Call Code: {} @ 0x{:08X}", code, ex.m_InstructionAddress);
				execution_success_ = false;
				execution_complete_ = true;
				if (processor_->get_jit_type() != JitType::None) {
					return 1;
				}
				throw ExecutionFailException();
		}
	}
	else
	{
		const char * __restrict ex_name = "";
		switch (ex.m_ExceptionType) {
			case CPU_Exception::Type::Interrupt:
				ex_name = "Interrupt"; break;
			case CPU_Exception::Type::Mod:
				ex_name = "Mod"; break;
			case CPU_Exception::Type::TLBL:
				ex_name = "TLBL"; break;
			case CPU_Exception::Type::TLBS:
				ex_name = "TLBS"; break;
			case CPU_Exception::Type::AdEL:
				fmt::println("** Unhandled Address Load CPU Exception: 0x{:08X} @ 0x{:08X}", ex.m_Code, ex.m_InstructionAddress);
				throw ExecutionFailException();
			case CPU_Exception::Type::AdES:
				fmt::println("** Unhandled Address Store CPU Exception: 0x{:08X} @ 0x{:08X}", ex.m_Code, ex.m_InstructionAddress);
				throw ExecutionFailException();
			case CPU_Exception::Type::IBE:
				ex_name = "IBE"; break;
			case CPU_Exception::Type::DBE:
				ex_name = "DBE"; break;
			case CPU_Exception::Type::Sys:
				ex_name = "Sys"; break;
			case CPU_Exception::Type::Bp:
				ex_name = "Bp"; break;
			case CPU_Exception::Type::RI:
				ex_name = "RI"; break;
			case CPU_Exception::Type::CpU:
				ex_name = "CpU"; break;
			case CPU_Exception::Type::Ov:
				ex_name = "Ov"; break;
			case CPU_Exception::Type::Tr:
				ex_name = "TR"; break;
			case CPU_Exception::Type::FPE:
				ex_name = "FPE"; break;
			case CPU_Exception::Type::Impl1:
				ex_name = "Impl1"; break;
			case CPU_Exception::Type::Impl2:
				ex_name = "Impl2"; break;
			case CPU_Exception::Type::C2E:
				ex_name = "C2E"; break;
			case CPU_Exception::Type::TLBRI:
				ex_name = "TLBRI"; break;
			case CPU_Exception::Type::TLBXI:
				ex_name = "TLBXI"; break;
			case CPU_Exception::Type::MDMX:
				ex_name = "MDMX"; break;
			case CPU_Exception::Type::WATCH:
				ex_name = "WATCH"; break;
			case CPU_Exception::Type::MCheck:
				ex_name = "MCheck"; break;
			case CPU_Exception::Type::Thread:
				ex_name = "Thread"; break;
			case CPU_Exception::Type::DPSPDis:
				ex_name = "DPSPDis"; break;
			case CPU_Exception::Type::GE:
				ex_name = "GE"; break;
			case CPU_Exception::Type::Prot:
				ex_name = "Prot"; break;
			case CPU_Exception::Type::CacheErr:
				ex_name = "CacheErr"; break;
		}
		fmt::println("** Unhandled {} CPU Exception: {:X} @ 0x{:08X}", ex_name, ex.m_Code, ex.m_InstructionAddress);
		execution_success_ = false;
		execution_complete_ = true;
		if (processor_->get_jit_type() != JitType::None) {
			return 1;
		}
		throw ExecutionFailException();
	}
	return 0;
}
