#pragma once
// Copyright 2016-2017 Digital Carbide / Michael Kuklinski. All Rights Reserved.

#include <common.hpp>
#include "mips/mips.hpp"
#include "elf/elf.hpp"
#include "mips/mmu.hpp"
#include "mips/platform/platform.hpp"

#include <thread>
#include <mutex>
#include <atomic>

#include <WinSock2.h>

namespace mips
{
   class system;
   class processor;
   class debugger
   {
      SOCKET m_ListenSocket = INVALID_SOCKET;
      SOCKET m_ClientSocket = INVALID_SOCKET;
      std::thread m_ServerThread;
      bool m_ack_mode = true;
      uint32 m_active_thread = uint32(-1);
      std::vector<uint32> m_threads = { 1 };
      bool m_non_stop = false;

      uint32 m_thread_next_sc = std::numeric_limits<uint32>::max();
      uint32 m_thread_next_g = std::numeric_limits<uint32>::max();

      bool handle_packet(const std::vector<char> &packet, std::vector<char> &response);

      std::atomic<bool> m_paused{ true };
      std::atomic<uint32> m_threads_to_pause{ 1 };
      std::atomic<bool> m_should_kill{ false };
      std::unordered_map<uint32, uint32> m_step;

      using breakpoint_t = std::tuple<uint32, uint32, uint32>;

      std::vector<breakpoint_t> m_breakpoints;
      std::vector<std::pair<uint32, breakpoint_t>> m_pending_breakpoints;

      std::mutex m_wait_lock;
      std::mutex m_breakpoint_lock;

      system &m_system;

      void handle_kill();
      void handle_stop();

   public:
      debugger(uint16 port, system &sys);
      ~debugger();

      bool should_pause() const
      {
         return m_paused;
      }

      bool should_kill() const
      {
         return m_should_kill;
      }

      void wait();

      bool should_interrupt_execution();
   };
}
