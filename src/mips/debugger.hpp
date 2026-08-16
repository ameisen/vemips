#pragma once
// Copyright 2016-2017 Digital Carbide / Michael Kuklinski. All Rights Reserved.

#include <common.hpp>

#include <atomic>
#include <limits>
#include <mutex>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
#define NOMINMAX 1
#	include <WinSock2.h>
#undef NOMINMAX

#include "mips/mips.hpp"


namespace mips
{
	class processor;
	class system;

	class debugger final {
		SOCKET m_ListenSocket = INVALID_SOCKET;
		SOCKET m_ClientSocket = INVALID_SOCKET;
		std::thread m_ServerThread;
		bool m_ack_mode = true;
		uint32 m_active_thread = uint32(-1);
		std::vector<uint32> m_threads { 1 };
		bool m_non_stop = false;

		uint32 m_thread_next_sc = std::numeric_limits<uint32>::max();
		uint32 m_thread_next_g = std::numeric_limits<uint32>::max();

		[[nodiscard]]
		bool handle_packet(const std::vector<char>& __restrict packet, std::vector<char>& __restrict response);

		std::atomic<bool> m_paused{ true };
		std::atomic<uint32> m_threads_to_pause{ 1 };
		std::atomic<bool> m_should_kill{ false };
		std::unordered_map<uint32, uint32> m_step;

		using breakpoint_t = std::tuple<uint32, uptr_guest, uint32>;

		std::vector<breakpoint_t> m_breakpoints;
		std::vector<std::pair<uint32, breakpoint_t>> m_pending_breakpoints;

		std::mutex m_wait_lock;
		std::mutex m_breakpoint_lock;

		system& m_system;

		_nothrow void handle_kill() noexcept;
		_nothrow void handle_stop() noexcept;

	public:
		debugger(uint16 port, system& sys);
		~debugger();

		[[nodiscard]]
		_pure
		_nothrow _forceinline bool should_pause() const noexcept {
			return m_paused;
		}

		[[nodiscard]]
		_pure
		_nothrow _forceinline bool should_kill() const noexcept {
			return m_should_kill;
		}

		void wait();

		[[nodiscard]]
		_pure // technically not pure but acts as though it is
		_nothrow bool should_interrupt_execution() noexcept;
	};
}
