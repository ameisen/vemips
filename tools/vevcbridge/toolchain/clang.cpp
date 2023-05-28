// ReSharper disable StringLiteralTypo
#include "buildcarbide.hpp"

#include "clang.hpp"
#include "options.hpp"
#include "file_utils/file_utils.hpp"

#include "platform/platform_headers.hpp"
#include "platform/win32/win32_system_handle.hpp"

#include "process/win32/process_win32.hpp"

using namespace buildcarbide;
using namespace buildcarbide::toolchain;

namespace {
	struct stream final {
		system_handle<0> read;
		system_handle<0> write;
	};

	static no_throw std::optional<wstring> get_version(const wstring &path) noexcept {
		SECURITY_ATTRIBUTES security_attributes = {
			.nLength = sizeof(security_attributes),
			.lpSecurityDescriptor = nullptr,
			.bInheritHandle = TRUE,
		};

		// Create a pipe for the child process's STDOUT.

		stream child_stdout;

		if (!CreatePipe(child_stdout.read.initialize(), child_stdout.write.initialize(), &security_attributes, 0)) {
			return {};
		}

		// Ensure the read handle to the pipe for STDOUT is not inherited.

		if (!SetHandleInformation(HANDLE(child_stdout.read), HANDLE_FLAG_INHERIT, 0)) {
			return {};
		}

		STARTUPINFO si = {
			.cb = sizeof(si),
			.dwFlags = STARTF_USESTDHANDLES,
			.hStdOutput = HANDLE(child_stdout.write),
			.hStdError = HANDLE(child_stdout.write)
		};
		PROCESS_INFORMATION pi = {};

		wstring command_line = path + L" --version";

		// Start the child process. 
		if (!CreateProcessW(
			nullptr,                        // No module name (use command line)
			command_line.data(),            // Command line
			nullptr,                        // Process handle not inheritable
			nullptr,                        // Thread handle not inheritable
			TRUE,                           // Set handle inheritance
			0,                              // No creation flags
			nullptr,                        // Use parent's environment block
			nullptr,                        // Use parent's starting directory 
			&si,                            // Pointer to STARTUPINFO structure
			&pi                             // Pointer to PROCESS_INFORMATION structure
			)) {
			return {};
		}

		child_stdout.write.~system_handle();

		std::vector<char> output;

		static constexpr const size_t buffer_size = 4'096;

		DWORD read_bytes;
		BOOL success;

		size_t total_size = 0;
		do {
			output.resize(output.size() + buffer_size);

			success = ReadFile(
				HANDLE(child_stdout.read),
				output.data() + total_size,
				buffer_size,
				&read_bytes,
				nullptr
			);
			if (read_bytes != 0) {
				total_size += read_bytes;
			}

			// Log chBuf
		}
		while (success || read_bytes != 0);

		output.resize(total_size);

		if (output.empty()) [[unlikely]] {
			return {};
		}

		return to_wstring(output.data());

		/*
		std::tuple<const stream&, std::vector<char>&> thread_params = {
			child_stdout,
			output
		};

		system_handle<0> read_stream_thread = CreateThread(
			nullptr,
			0,
			LPTHREAD_START_ROUTINE(&read_stream),
			&thread_params,
			0,
			nullptr
		);

		WaitForSingleObject()
		*/
	}

	template <typename... Args>
	static wstring find_executable(Args&&... args) {

	}
}

clang::clang() noexcept {

}

wstring clang::get_compile_args(
	const options &__restrict options,
	const wstring &__restrict filepath,
	const wstring &__restrict configuration,
	const wstring &__restrict intermediate,
	wstring &__restrict object_file
) {
	// TODO rework this.

	//auto vv = get_version(options.sdk_path + L"/mipsld");

	//auto vp = process::process(options.sdk_path + L"/mipsld", L"--version");
	//auto vv = vp.get_output();

	std::vector<wstring> cfg = {
		L"clang",
		L"-target", L"mipsel-unknown-linux",
		L"-march=mips32r6",
		L"-ffunction-sections",
		L"-fdata-sections",
		L"-mcompact-branches=always",
		L"-fasynchronous-unwind-tables",
		L"-funwind-tables",
		L"-fexceptions",
		L"-fcxx-exceptions",
		L"-std=gnu++2b",
		L"-stdlib=libc++"
	};

	if (configuration == L"Debug") {
		static constexpr const wchar_t *const args[] = {
			L"-O0",
			L"-glldb"
		};
		cfg.append_range(args);
	}
	else if (configuration == L"Release") {
		static constexpr const wchar_t *const args[] = {
			L"-mno-check-zero-division",
			L"-O3",
			L"-Os"
		};
		cfg.append_range(args);
	}

	const auto system_include = [&cfg] (wstring&& path) {
		cfg.push_back(fmt::format(L"-I{}", std::move(path)));
	};

	system_include(file_utils::build_path(options.sdk_path, L"incxx"));
	system_include(file_utils::build_path(options.sdk_path, L"inc"));
	cfg.emplace_back(L"-c");

	cfg.push_back(filepath);

	object_file = file_utils::build_path(intermediate, (file_utils::get_file_name(filepath) + L".o"));

	cfg.emplace_back(L"-o");
	cfg.push_back(object_file);

	return merge_args(cfg);
}

wstring clang::get_link_args(
	const options &__restrict options,
	std::span<wstring> object_files,
	const wstring &__restrict filepath,
	const wstring &__restrict configuration
) {
	// TODO rework this.

	std::vector<wstring> cfg = {
		L"ld.lld",
		L"-znorelro",
		L"--eh-frame-hdr"
	};

	if (configuration == L"Debug") {
		static constexpr const wchar_t *const args[] = {
			L"--discard-none"
		};
		cfg.append_range(args);
	}
	else if (configuration == L"Release") {
		static constexpr const wchar_t *const args[] = {
			L"--discard-all",
			L"--gc-sections",
			L"--icf=all",
			L"--strip-all"
		};
		cfg.append_range(args);
	}

	{
		static constexpr const wchar_t *const args[] = {
			L"-lc",
			L"-lc++",
			L"-lc++abi",
			L"-lllvmunwind"
		};
		cfg.append_range(args);
	}

	const auto library_path = [&cfg] (wstring&& path) {
		cfg.push_back(fmt::format(L"-L{}", std::move(path)));
	};

	library_path(file_utils::build_path(options.sdk_path, L"lib"));

	cfg.append_range(object_files);

	cfg.emplace_back(L"-o");
	cfg.push_back(filepath);

	return merge_args(cfg);
}
