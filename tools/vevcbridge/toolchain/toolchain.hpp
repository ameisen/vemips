#pragma once

#include <span>
#include <vector>

namespace buildcarbide {
	struct options;
}

namespace buildcarbide::toolchain {
	class toolchain {
	protected:
		toolchain() noexcept = default;
	public:
		virtual ~toolchain() noexcept = default;

		virtual wstring get_compile_args(
			const options& __restrict options,
			const wstring& __restrict filepath,
			const wstring& __restrict configuration,
			const wstring& __restrict intermediate,
			wstring& __restrict object_file
		) = 0;

		virtual wstring get_link_args(
			const options& __restrict options,
			std::span<wstring> object_files,
			const wstring& __restrict filepath,
			const wstring& __restrict configuration
		) = 0;

		static no_throw wstring merge_args(std::span<wstring> args) noexcept;
		static no_throw wstring merge_args(std::vector<wstring>&& args) noexcept;
	};
}
