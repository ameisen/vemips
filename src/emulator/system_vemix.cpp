#include "pch.hpp"

#include "system_vemix.hpp"

#include "..\..\common\inc\bits\syscall.h"

using namespace mips;

system_vemix::system_vemix(const options & __restrict init_options, const elf::binary & __restrict binary) : system(init_options, binary) {}

void system_vemix::clock(uint64 clocks) __restrict {
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
		const uint32 code = m_processor->get_register<uint32>(2);
		switch (code) {
			case __NR_futex: {
				// TODO ?
			}
			case __NR_sbrk: { // sbrk
				// todo move to system object
				static uint32 system_break = m_system_break;

				const uint32 increment = m_processor->get_register<uint32>(4);

				if (!increment) {
					m_processor->set_register<uint32>(2, system_break);
					break;
				}

				// make sure it just makes sense, first.
				const uint32 end_addr = system_break + increment;
				if (end_addr < system_break) {
					m_processor->set_register<uint32>(2, std::numeric_limits<uint32>::max());
					break;
				}

				// now let's check if we blow our memory constraints.
				const uint32 max_data_seg = m_options.total_memory - m_options.stack_memory;
				if (end_addr > max_data_seg) {
					m_processor->set_register<uint32>(2, std::numeric_limits<uint32>::max());
					break;
				}

				// Now let's just make sure it doesn't go past stack.
				const uint32 stack_ptr = m_processor->get_register<uint32>(29);
				if (end_addr > stack_ptr) {
					m_processor->set_register<uint32>(2, std::numeric_limits<uint32>::max());
					break;
				}

				// Otherwise, we can allocate away.
				const uint32 ret_value = system_break;

				system_break += increment;
				m_system_break = system_break;
				m_processor->set_register<uint32>(7, 0);
				m_processor->set_register<uint32>(2, ret_value);
			} break;
			case __NR_mmap2: {
				// ReSharper disable CppDeclaratorNeverUsed
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
				static constexpr const uint32 MAP_SHARED	  = 0x01;
				static constexpr const uint32 MAP_PRIVATE	 = 0x02;
				static constexpr const uint32 MAP_TYPE		 = 0x0f;
				static constexpr const uint32 MAP_FIXED		= 0x10;
				static constexpr const uint32 MAP_ANON		 = 0x20;
				static constexpr const uint32 MAP_ANONYMOUS  = MAP_ANON;
				static constexpr const uint32 MAP_NORESERVE  = 0x4000;
				static constexpr const uint32 MAP_GROWSDOWN  = 0x0100;
				static constexpr const uint32 MAP_DENYWRITE  = 0x0800;
				static constexpr const uint32 MAP_EXECUTABLE = 0x1000;
				static constexpr const uint32 MAP_LOCKED	  = 0x2000;
				static constexpr const uint32 MAP_POPULATE	= 0x8000;
				static constexpr const uint32 MAP_NONBLOCK	= 0x10000;
				static constexpr const uint32 MAP_STACK		= 0x20000;
				static constexpr const uint32 MAP_HUGETLB	 = 0x40000;
				static constexpr const uint32 MAP_FILE		 = 0;

				static constexpr const uint32 PROT_NONE		= 0;
				static constexpr const uint32 PROT_READ		= 1;
				static constexpr const uint32 PROT_WRITE	  = 2;
				static constexpr const uint32 PROT_EXEC		= 4;
				static constexpr const uint32 PROT_GROWSDOWN = 0x01000000;
				static constexpr const uint32 PROT_GROWSUP	= 0x02000000;
#pragma clang diagnostic pop
				// ReSharper restore CppDeclaratorNeverUsed

				// what... what do we do with memory mapping?
				/*
				const uint32 addr = m_processor->get_register<uint32>(4);
				const uint32 length = m_processor->get_register<uint32>(5);
				const uint32 prot = m_processor->get_register<uint32>(6);
				const uint32 flags = m_processor->get_register<uint32>(7);
				const int fd = m_processor->mem_fetch<int>(m_processor->get_register<uint32>(29) + 16);
				const int offset = m_processor->mem_fetch<int>(m_processor->get_register<uint32>(29) + 20);
				*/

				// we ignore 'addr', because the hint is meaningless to us.
				// THe only thing we are going to do is keep track of what has already
				// been mapped, and try to make it so we don't map things that would
				// pass the stack pointer ($29)
				m_processor->set_register<uint32>(2, uint32(-1));
			} break;
			case __NR_rt_sigaction: {
				// do nothing substantial for now, though we do need to actually handle this.
				m_processor->set_register<uint32>(2, uint32(-1));
			} break;
			case SYS_rt_sigprocmask: {
				// do nothing substantial for now, though we do need to actually handle this.
				m_processor->set_register<uint32>(2, uint32(-1));
			} break;
			case __NR_writev: {
				const uint32 fd = m_processor->get_register<uint32>(4);
				const uint32 iov = m_processor->get_register<uint32>(5);
				const uint32 iovcnt = m_processor->get_register<uint32>(6);
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
					m_processor->set_register<uint32>(2, uint32(-1));
					break;
				}

				__pragma(pack(1)) struct iovec {
					uint32 iov_base;
					uint32 iov_len;
				};

				uint32 iov_ptr = iov;
				for (uint32 i = 0; i < iovcnt; ++i) {
					const iovec _iovec = m_processor->mem_fetch<iovec>(iov_ptr);
						
					// TODO this can be done far more quickly.
					for (uint32 t = 0; t < _iovec.iov_len; ++t) {
						const char c = m_processor->mem_fetch<char>(_iovec.iov_base + t);
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
				const uint32 fd = m_processor->get_register<uint32>(4);
				const uint32 request = m_processor->get_register<uint32>(5);
				const uint32 arg1 = m_processor->get_register<uint32>(6);
				const uint32 arg2 = m_processor->get_register<uint32>(7);
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
				m_processor->set_register<uint32>(2, uint32(-1)); // don't know this ioctl
																				  //break;
																				  //}
				m_processor->set_register<uint32>(2, 0);
			}  break;
			case __NR_exit_group:
			case __NR_exit:
				m_execution_success = true;
				m_execution_complete = true;
				if (m_processor->get_jit_type() != JitType::None) {
					return 1;
				}
				throw ExecutionCompleteException();
				break;
			case __NR_set_thread_area:
				// m_user_value
				m_processor->m_user_value = m_processor->get_register<uint32>(4);
				m_processor->set_register<uint32>(2, 0);
				break; //do nothing.
			case __NR_set_tid_address: {
				// pretend to do something meaningful.
				/*
				const uint32 ptr = m_processor->get_register<uint32>(4);
				*/
				//m_processor->mem_write<uint32>(ptr, 0);
				m_processor->set_register<uint32>(2, 0);
			}  break;
			case __NR_gettid: {
				m_processor->set_register<uint32>(2, 0);
			}  break;
			case __NR_tkill: {

			}  break;
			default:
				printf("** Unknown System Call Code: %u @ 0x%08X\n", code, ex.m_InstructionAddress);
				m_execution_success = false;
				m_execution_complete = true;
				if (m_processor->get_jit_type() != JitType::None) {
					return 1;
				}
				throw ExecutionFailException();
		}
	}
	else
	{
		const char * __restrict exName = "";
		switch (ex.m_ExceptionType) {
			case CPU_Exception::Type::Interrupt:
				exName = "Interrupt"; break;
			case CPU_Exception::Type::Mod:
				exName = "Mod"; break;
			case CPU_Exception::Type::TLBL:
				exName = "TLBL"; break;
			case CPU_Exception::Type::TLBS:
				exName = "TLBS"; break;
			case CPU_Exception::Type::AdEL:
				printf("** Unhandled Address Load CPU Exception: 0x%08X @ 0x%08X\n", ex.m_Code, ex.m_InstructionAddress); throw ExecutionFailException();
			case CPU_Exception::Type::AdES:
				printf("** Unhandled Address Store CPU Exception: 0x%08X @ 0x%08X\n", ex.m_Code, ex.m_InstructionAddress); throw ExecutionFailException();
			case CPU_Exception::Type::IBE:
				exName = "IBE"; break;
			case CPU_Exception::Type::DBE:
				exName = "DBE"; break;
			case CPU_Exception::Type::Sys:
				exName = "Sys"; break;
			case CPU_Exception::Type::Bp:
				exName = "Bp"; break;
			case CPU_Exception::Type::RI:
				exName = "RI"; break;
			case CPU_Exception::Type::CpU:
				exName = "CpU"; break;
			case CPU_Exception::Type::Ov:
				exName = "Ov"; break;
			case CPU_Exception::Type::Tr:
				exName = "TR"; break;
			case CPU_Exception::Type::FPE:
				exName = "FPE"; break;
			case CPU_Exception::Type::Impl1:
				exName = "Impl1"; break;
			case CPU_Exception::Type::Impl2:
				exName = "Impl2"; break;
			case CPU_Exception::Type::C2E:
				exName = "C2E"; break;
			case CPU_Exception::Type::TLBRI:
				exName = "TLBRI"; break;
			case CPU_Exception::Type::TLBXI:
				exName = "TLBXI"; break;
			case CPU_Exception::Type::MDMX:
				exName = "MDMX"; break;
			case CPU_Exception::Type::WATCH:
				exName = "WATCH"; break;
			case CPU_Exception::Type::MCheck:
				exName = "MCheck"; break;
			case CPU_Exception::Type::Thread:
				exName = "Thread"; break;
			case CPU_Exception::Type::DPSPDis:
				exName = "DPSPDis"; break;
			case CPU_Exception::Type::GE:
				exName = "GE"; break;
			case CPU_Exception::Type::Prot:
				exName = "Prot"; break;
			case CPU_Exception::Type::CacheErr:
				exName = "CacheErr"; break;
		}
		printf("** Unhandled %s CPU Exception: %X @ 0x%08X\n", exName, ex.m_Code, ex.m_InstructionAddress);
		m_execution_success = false;
		m_execution_complete = true;
		if (m_processor->get_jit_type() != JitType::None) {
			return 1;
		}
		throw ExecutionFailException();
	}
	return 0;
}
