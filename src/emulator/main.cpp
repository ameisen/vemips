#include "pch.hpp"

#include "main.hpp"

#include <cstddef>
#include <cstdio>
#include <concepts>
#include <memory>
#include <span>
#include <vector>
#include <fmt/format.h>

#include "common.hpp"
#include "options.hpp"
#include "platform.hpp"
#include "system_vemix.hpp"
#if EMSCRIPTEN
	#include "base64.hpp"
#endif


namespace 
{
	template <typename TChar> requires (std::same_as<TChar, char> || std::same_as<TChar, wchar_t>)
	static int main_impl(const std::span<const TChar*> args) {
		using namespace vemips;

		platform::set_process_high_priority();

		auto parsed_argument_data = options::parse(args);
		if (!parsed_argument_data) [[unlikely]]
		{
			return parsed_argument_data.error();
		}

		options::argument_data argument_data = std::move(parsed_argument_data.value());

		auto start_time = std::chrono::high_resolution_clock::now();
		uint64 instructions = 0;

		mips::processor::statistics statistics;
		size_t largest_jit_instruction = 0;

		try {
			std::unique_ptr<mips::system> system;

			fmt::println("Processing ELF Binary");
			elf::binary elf_binary{ argument_data.binary_data };
			fmt::println("Configuring VCPU and Configuring VENV");
			
			mips::system::options sys_options = {
				.total_memory = argument_data.available_memory,
				.stack_memory = argument_data.stack_memory,
				.jit_type = argument_data.jit,
				.mmu_type = argument_data.mmu_type,
				.debug_port = argument_data.debug.port,
				.read_only_exec = argument_data.use_rox,
				.collect_statistics = argument_data.collect_statistics,
				.disable_cti = argument_data.disable_cti,
				.ticked = argument_data.ticks != 0,
				.instruction_cache = argument_data.instruction_cache,
				.debug = argument_data.debug.enabled,
				.debug_owned = argument_data.debug.enabled,
				.strict_noncoherence = argument_data.strict_noncoherence
			};
			try {
				sys_options.validate();
			}
			catch (const std::exception& ex) {
				[[unlikely]]
				fmt::println(stderr, "Failed to validate system options:\n\t{}", ex.what());
				return 1;
			}

			system = std::make_unique<system_vemix>(sys_options, elf_binary);

			fmt::println("Beginning Execution ---");
			std::ignore = std::fflush(stdout);
			start_time = std::chrono::high_resolution_clock::now();
			try {
				for (;;) {
					system->clock(argument_data.ticks);
					//fmt::println("Instructions: {}", processor.get_instruction_count());
				}
			}
			catch (...) {
				instructions = system->get_instruction_count();
				if (argument_data.collect_statistics) {
					statistics.append(*system->get_statistics());
					largest_jit_instruction = system->get_jit_max_instruction_size();
				}
				throw;
			}
		}
		catch (const std::runtime_error& exception) {
			fmt::println(stderr, "\n** Error: {}", exception.what());
		}
		catch (mips::ExecutionCompleteException) {
			fmt::println("\n** 'main' return detected - execution terminated");
		}
		catch (mips::ExecutionFailException) {
			fmt::println(stderr, "\n** Unhandled CPU exception - execution terminated");
		}
		catch (...) {
			fmt::println(stderr, "\n** Error");
		}

		const auto end_time = std::chrono::high_resolution_clock::now();
		const auto duration = end_time - start_time;
		const uint64 ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		const double seconds = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();

		const double ips = double(instructions) / seconds;
		const uint64 ips_rounded = uint64(std::rint(ips));

		if (const uint64 cpu_frequency = platform::get_host_frequency(); cpu_frequency == 1) {
			fmt::println("** Execution Duration: {} ms ({} ips)", ms, ips_rounded);
		}
		else {
			const double frequency_ratio = double(cpu_frequency * 1'000'000) / ips;

			fmt::println("** Execution Duration: {} ms ({} ips - 1:{:.2f} guest/host)", ms, ips_rounded, frequency_ratio);
		}
		fmt::println("** Instructions Executed: {}", instructions);

		if (argument_data.collect_statistics) {
			fmt::println("** Collected Statistics:");

			if (largest_jit_instruction) {
				fmt::println("\tLargest JIT Instruction: {} bytes", largest_jit_instruction);
			}
			fmt::println("\tJIT Transitions: {}", statistics.jit_transitions);
			std::puts("");

			struct final {
				bool operator()(const std::pair<const char *, size_t> & __restrict a, const std::pair<const char *, size_t> & __restrict b) const __restrict {
					return a.second > b.second;
				}	
			} constexpr custom_comparator;

			const auto dump_instructions = [&](const char* const tab, const mips::processor::statistics::instruction_map& data) {
				std::vector<std::pair<const char *, size_t>> instruction_stats;
				instruction_stats.reserve(data.size());
				for (const auto& item : data)
				{
					instruction_stats.emplace_back(item);
				}

				std::ranges::stable_sort(instruction_stats, custom_comparator);

				size_t max_len = 0;
				for (const auto &name : instruction_stats | std::views::keys) {
					max_len = std::max(max_len, std::strlen(name));
				}

				for (const auto &[name, count] : instruction_stats) {
					fmt::println("{}{:<{}} - {}", tab, name, max_len, count);
				}
			};

			std::puts("\tAll Instructions:");
			dump_instructions("\t\t", statistics.instructions);
			if (argument_data.jit != mips::JitType::None)
			{
				std::puts("");
				std::puts("\tEmulated Instructions:");
				dump_instructions("\t\t", statistics.jit_emulated_instructions);
			}
		}

		std::ignore = std::getchar();

		return 0;
	}
}

#ifdef UNICODE
// ReSharper disable once IdentifierTypo
int wmain(const int argc, const wchar_t* argv[]) {  // NOLINT(misc-use-internal-linkage)
	return main_impl(std::span{ argv, argv + argc });
}
#else
int main(const int argc, const char* argv[]) {
	return main_impl(std::span{ argv, argv + argc });
}
#endif
