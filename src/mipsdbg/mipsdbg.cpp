// Copyright 2016-2017 Digital Carbide / Michael Kuklinski. All Rights Reserved.

#include "common.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

//#define WITH_FILE_OUTPUT 1
#define FILE_OUT "D:\\dbgout.txt"

#ifndef NOMINMAX
#	define NOMINMAX 1
#endif
#include <Windows.h>

namespace {
	static std::mutex out_lock;
	static std::mutex start_lock;

#define print_line(format, ...) ( \
		[&] { \
			std::unique_lock print_lock(out_lock); \
			fmt::println(format, __VA_ARGS__); \
			std::fflush(stdout); \
		}() \
	)

	template <typename... Args>
	static void print_raw(Args&&... args) {
		std::unique_lock print_lock(out_lock);
		std::fwrite(std::forward<Args>(args)..., stdout);
		std::fflush(stdout);
	}

#ifdef WITH_FILE_OUTPUT
	FILE* fp = nullptr;
#endif
	std::string target_address;
	std::vector<std::string> pending_commands;
	std::mutex pending_lock;

	class debugger_process final {
		std::thread process_thread_;

		struct stream_pair final {
			HANDLE read = nullptr;
			HANDLE write = nullptr;

			stream_pair() = default;
			stream_pair(const stream_pair&) = delete;
			stream_pair(stream_pair&& stream) noexcept : read(stream.read), write(stream.write) {
				stream.read = nullptr;
				stream.write = nullptr;
			}
			~stream_pair() {
				if (read) {
					CloseHandle(read);
				}
				if (write) {
					CloseHandle(write);
				}
			}

			stream_pair& operator = (const stream_pair&) = delete;
			stream_pair& operator = (stream_pair&& stream) noexcept {
				if (read) {
					CloseHandle(read);
				}
				if (write) {
					CloseHandle(write);
				}
				read = stream.read;
				write = stream.write;
				stream.read = nullptr;
				stream.write = nullptr;

				return *this;
			}
		};

		stream_pair stdout_ = {};
		stream_pair stdin_ = {};

	public:
		debugger_process(const std::string & process_name, const std::string & arguments)
		{
			SECURITY_ATTRIBUTES security_attributes = {
				.nLength = sizeof(security_attributes),
				.lpSecurityDescriptor = nullptr,
				.bInheritHandle = TRUE,
			};

			if (
				!CreatePipe(&stdout_.read, &stdout_.write, &security_attributes, 0) ||
				!SetHandleInformation(stdout_.read, HANDLE_FLAG_INHERIT, 0) ||
				!CreatePipe(&stdin_.read, &stdin_.write, &security_attributes, 0) ||
				!SetHandleInformation(stdin_.write, HANDLE_FLAG_INHERIT, 0)
			) [[unlikely]] {
				throw std::exception("Could not create debugger process");
			}

			PROCESS_INFORMATION process_info = {};
			STARTUPINFOA start_info = {
				.cb = sizeof(start_info),
				.dwFlags = STARTF_USESTDHANDLES,
				.hStdInput = stdin_.read,
				.hStdOutput = stdout_.write,
				.hStdError = stdout_.write
			};

			{
				const std::string command_line = process_name + " " + arguments;
#ifdef WITH_FILE_OUTPUT
				if (fp) {
					fmt::println(fp, " ++ Starting Debugger: {}", command_line);
				}
#endif
				if (
					const BOOL result = CreateProcessA(
						nullptr,
						LPSTR(command_line.c_str()),
						nullptr,
						nullptr,
						TRUE,
						0,
						nullptr,
						nullptr,
						&start_info,
						&process_info
					);
				!result
					) [[unlikely]] {
					throw std::exception("Could not create debugger process");
				}
			}

			{
				std::unique_lock pending_lock_lock(pending_lock);
				for (auto cmd : pending_commands) {
					cmd += '\n';
					xassert(cmd.length() <= std::numeric_limits<DWORD>::max());
					DWORD to_write = DWORD(cmd.length());
					DWORD total_written = 0;
#ifdef WITH_FILE_OUTPUT
					if (fp) {
						fmt::print(fp, " >> ");
					}
#endif
					while (to_write) {
						DWORD written_bytes = 0;
						WriteFile(stdin_.write, cmd.data() + total_written, to_write, &written_bytes, nullptr);
						FlushFileBuffers(stdin_.write);
#ifdef WITH_FILE_OUTPUT
						if (fp) {
							std::fwrite(cmd.data() + total_written, 1, written_bytes, fp);
						}
#endif
						to_write -= written_bytes;
						total_written += written_bytes;
					}
				}
			}

			process_thread_ = std::thread([this]() {
				std::string out_buffer;
				for (;;) {
					DWORD read_bytes = 0;
					char buffer[513];
					buffer[sizeof(buffer) - 1] = '\0';
					if (ReadFile(stdout_.read, buffer, sizeof(buffer) - 1, &read_bytes, nullptr) == 0) [[unlikely]] {
						std::exit(1);
					}

					for (size_t i = 0; i < read_bytes; ++i) {
						out_buffer.push_back(buffer[i]);

						if (buffer[i] == '\n') {
							// consume anything that is just '^done'
							if (out_buffer.find("^done") != 0) {
								// check and fix filenames.
								constexpr std::string_view search_token = "fullname=";
								if (
									size_t location = out_buffer.find(search_token);
									location != std::string::npos
								){
									location += search_token.length();
									const char delimiter = out_buffer[location];
									const size_t end_location = out_buffer.find(delimiter, location + 1);

									while (location < end_location) {
										if (out_buffer[location] == '\\' && out_buffer[location + 1] != '\\') {
											out_buffer.insert(out_buffer.begin() + location, '\\');
											++location;
										}
										++location;
									}
								}

								// push buffer.
#ifdef WITH_FILE_OUTPUT
								if (fp) {
									fmt::print(fp, " ** ");
									std::fwrite(out_buffer.data(), 1, out_buffer.size(), fp);
									std::fflush(fp);
								}
#endif

								print_raw(out_buffer.data(), 1, out_buffer.size());

							}
							out_buffer.clear();
						}
					}
				}
				});
		}

		void write_to(std::string str) const {
			str += '\n';

			xassert(str.length() <= std::numeric_limits<DWORD>::max());
			DWORD to_write = DWORD(str.length());
			DWORD total_written = 0;
#ifdef WITH_FILE_OUTPUT
			if (fp) {
				fmt::print(fp, " >> ");
			}
#endif
			while (to_write) {
				DWORD written_bytes = 0;
				WriteFile(stdin_.write, str.data() + total_written, to_write, &written_bytes, nullptr);
				FlushFileBuffers(stdin_.write);
#ifdef WITH_FILE_OUTPUT
				if (fp) {
					std::fwrite(str.data() + total_written, 1, written_bytes, fp);
				}
#endif
				to_write -= written_bytes;
				total_written += written_bytes;
			}
		}
	};

	class debugged_process final {
	public:
		debugged_process(const std::string & process_name, const std::string & arguments)
		{
			PROCESS_INFORMATION process_info = {};
			STARTUPINFOA start_info = {
				.cb = sizeof(start_info)
			};

			const std::string command_line = process_name + " " + arguments;
#ifdef WITH_FILE_OUTPUT
			if (fp) {
				fmt::println(fp, " ++ Starting Debug App: {}", command_line);
			}
#endif
			if (
				const BOOL result = CreateProcessA(
					nullptr,
					LPSTR(command_line.c_str()),
					nullptr,
					nullptr,
					TRUE,
					CREATE_NEW_CONSOLE,
					nullptr,
					nullptr,
					&start_info,
					&process_info
				);
				!result
			) [[unlikely]] {
				throw std::exception("Could not create debugged process");
			}
		}
	};
}

int main(int argc, const char **argv) {
	// This mutex starts locked.
	start_lock.lock();

	std::string mips_bin;
	std::string executable;
	std::unique_ptr<debugger_process> debugger = nullptr;
	std::unique_ptr<debugged_process> debugged = nullptr;

	bool handle_one_break = true;

#ifdef WITH_FILE_OUTPUT
	fp = std::fopen(FILE_OUT, "w");
	if (!fp) { [[unlikely]]
		fmt::println(stderr, "Could not open file '" FILE_OUT "' for logging");
	}
#endif

#ifdef WITH_FILE_OUTPUT
	if (fp) {
		fmt::println(fp, "argv");
		for (int i = 0; i < argc; ++i) {
			fmt::println(fp, "{}", argv[i]);
		}
		fmt::println(fp, "\n");
		std::fflush(fp);
	}
#endif

	print_line("(gdb)");

	std::vector<std::string> pass_args;

	bool first = false;

	for (;;) {
		std::string in;
		std::getline(std::cin, in);
		if (std::cin.eof() || std::cin.fail()) {
			break;
		}

		if (in.length() == 0) {
			continue;
		}

		if (!first) [[unlikely]] {
			print_line("(gdb)");
		}
		first = false;

		const std::string orig_in = in;

#ifdef WITH_FILE_OUTPUT
		if (fp) {
			fmt::println(fp, "{}", in);
			std::fflush(fp);
		}
#endif

		// extract ID if present.
		std::string token_id;
		while (!in.empty() && in.front() >= '0' && in.front() <= '9') {
			token_id.push_back(in.front());
			in.erase(0, 1);
		}

		const std::string in_no_tag = in;

		// Now tokenize the remainder.
		std::vector<std::string> tokens;

		const auto get_token = [&](const size_t i) -> const std::string & {
			if (i >= tokens.size()) {
				static const std::string empty;
				return empty;
			}
			return tokens[i];
		};

		{
			std::string token;
			bool in_string = false;
			bool in_escape = false;
			while (!in.empty()) {
				while (!in.empty() && (in_string || !isspace(in.front()))) {
					if (!in_escape && in.front() == '\\') {
						in_escape = true;
					}
					else {
						char c = in.front();
						if (in_escape) {
							in_escape = false;
							switch (c) {
								case 'n': c = '\n'; break;
								case 't': c = '\t'; break;
								case 'v': c = '\v'; break;
								case 'b': c = '\b'; break;
								case 'r': c = '\r'; break;
								case 'f': c = '\f'; break;
								case 'a': c = '\a'; break;
								case '\\': c = '\\'; break;
								case '?': c = '?'; break;
								case '\'': c = '\''; break;
								case '\"': c = '\"'; break;
								case '0': c = '\0'; break;
								default: {
									in.erase(0, 1);
									continue;
								}
							}
						}
						else {
							if (c == '\"') {
								in_string = !in_string;
								in.erase(0, 1);
								continue;
							}
						}

						token.push_back(c);
						in.erase(0, 1);
					}
				}
				while (!in.empty() && isspace(in.front())) {
					in.erase(0, 1);
				}
				if (!token.empty()) {
					tokens.push_back(token);
					token.clear();
				}
			}
		}

		// Now we maybe have a token ID. Save it.
		if (get_token(0) == "-gdb-set" && !debugger) {
			if (get_token(1) == "target-async") {
				if (get_token(2) == "on") {
					print_line("{}^done", token_id);
				}
				else if (get_token(2) == "off") {
					print_line("{}^done", token_id);
				}
				else [[unlikely]] {
					print_line("{}^error,msg=\"The request \'\'target-async\' expects \"on\" or \"off\"\' failed.\"", token_id);
				}
			}
			else if (get_token(1) == "solib-search-path") {
				//auto&& path = get_token(2);
				print_line("{}^done", token_id);
				pass_args.push_back(orig_in);
			}
			else if (get_token(1) == "stop-on-solib-events") {
				//auto&& path = get_token(2);
				print_line("{}^done", token_id);
				pass_args.push_back(orig_in);
			}
		}
		else if (get_token(0) == "-exec-arguments") {
			auto&& arg = get_token(1);
			print_line("{}^done", token_id);
			mips_bin = arg;
		}
		else if (get_token(0) == "-interpreter-exec") {
			if (get_token(1) == "console") {
				auto&& cmd = get_token(2);
				if (cmd == "show configuration") {
					print_line("{}^done,msg=\"mipsdbg\"", token_id);
				}
				else if (cmd == "show architecture") {
					print_line("{}^done,msg=\"mipsel\"", token_id);
				}
				else {
					print_line("{}^done", token_id);
				}
			}
		}
		else if (get_token(0) == "-file-exec-and-symbols") {
			auto&& exec = get_token(1);
			print_line("{}^done", token_id);
			executable = exec;
			while (executable.front() == '\\') {
				executable = executable.substr(1);
			}
		}
		else if (get_token(0) == "-target-select") {
			if (get_token(1) == "remote") {
				target_address = get_token(2);

				// extract port from target_address, if it's there. Which is must be.
				size_t port_offset = target_address.find(':');
				if (port_offset == std::string::npos) [[unlikely]] {
					std::exit(1);
				}
				std::string_view port = std::string_view{target_address}.substr(port_offset + 1);
				// TODO : Properly escape `mips_bin`
				std::string args = fmt::format("--jit1 --mmu none --icache --debug {} {}", port, mips_bin);
				debugged = std::make_unique<debugged_process>(executable, args);

				print_line("{}^done", token_id);
			}
			else [[unlikely]] {
				print_line("{}^error\n", token_id);
				std::exit(1);
			}
		}
		else if (get_token(0) == "-break-insert" && handle_one_break) {
			print_line("{}^done", token_id);
			handle_one_break = false;
		}
		else if (get_token(0) == "-exec-run") {
			if (!debugger) {
				debugger = std::make_unique<debugger_process>("lldb-mi", mips_bin);
			}

			start_lock.unlock();

			// TODO : use variadics and avoid an allocation and copy
			std::string cmd0 = fmt::format("gdb-remote {}\n", target_address);
			debugger->write_to(cmd0);

			print_line("{}^running", token_id);
		}
		else if (get_token(0) == "kill") {
			print_line("{}^done", token_id);
		}
		else if (get_token(0) == "exit") {
			print_line("{}^done", token_id);
			return 0;
		}
		else if (get_token(0) == "-gdb-exit") {
			print_line("{}^exit", token_id);
			return 0;
		}
		else if (get_token(0) == "quit") {
			print_line("{}^done", token_id);
			return 0;
		}
		else if (get_token(0) == "logout") {
			print_line("{}^done", token_id);
			return 0;
		}
		else if (get_token(0) == "-break-insert" && !debugger) {
			if (!debugger) {
				{
					std::unique_lock pending_lock_lock(pending_lock);
					pending_commands.push_back(orig_in);
				}
				debugger = std::make_unique<debugger_process>("lldb-mi", mips_bin);
			}
		}
		else {
			if (debugger) {
				debugger->write_to(orig_in);
			}
			else [[unlikely]] {
				print_line("error: \'{}\' is not a valid command.", get_token(0));
				print_line("error: Unrecognized command \'{}\'.\n", get_token(0));
				print_line("{}^error\n", token_id);
				return 0;
			}
		}
	}

	return 0;
}
