// Copyright 2016-2017 Digital Carbide / Michael Kuklinski. All Rights Reserved.

#include "common.hpp"

#include <cstdio>
#include <string>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cassert>

#ifdef NOMINMAX
#	undef NOMINMAX
#endif
#define NOMINMAX 1
#include <Windows.h>

std::mutex out_lock;
std::mutex start_lock;

#define printout(...)								\
do														 \
{														  \
	std::unique_lock<std::mutex> _ul(out_lock);  \
	printf(__VA_ARGS__);							 \
	fflush(stdout);									\
} while (false)

#define printraw(...)								\
do														 \
{														  \
	std::unique_lock<std::mutex> _ul(out_lock);  \
	fwrite(__VA_ARGS__);							 \
	fflush(stdout);									\
} while (false)


FILE *fp = nullptr;
std::string target_addr;
std::vector<std::string> pending_commands;
std::mutex pending_lock;

class debugger_process
{
	std::thread m_process_thread;

	HANDLE m_StdInRd = nullptr;
	HANDLE m_StdInWr = nullptr;
	HANDLE m_StdOutRd = nullptr;
	HANDLE m_StdOutWr = nullptr;

public:
	debugger_process(std::string procName, std::string arguments)
	{
		SECURITY_ATTRIBUTES saAttr;
		saAttr.nLength = sizeof(saAttr);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = nullptr;

		if (!CreatePipe(&m_StdOutRd, &m_StdOutWr, &saAttr, 0))
		{
			throw std::string("Could not create debugger process");
		}
		if (!SetHandleInformation(m_StdOutRd, HANDLE_FLAG_INHERIT, 0))
		{
			throw std::string("Could not create debugger process");
		}
		if (!CreatePipe(&m_StdInRd, &m_StdInWr, &saAttr, 0))
		{
			throw std::string("Could not create debugger process");
		}
		if (!SetHandleInformation(m_StdInWr, HANDLE_FLAG_INHERIT, 0))
		{
			throw std::string("Could not create debugger process");
		}

		PROCESS_INFORMATION piProcInfo;
		STARTUPINFO siStartInfo;
		memset(&piProcInfo, 0, sizeof(piProcInfo));
		memset(&siStartInfo, 0, sizeof(siStartInfo));
		siStartInfo.cb = sizeof(siStartInfo);
		siStartInfo.hStdError = m_StdOutWr;
		siStartInfo.hStdOutput = m_StdOutWr;
		siStartInfo.hStdInput = m_StdInRd;
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;


		fprintf(fp, " ++ Starting Debugger: %s\n", (procName + " " + arguments).c_str());
		BOOL res = CreateProcessA(
			nullptr,
			LPSTR((procName + " " + arguments).c_str()),
			nullptr,
			nullptr,
			TRUE,
			0,
			nullptr,
			nullptr,
			&siStartInfo,
			&piProcInfo
		);
		if (!res)
		{
			throw std::string("Could not create debugger process");
		}

		pending_lock.lock();
		for (auto cmd : pending_commands)
		{
			cmd += '\n';
			xassert(cmd.length() <= std::numeric_limits<DWORD>::max());
			DWORD toWrite = DWORD(cmd.length());
			DWORD totalWritten = 0;
			fprintf(fp, " >> ");
			while (toWrite)
			{
				DWORD writtenBytes = 0;
				WriteFile(m_StdInWr, cmd.data() + totalWritten, toWrite, &writtenBytes, nullptr);
				FlushFileBuffers(m_StdInWr);
				fwrite(cmd.data() + totalWritten, 1, writtenBytes, fp);
				toWrite -= writtenBytes;
				totalWritten += writtenBytes;
			}
		}
		pending_lock.unlock();

		m_process_thread = std::thread([this]() {
			std::string outBuffer;
			for (;;)
			{
				DWORD readBytes = 0;
				char buffer[512];
				if (ReadFile(m_StdOutRd, buffer, sizeof(buffer), &readBytes, nullptr) == 0)
				{
					exit(1);
				}

				for (size_t i = 0; i < readBytes; ++i)
				{
					outBuffer.push_back(buffer[i]);
					
					if (buffer[i] == '\n')
					{
						// consume anything that is just '^done'
						if (outBuffer.find("^done") != 0)
						{

							// check and fix filenames.
							std::string searchTok = "fullname=";
							size_t loc = outBuffer.find(searchTok);
							if (loc != std::string::npos)
							{
								loc += searchTok.length();
								char delim = outBuffer[loc];
								size_t end_loc = outBuffer.find(delim, loc + 1);

								while (loc < end_loc)
								{
									if (outBuffer[loc] == '\\' && outBuffer[loc + 1] != '\\')
									{
										outBuffer.insert(outBuffer.begin() + loc, '\\');
										++loc;
									}
									++loc;
								}
							}

							// push buffer.
							fprintf(fp, " ** ");
							fwrite(outBuffer.data(), 1, outBuffer.size(), fp);
							fflush(fp);

							printraw(outBuffer.data(), 1, outBuffer.size(), stdout);

						}
						outBuffer.clear();
					}
				}
			}
		});
	}
	~debugger_process()
	{
	}

	void write_to(std::string str)
	{
		str += '\n';

		xassert(str.length() <= std::numeric_limits<DWORD>::max());
		DWORD toWrite = DWORD(str.length());
		DWORD totalWritten = 0;
		fprintf(fp, " >> ");
		while (toWrite)
		{
			DWORD writtenBytes = 0;
			WriteFile(m_StdInWr, str.data() + totalWritten, toWrite, &writtenBytes, nullptr);
			FlushFileBuffers(m_StdInWr);
			fwrite(str.data() + totalWritten, 1, writtenBytes, fp);
			toWrite -= writtenBytes;
			totalWritten += writtenBytes;
		}
	}
};

class debugged_process
{
public:
	debugged_process(std::string procName, std::string arguments)
	{
		SECURITY_ATTRIBUTES saAttr;
		saAttr.nLength = sizeof(saAttr);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = nullptr;

		PROCESS_INFORMATION piProcInfo;
		STARTUPINFO siStartInfo;
		memset(&piProcInfo, 0, sizeof(piProcInfo));
		memset(&siStartInfo, 0, sizeof(siStartInfo));
		siStartInfo.cb = sizeof(siStartInfo);
		//siStartInfo.hStdError = m_StdOutWr;
		//siStartInfo.hStdOutput = m_StdOutWr;
		//siStartInfo.hStdInput = m_StdInRd;
		//siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

		fprintf(fp, " ++ Starting Debug App: %s\n", (procName + " " + arguments).c_str());
		BOOL res = CreateProcessA(
			nullptr,
			LPSTR((procName + " " + arguments).c_str()),
			nullptr,
			nullptr,
			TRUE,
			CREATE_NEW_CONSOLE,
			nullptr,
			nullptr,
			&siStartInfo,
			&piProcInfo
		);
		if (!res)
		{
			throw std::string("Could not create debugged process");
		}
	}
	~debugged_process()
	{
	}
};

int main(int argc, const char **argv)
{
	// This mutex starts locked.
	start_lock.lock();

	std::string mips_bin;
	std::string executable;
	debugger_process *m_debugger = nullptr;
	debugged_process *m_debugged = nullptr;

	bool handle_one_break = true;

	fp = fopen("D:\\dbgout.txt", "w");

	fprintf(fp, "argv\n");
	for (int i = 0; i < argc; ++i)
	{
		fprintf(fp, "%s\n", argv[i]);
	}

	fprintf(fp, "\n");

	printout("(gdb)\n");

	fflush(fp);

	std::vector<std::string> pass_args;

	bool first = false;

	for (;;)
	{
		std::string in;
		std::getline(std::cin, in);
		if (std::cin.eof() || std::cin.fail())
		{
			break;
		}

		if (in.length() == 0)
		{
			continue;
		}

		if (!first)
		{
			printout("(gdb)\n");
		}
		first = false;

		const std::string orig_in = in;

		fprintf(fp, "%s\n", in.c_str());
		fflush(fp);

		// extract ID if present.
		std::string token_id;
		while (in.size() && in.front() >= '0' && in.front() <= '9')
		{
			token_id.push_back(in.front());
			in.erase(0, 1);
		}

		const std::string in_no_tag = in;

		// Now tokenize the remainder.
		std::vector<std::string> tokens;

		const auto get_token = [&](size_t i) -> std::string
		{
			if (i >= tokens.size())
			{
				return "";
			}
			return tokens[i];
		};

		{
			std::string token;
			bool in_string = false;
			bool in_escape = false;
			while (in.size())
			{
				while (in.size() && (in_string || !isspace(in.front())))
				{
					if (!in_escape && in.front() == '\\')
					{
						in_escape = true;
					}
					else
					{
						char c = in.front();
						if (in_escape)
						{
							in_escape = false;
							switch (c)
							{
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
								case '\0': c = '\0'; break;
								default:
								{
									in.erase(0, 1);
									continue;
								}
							}
						}
						else
						{
							if (c == '\"')
							{
								in_string = !in_string;
								in.erase(0, 1);
								continue;
							}
						}

						token.push_back(c);
						in.erase(0, 1);
					}
				}
				while (in.size() && isspace(in.front()))
				{
					in.erase(0, 1);
				}
				if (token.size())
				{
					tokens.push_back(token);
					token.clear();
				}
			}
		}

		// Now we maybe have a token ID. Save it.
		if (get_token(0) == "-gdb-set" && !m_debugger)
		{
			if (get_token(1) == "target-async")
			{
				if (get_token(2) == "on")
				{
					printout("%s^done\n", token_id.c_str());
				}
				else if (get_token(2) == "off")
				{
					printout("%s^done\n", token_id.c_str());
				}
				else
				{
					printout("%s^error,msg=\"The request \'\'target-async\' expects \"on\" or \"off\"\' failed.\"\n", token_id.c_str());
				}
			}
			else if (get_token(1) == "solib-search-path")
			{
				std::string path = get_token(2);
				printout("%s^done\n", token_id.c_str());
				pass_args.push_back(orig_in);
			}
			else if (get_token(1) == "stop-on-solib-events")
			{
				std::string path = get_token(2);
				printout("%s^done\n", token_id.c_str());
				pass_args.push_back(orig_in);
			}
		}
		else if (get_token(0) == "-exec-arguments")
		{
			std::string arg = get_token(1);
			printout("%s^done\n", token_id.c_str());
			mips_bin = arg;
		}
		else if (get_token(0) == "-interpreter-exec")
		{
			if (get_token(1) == "console")
			{
				std::string cmd = get_token(2);
				if (cmd == "show configuration")
				{
					printout("%s^done,msg=\"mipsdbg\"\n", token_id.c_str());
				}
				else if (cmd == "show architecture")
				{
					printout("%s^done,msg=\"mipsel\"\n", token_id.c_str());
				}
				else
				{
					printout("%s^done\n", token_id.c_str());
				}
			}
		}
		else if (get_token(0) == "-file-exec-and-symbols")
		{
			std::string exec = get_token(1);
			printout("%s^done\n", token_id.c_str());
			executable = exec;
			while (executable.front() == '\\')
			{
				executable = executable.substr(1);
			}
		}
		else if (get_token(0) == "-target-select")
		{
			if (get_token(1) == "remote")
			{
				target_addr = get_token(2);

				std::string args = "--jit1 --mmu none --icache --debug ";
				// extract port from target_address, if it's there. Which is must be.
				size_t port_offset = target_addr.find(':');
				if (port_offset == std::string::npos)
				{
					exit(1);
				}
				std::string port = target_addr.substr(port_offset + 1);
				args += port + " ";
				args += mips_bin;
				m_debugged = new debugged_process(executable, args);

				printout("%s^done\n", token_id.c_str());
			}
			else
			{
				printout("%s^error\n", token_id.c_str());
				exit(1);
			}
		}
		else if (get_token(0) == "-break-insert" && handle_one_break)
		{
			printout("%s^done\n", token_id.c_str());
			handle_one_break = false;
		}
		else if (get_token(0) == "-exec-run")
		{
			if (!m_debugger)
			{
				m_debugger = new debugger_process("lldb-mi", mips_bin);
			}

			start_lock.unlock();

			std::string cmd0 = std::string("gdb-remote ") + target_addr + "\n";
			m_debugger->write_to(cmd0);

			printout("%s^running\n", token_id.c_str());
		}
		else if (get_token(0) == "kill")
		{
			printout("%s^done\n", token_id.c_str());
		}
		else if (get_token(0) == "exit")
		{
			printout("%s^done\n", token_id.c_str());
			return 0;
		}
		else if (get_token(0) == "-gdb-exit")
		{
			printout("%s^exit\n", token_id.c_str());
			return 0;
		}
		else if (get_token(0) == "quit")
		{
			printout("%s^done\n", token_id.c_str());
			return 0;
		}
		else if (get_token(0) == "logout")
		{
			printout("%s^done\n", token_id.c_str());
			return 0;
		}
		else if (get_token(0) == "-break-insert" && !m_debugger)
		{
			if (!m_debugger)
			{
				pending_lock.lock();
				pending_commands.push_back(orig_in);
				pending_lock.unlock();
				m_debugger = new debugger_process("lldb-mi", mips_bin);
			}
		}
		else
		{
			if (m_debugger)
			{
				m_debugger->write_to(orig_in);
			}
			else
			{
				printout("error: \'%s\' is not a valid command.\n", get_token(0).c_str());
				printout("error: Unrecognized command \'%s\'.\n", get_token(0).c_str());
				printout("%s^error\n", token_id.c_str());
				return 0;
			}
		}
	}

	return 0;
}
