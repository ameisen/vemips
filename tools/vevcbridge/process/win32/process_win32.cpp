#include "process_win32.hpp"

#ifdef NOMINMAX
#	undef NOMINMAX
#endif
// ReSharper disable once IdentifierTypo
#define NOMINMAX 1
#include <Windows.h>

using namespace buildcarbide;

process::process::process(const std::string & __restrict executable, const std::string & __restrict args) {
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
	) {
		throw std::string("Could not create process");
	}

	PROCESS_INFORMATION process_information = {};
	STARTUPINFO start_info = {
		.cb = sizeof(start_info),
		.dwFlags = STARTF_USESTDHANDLES,
		.hStdInput = stdin_.read,
		.hStdOutput = stdout_.write,
		.hStdError = stdout_.write,
	};

	if (
		const BOOL result = CreateProcessA(
			nullptr,
			LPSTR((executable + " " + args).c_str()),
			nullptr,
			nullptr,
			TRUE,
			0,
			nullptr,
			nullptr,
			&start_info,
			&process_information
		);
	!result
	) {
		throw std::string("Could not create process");
	}

	process_ = process_information.hProcess;
	thread_ = process_information.hThread;
}

process::process::~process() {
	const auto safe_close_handle = [](const HANDLE handle) {
		if (handle) {
			CloseHandle(handle);
		}
	};

	safe_close_handle(stdin_.read);
	safe_close_handle(stdin_.write);
	safe_close_handle(stdout_.read);
	safe_close_handle(stdout_.write);
	safe_close_handle(thread_);
	safe_close_handle(process_);
}

std::vector<std::string> process::process::get_output() const __restrict {
	std::vector<std::string> out;

	std::string full_string;
	for (;;) {
		DWORD read_bytes = 0;
		char buffer[513] = {};
		if (ReadFile(stdout_.read, buffer, sizeof(buffer), &read_bytes, nullptr)) {
			break;
		}

		full_string += buffer;
	}

	out.emplace_back();
	std::string *cur_string = &out.back();
	for (const char c : full_string) {
		switch (c) {
			case '\r':
				// ignore \r
				break;
			case '\n':
				// Push string, get new one.
				out.emplace_back();
				cur_string = &out.back();
				break;
			default:
				*cur_string += c;
				break;
		}
	}

	return out;
}

int process::process::get_return_code() const __restrict {
	// wait for process to terminate.
	DWORD code;
	while ((code = WaitForSingleObject(process_, INFINITE)) == WAIT_TIMEOUT) {
	}

	DWORD exit_code = 0;

	if (
		const BOOL result = GetExitCodeProcess(process_, &exit_code);
		code == WAIT_FAILED || result == 0 || exit_code == STILL_ACTIVE
	)
	{
		// something bad happened.
		throw std::string("Internal error retrieving subprocess status");
	}

	return int(exit_code);
}
