#include "process_win32.hpp"

#define NOMINMAX 1
#include <Windows.h>

using namespace buildcarbide;

process::process::process(const std::string & __restrict executable, const std::string & __restrict args)
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(saAttr);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = nullptr;

	if (!CreatePipe(&m_StdOutRd, &m_StdOutWr, &saAttr, 0))
	{
		throw std::string("Could not create process");
	}
	if (!SetHandleInformation(m_StdOutRd, HANDLE_FLAG_INHERIT, 0))
	{
		throw std::string("Could not create process");
	}
	if (!CreatePipe(&m_StdInRd, &m_StdInWr, &saAttr, 0))
	{
		throw std::string("Could not create process");
	}
	if (!SetHandleInformation(m_StdInWr, HANDLE_FLAG_INHERIT, 0))
	{
		throw std::string("Could not create process");
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

	BOOL res = CreateProcessA(
		nullptr,
		LPSTR((executable + " " + args).c_str()),
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
		throw std::string("Could not create process");
	}

	m_Process = piProcInfo.hProcess;
	m_Thread = piProcInfo.hThread;
}

process::process::~process()
{
	if (m_StdInRd)
	{
		CloseHandle(m_StdInRd);
	}
	if (m_StdInWr)
	{
		CloseHandle(m_StdInWr);
	}
	if (m_StdOutRd)
	{
		CloseHandle(m_StdOutRd);
	}
	if (m_StdOutWr)
	{
		CloseHandle(m_StdOutWr);
	}
	if (m_Thread)
	{
		CloseHandle(m_Thread);
	}
	if (m_Process)
	{
		CloseHandle(m_Process);
	}
}

std::vector<std::string> process::process::get_output() const __restrict
{
	std::vector<std::string> out;

	std::string full_string;
	for (;;)
	{
		DWORD readBytes = 0;
		char buffer[513];
		memset(buffer, 0, sizeof(buffer));
		if (ReadFile(m_StdOutRd, buffer, sizeof(buffer), &readBytes, nullptr))
		{
			break;
		}

		full_string += buffer;
	}

	out.emplace_back();
	std::string *cur_string = &out.back();
	for (char c : full_string)
	{
		if (c == '\r')
		{
			// ignore \r
			continue;
		}
		else if (c == '\n')
		{
			// Push string, get new one.
			out.emplace_back();
			cur_string = &out.back();
		}
		else
		{
			*cur_string += c;
		}
	}

	return out;
}

int process::process::get_return_code() const __restrict
{
	// wait for process to terminate.
	DWORD code;
	while ((code = WaitForSingleObject(m_Process, INFINITE)) == WAIT_TIMEOUT)
	{}

	DWORD exit_code = 0;
	BOOL result = GetExitCodeProcess(m_Process, &exit_code);

	if (code == WAIT_FAILED || result == 0 || exit_code == STILL_ACTIVE) // something bad happened.
	{
		throw std::string("Internal error retrieving subprocess status");
	}

	return int(exit_code);
}
