#pragma once

#include <string>
#include <vector>

#include "platform/platform.hpp"

namespace buildcarbide::process {
	class process final {
		struct stream_pair final {
			system_handle<0> read;
			system_handle<0> write;
		};

		stream_pair stdin_ = {};
		stream_pair stdout_ = {};
		system_handle<0> process_;
		system_handle<0> thread_;
		mutable bool terminated_ = false;

	public:
		process(const std::wstring & __restrict executable, const std::wstring & __restrict args);
		process(const process&) = delete;

		class process_output final : std::vector<std::string_view> {
			std::vector<char> backing_;
		public:
			no_throw process_output(std::vector<char>&& backing, std::vector<std::string_view>&& lines) noexcept : std::vector<std::string_view>(std::move(lines)), backing_(std::move(backing)) {}
			no_throw process_output(const process_output&) noexcept = default;
			no_throw process_output(process_output&&) noexcept = default;
		};

		[[nodiscard]]
		process_output get_output() const __restrict;
		[[nodiscard]]
		int get_return_code() const __restrict;
	};
}
