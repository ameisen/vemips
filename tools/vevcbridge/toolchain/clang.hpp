#pragma once

#include "toolchain.hpp"

namespace buildcarbide::toolchain {
	class clang final : public toolchain {
		wstring compiler_path_;
		wstring linker_path_;

	public:
		clang() noexcept;
		virtual ~clang() noexcept override = default;

		virtual wstring get_compile_args(
			const options& __restrict options,
			const wstring& __restrict filepath,
			const wstring& __restrict configuration,
			const wstring& __restrict intermediate,
			wstring& __restrict object_file
		) override;

		virtual wstring get_link_args(
			const options& __restrict options,
			std::span<wstring> object_files,
			const wstring& __restrict filepath,
			const wstring& __restrict configuration
		) override;
	};
}
