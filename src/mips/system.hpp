#pragma once

#include <common.hpp>
#include "mips/mips.hpp"
#include "elf/elf.hpp"
#include "mips/mmu.hpp"
#include "mips/platform/platform.hpp"
#include "mips/debugger.hpp"

namespace mips
{
   class debugger;
   class processor;
   class system
   {
   protected:
      friend class processor;

      processor * __restrict m_processor = nullptr;
      memory_source * __restrict m_memory_source = nullptr;
      std::vector<char> m_memory;
      platform::host_mmu *m_host_mmu = nullptr;
      uint32 m_system_break = 0;
      debugger *m_debugger = nullptr;

      // Termination
      bool m_execution_complete = false;
      bool m_execution_success = false;

   public:
      struct options
      {
         uint32 total_memory = 0;
         uint32 stack_memory = 0; // 0 = no stack reservation, unified memory space
         mips::JitType jit_type = JitType::None;
         bool read_only_exec = false;
         bool record_instruction_stats = false;
         bool ticked = false;
         bool instruction_cache = false;
         mmu mmu_type = mmu::emulated;
         bool debug = false;
         uint16 debug_port = 0;
         bool debug_owned = false;

         void validate() const __restrict;
      };
   protected:
      const options m_options;

      void initialize(const elf::binary & __restrict binary) __restrict;
   public:
      system(const options &init_options, const elf::binary & __restrict binary);
      virtual ~system();

      virtual void clock(uint64 clocks) __restrict;
      
      uint64 get_instruction_count() const __restrict;

      const std::unordered_map<const char *, size_t> & get_stats_map() const __restrict;

      size_t get_jit_max_instruction_size() const __restrict;

      virtual uint32 handle_exception(const CPU_Exception &ex) __restrict = 0;

      bool is_execution_complete() const __restrict
      {
         return m_execution_complete;
      }

      bool is_execution_success() const __restrict
      {
         return m_execution_success;
      }

      bool is_debugger_owned() const __restrict
      {
         return m_options.debug_owned;
      }

      processor * get_processor() const __restrict
      {
         return m_processor;
      }

      debugger * get_debugger() const __restrict
      {
         return m_debugger;
      }
   };
}
