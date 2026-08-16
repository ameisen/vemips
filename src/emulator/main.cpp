#include "pch.hpp"

#include "main.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <concepts>
#include <exception>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <tuple>
#include <utility>
#include <vector>
#include <fmt/format.h>

#include "common.hpp"
#include "options.hpp"
#include "platform.hpp"
#include "system_vemix.hpp"
#if EMSCRIPTEN
	#include "base64.hpp"
#endif
#include "elf/elf.hpp"
#include "mips/exception.hpp"
#include "mips/statistics.hpp"
#include "mips/system.hpp"
#include "mips/processor/processor.hpp"
#include "mips/processor/jit/jit.hpp"


namespace 
{
	struct final {
		[[nodiscard]]
		_pure _nothrow bool operator()(
			const std::pair<const char*, size_t> & __restrict a,
			const std::pair<const char*, size_t> & __restrict b
		) const __restrict noexcept {
			return a.second > b.second;
		}	
	} constexpr custom_comparator;

	template <char_type CharT>
	[[nodiscard]]
	static int main_sxs_impl(const std::span<const CharT*> args, vemips::options::argument_data& argument_data) {
		using namespace vemips;

		argument_data.ticks = 1;
		
		fmt::println("Processing ELF Binary");
		mips::elf::binary elf_binary{ argument_data.binary_data };
		fmt::println("Configuring VCPU and Configuring VENV for Side-by-Side comparison");

		std::unique_ptr<mips::system> systems[2] = {
			[&]() -> std::unique_ptr<mips::system> {
				mips::system::options sys_options = {
					.total_memory = argument_data.available_memory,
					.stack_memory = argument_data.stack_memory,
					.jit_type = mips::JitType::None,
					.llsc_type = argument_data.llsc_type,
					.mmu_type = argument_data.mmu_type,
					.debug_port = argument_data.debug.port,
					.read_only_exec = argument_data.use_rox,
					.collect_statistics = false,
					.disable_cti = argument_data.disable_cti,
					.ticked = true,
					.debug = false,
					.debug_owned = false,
					.self_modifying_code = argument_data.self_modifying_code
				};
				try {
					sys_options.validate();
				}
				catch (const std::exception& ex) {
					[[unlikely]]
					fmt::println(stderr, "Failed to validate system options:\n\t{}", ex.what());
					return {};
				}

				return std::make_unique<system_vemix>(sys_options, elf_binary);
			}(),

			[&]() -> std::unique_ptr<mips::system> {
				mips::system::options sys_options = {
					.total_memory = argument_data.available_memory,
					.stack_memory = argument_data.stack_memory,
					.jit_type = argument_data.jit,
					.llsc_type = argument_data.llsc_type,
					.mmu_type = argument_data.mmu_type,
					.debug_port = argument_data.debug.port,
					.read_only_exec = argument_data.use_rox,
					.collect_statistics = false,
					.disable_cti = argument_data.disable_cti,
					.ticked = true,
					.debug = false,
					.debug_owned = false,
					.self_modifying_code = argument_data.self_modifying_code
				};
				try {
					sys_options.validate();
				}
				catch (const std::exception& ex) {
					[[unlikely]]
					fmt::println(stderr, "Failed to validate system options:\n\t{}", ex.what());
					return {};
				}

				return std::make_unique<system_vemix>(sys_options, elf_binary, true);
			}()
		};

		for (const auto& system : systems)
		{
			if (!system)
			{
				fmt::println(stderr, "Failed to create system");
				return -1;
			}
		}

		fmt::println("Beginning Execution ---");
		std::ignore = std::fflush(stdout);

		bool executing[std::size(systems)] = { true, true };

		bool error = false;
		bool mismatch = false;

		while (executing[0] || executing[1]) [[likely]]
		{
			for (size_t i = 0; i < std::size(systems); ++i)
			{
				if (!executing[i]) {
					continue;
				}

				try {
					systems[i]->clock(argument_data.ticks);
				}
				catch (mips::ExecutionCompleteException) {
					executing[i] = false;
					fmt::println("\n** 'main' return detected - execution terminated");
				}
				catch (...) {
					executing[i] = false;
					fmt::println(
						"Exception executing systems[{}] (PC: {:08X} IC: {})",
						i, systems[i]->get_processor()->get_program_counter(),
						systems[i]->get_instruction_count()
					);
					error = true;
				}
			}

			bool exec_base_value = executing[0];

			for (bool value : executing)
			{
				if (value != exec_base_value) [[unlikely]]
				{
					fmt::println("State Mismatch Detected: a system has stopped execution");
					mismatch = true;
					break;
				}
			}

			const auto compare_memory = [] <bool Shadow> (
				const char* const tab,
				const std::span<char> memory0,
				const std::span<char> memory1
			) {
				if (memory0.empty() && memory1.empty())
				{
					return;
				}

				bool printed_header = false;

				const auto print_header = [&]
				{
					if (printed_header)
					{
						return;
					}
					std::puts("");
					fmt::println("{}{}Memory:",
						tab,
						Shadow ? "Shadow " : ""
					);
					printed_header = true;
				};

				if (memory0.size() != memory1.size())
				{
					print_header();
					fmt::println("{}{}Memory size mismatch: {} != {}", tab,
						Shadow ? "Shadow " : "",
						memory0.size(),
						memory1.size()
					);
					
					return;
				}

				if (memory0.data() == memory1.data())
				{
					xassert(memory0.data() != memory1.data());
					return;
				}

				// TODO : just assuming 4-byte aligned for now
				if (std::memcmp(memory0.data(), memory1.data(), memory0.size()) != 0) [[unlikely]]
				{
					using compare_type = uint32;

					const compare_type* const __restrict memory0_ptr = reinterpret_cast<const compare_type* __restrict>(memory0.data());
					const compare_type* const __restrict memory1_ptr = reinterpret_cast<const compare_type* __restrict>(memory1.data());

					constexpr usize max_mismatches = 16;
					usize num_mismatches = 0;

					bool first = true;

					for (
						uint32 i = 0, ptr_index = 0;
						i < memory0.size();
						i += sizeof(compare_type), ++ptr_index
					)
					{
						const uint32 value0 = memory0_ptr[ptr_index];
						const uint32 value1 = memory1_ptr[ptr_index];

						if (value0 != value1) [[unlikely]]
						{
							if (first)
							{
								print_header();
								fmt::println("{}{}Memory Mismatches:", tab, Shadow ? "Shadow " : "");
								first = false;
							}

							fmt::println(
								"{}\t@{:08X}: {:08X} != {:08X}",
								tab,
								i,
								value0,
								value1
							);

							if (++num_mismatches >= max_mismatches)
							{
								break;
							}
						}
					}
				}
				return;
			}; 

			if (
				const auto compare_result = systems[0]->compare(*systems[1]);
				!compare_result.empty()
			) [[unlikely]]
			{
				mismatch = true;

				fmt::println("State Mismatch Detected: system state mismatch");
				fmt::println("Instruction Count: {}", systems[0]->get_instruction_count());

				for (size_t i = 0; i < std::size(systems); ++i)
				{
					const size_t other_system_i = i == 0 ? 1 : 0;

					auto& system = systems[i];
					auto* processor = system->get_processor();

					auto& other_system = systems[other_system_i];
					auto* other_processor = other_system->get_processor();

					fmt::println("System {} state:", i);

					const auto& registers = processor->get_register_file();
					const auto& other_registers = other_processor->get_register_file();

					const auto get_difference_mark = [] (const auto& a, const auto& b) {
						return (a != b) ? 'X' : ' ';
					};

					fmt::println("\tRegisters:");
					for (size_t ri = 0; ri < registers.size(); ri += 4)
					{
						size_t indices[4] = {
							ri,
							ri + 1,
							ri + 2,
							ri + 3
						};

						fmt::println(
							"\t\t{:2}: {:08X} {} {:2}: {:08X} {} {:2}: {:08X} {} {:2}: {:08X} {}",
							indices[0], registers[indices[0]], get_difference_mark(registers[indices[0]], other_registers[indices[0]]),
							indices[1], registers[indices[1]], get_difference_mark(registers[indices[1]], other_registers[indices[1]]),
							indices[2], registers[indices[2]], get_difference_mark(registers[indices[2]], other_registers[indices[2]]),
							indices[3], registers[indices[3]], get_difference_mark(registers[indices[3]], other_registers[indices[3]])
						);
					}
					fmt::println(
						"\t\tpc: {:08X} {} bt: {:08X} {} uv: {:08X} {}",
						processor->get_program_counter(), get_difference_mark(processor->get_program_counter(), other_processor->get_program_counter()),
						processor->get_branch_target(), get_difference_mark(processor->get_branch_target(), other_processor->get_branch_target()),
						processor->get_user_value(), get_difference_mark(processor->get_user_value(), other_processor->get_user_value())
					);

					fmt::println("\tFlags:");
					{
						using flag = mips::processor::flag;

						const auto flags = processor->get_all_flags();
						const auto other_flags = other_processor->get_all_flags();

						const auto check_flag = [](const flag flags, const flag flag) {
							return (flags & flag) != mips::processor::flag::none;	
						};

						#define check_print_flag(flag_name) \
							if (check_flag(flags, flag:: flag_name)) { \
								fmt::println("\t\t" #flag_name " {} ", check_flag(other_flags, flag:: flag_name) ? ' ' : 'X'); \
							}

						check_print_flag(branch_delay);
						check_print_flag(no_cti);
						check_print_flag(trapped_exception);
						check_print_flag(pc_changed);
						check_print_flag(instruction_hazard);
						check_print_flag(instruction_hazard_delay_branch);

						if (flags == mips::processor::flag::none)
						{
							std::puts("\t\tnone");
						}

						#undef check_print_flag
					}
				}

				compare_memory.template operator()<false>(
					"\t",
					systems[0]->get_memory(),
					systems[1]->get_memory()
				);
				compare_memory.template operator()<true>(
					"\t",
					systems[0]->get_shadow_memory(),
					systems[1]->get_shadow_memory()
				);

				std::puts("");

				std::puts("\tRaw Report:");

				for (const auto& line : compare_result)
				{
					fmt::println("\t\t{}", line);
				}

				break;
			}

			if (error && !mismatch)
			{
				fmt::println("No State Mismatch detected...");
			}
		}

		return (mismatch || error) ? -2 : 0;
	}

	template <char_type CharT>
	[[nodiscard]]
	static int main_impl(const std::span<const CharT*> args) {
		using namespace vemips;

		// TODO : This is largely temporary until we find a better way to do this.
		platform::set_console_min_width(130);

		platform::set_process_high_priority();

		auto parsed_argument_data = options::parse(args);
		if (!parsed_argument_data) [[unlikely]]
		{
			return parsed_argument_data.error();
		}

		options::argument_data argument_data = std::move(parsed_argument_data.value());

		if (argument_data.side_by_side)
		{
			const int result = main_sxs_impl(args, argument_data);

			std::ignore = std::getchar();

			return result;
		}
		else
		{
			auto start_time = std::chrono::high_resolution_clock::now();
			uint64 instructions = 0;

			mips::statistics statistics;
			std::pair<const char*, size_t> largest_jit_instruction = { nullptr, 0 };

			try {
				std::unique_ptr<mips::system> system;

				fmt::println("Processing ELF Binary");
				mips::elf::binary elf_binary{ argument_data.binary_data };
				fmt::println("Configuring VCPU and Configuring VENV");
			
				mips::system::options sys_options = {
					.total_memory = argument_data.available_memory,
					.stack_memory = argument_data.stack_memory,
					.jit_type = argument_data.jit,
					.llsc_type = argument_data.llsc_type,
					.mmu_type = argument_data.mmu_type,
					.debug_port = argument_data.debug.port,
					.read_only_exec = argument_data.use_rox,
					.collect_statistics = argument_data.collect_statistics,
					.disable_cti = argument_data.disable_cti,
					.ticked = argument_data.ticks != 0,
					.debug = argument_data.debug.enabled,
					.debug_owned = argument_data.debug.enabled,
					.self_modifying_code = argument_data.self_modifying_code
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
			catch (const std::exception& exception) {
				fmt::println(stderr, "\n** Internal Error: {}", exception.what());
			}
			catch (mips::ExecutionCompleteException) {
				fmt::println("\n** 'main' return detected - execution terminated");
			}
			catch (mips::ExecutionFailException) {
				fmt::println(stderr, "\n** Unhandled CPU exception - execution terminated");
			}
			catch (...) {
				fmt::println(stderr, "\n** Unknown Internal Error");
			}

			const auto end_time = std::chrono::high_resolution_clock::now();
			const auto duration = end_time - start_time;
			const uint64 ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
			const double seconds = ms / 1'000.0;

			const double ips = double(instructions) / seconds;
			const uint64 ips_rounded = uint64(std::rint(ips));

			if (uint64 cpu_frequency = platform::get_host_frequency(); cpu_frequency == 1) {
				fmt::println("** Execution Duration: {} ms ({} ips)", ms, ips_rounded);
			}
			else {
				cpu_frequency *= 1'000'000;

				/*
				// This is based on my local system, which finishes the benchmark in ~6.025s @ 3901.
				// It also runs this in Release-LLVM (at the time) in 228.487s @ 187'805'812 ips.
				// This gives it an assumed ratio of 1:20.7714551454, and an assumed time of
				// 11.0000478253 s, which is too low, as the actual ratio is closer to 1:37.259253112.
				// The ratio of 11.0000478253 to 6.025 is 1.82573407889.
				*/
				constexpr double experimental_time = 6.025;
				constexpr double calculated_time = 11.0000478253;
				constexpr double cpu_frequency_fudge_mult = calculated_time / experimental_time;
				const double cpu_frequency_modified = cpu_frequency * cpu_frequency_fudge_mult;

				const double frequency_ratio = double(cpu_frequency_modified) / ips;

				if (ms > 1'000)
				{
					fmt::println("** Execution Duration: {:.3f} s ({} ips - 1:{:.2f} guest/host)", seconds, ips_rounded, frequency_ratio);
				}
				else
				{
					fmt::println("** Execution Duration: {} ms ({} ips - 1:{:.2f} guest/host)", ms, ips_rounded, frequency_ratio);
				}
			}
			fmt::println("** Instructions Executed: {}", instructions);

			if (argument_data.collect_statistics) {
				fmt::println("** Collected Statistics:");

				if (largest_jit_instruction.second) {
					if (largest_jit_instruction.first != nullptr)
					{
						fmt::println("\tLargest JIT Instruction: {} bytes ({})", largest_jit_instruction.second, largest_jit_instruction.first);
					}
					else
					{
						fmt::println("\tLargest JIT Instruction: {} bytes", largest_jit_instruction.second);
					}
				}
				fmt::println("\tJIT Transitions: {}", statistics.jit_transitions);
				std::puts("");

				const auto dump_instructions = [&](const char* const tab, const mips::statistics::instruction_map& data) {
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
