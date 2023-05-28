#include "buildcarbide.hpp"

#include "process_win32.hpp"

#include "platform/platform_headers.hpp"

using namespace buildcarbide;

process::process::process(const std::wstring & __restrict executable, const std::wstring & __restrict args) {
	SECURITY_ATTRIBUTES security_attributes = {
		.nLength = sizeof(security_attributes),
		.lpSecurityDescriptor = nullptr,
		.bInheritHandle = TRUE,
	};

	if (
		!CreatePipe(stdout_.read.initialize(), stdout_.write.initialize(), &security_attributes, 0) ||
		!SetHandleInformation(HANDLE(stdout_.read), HANDLE_FLAG_INHERIT, 0) ||
		!CreatePipe(stdin_.read.initialize(), stdin_.write.initialize(), &security_attributes, 0) ||
		!SetHandleInformation(HANDLE(stdin_.write), HANDLE_FLAG_INHERIT, 0)
	) {
		terminated_ = true;
		throw std::exception("Could not create process");
	}

	PROCESS_INFORMATION process_information = {};
	STARTUPINFO start_info = {
		.cb = sizeof(start_info),
		.dwFlags = STARTF_USESTDHANDLES,
		.hStdInput = HANDLE(stdin_.read),
		.hStdOutput = HANDLE(stdout_.write),
		.hStdError = HANDLE(stdout_.write),
	};

	if (
		const BOOL result = CreateProcess(
			nullptr,
			LPWSTR((executable + L" " + args).c_str()),
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
		terminated_ = true;
		throw std::exception("Could not create process");
	}

	stdin_.write.~system_handle();
	stdout_.write.~system_handle();

	process_ = process_information.hProcess;
	thread_ = process_information.hThread;
}

process::process::process_output process::process::get_output() const __restrict {
	if (!terminated_) {
		// wait for process to terminate.
		while (WaitForSingleObject(HANDLE(process_), INFINITE) == WAIT_TIMEOUT) {
		}

		terminated_ = true;
	}

	std::vector<std::string_view> out;

	static constexpr const DWORD buffer_increase = 513;
	std::vector<char> full_string;
	size_t total_size = 0;
	for (;;) {
		full_string.resize(full_string.size() + buffer_increase);
		DWORD read_bytes = 0;
		if (!ReadFile(HANDLE(stdout_.read), full_string.data() + total_size, buffer_increase, &read_bytes, nullptr)) {
			break;
		}

		total_size += read_bytes;
	}
	full_string.resize(total_size);

	bool last_carriage_return = false;
	size_t start_offset = 0;
	size_t current_offset = 0;

	const auto push_line = [&] (const bool carriage) {
		if (carriage && last_carriage_return) {
			last_carriage_return = false;
			start_offset += 2;
			return;
		}
		if (start_offset == current_offset) {
			return;
		}
		if (current_offset - start_offset == 1 && (full_string[start_offset] == '\n' || full_string[start_offset] == '\r')) {
			out.emplace_back(&full_string[start_offset], 0);
		}
		else {
			out.emplace_back(&full_string[start_offset], current_offset - start_offset);
		}
		start_offset = current_offset;
	};

	for (const char c : full_string) {
		switch (c) {
			case '\r':
				push_line(true);
				last_carriage_return = true;
				break;
			case '\n':
				push_line(true);
				break;
			default:
				last_carriage_return = false;
				break;
		}
		++current_offset;
	}

	push_line(false);

	return {std::move(full_string), std::move(out)};
}

int process::process::get_return_code() const __restrict {
	DWORD code;
	if (!terminated_) {
		// wait for process to terminate.
		while ((code = WaitForSingleObject(HANDLE(process_), INFINITE)) == WAIT_TIMEOUT) {
		}

		terminated_ = true;
	}
	else {
		code = WAIT_OBJECT_0;
	}

	DWORD exit_code = 0;

	if (
		const BOOL result = GetExitCodeProcess(HANDLE(process_), &exit_code);
		code == WAIT_FAILED || result == 0 || exit_code == STILL_ACTIVE
	)
	{
		// something bad happened.
		throw std::exception("Internal error retrieving subprocess status");
	}

	return int(exit_code);
}
