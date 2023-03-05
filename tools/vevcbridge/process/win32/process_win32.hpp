#pragma once

#include <string>
#include <vector>

namespace buildcarbide::process {
	class process final {
		struct stream_pair final {
			void * read = nullptr;
			void * write = nullptr;
		};

		stream_pair stdin_ = {};
		stream_pair stdout_ = {};
		void * process_ = nullptr;
		void * thread_ = nullptr;

	public:
		process(const std::string & __restrict executable, const std::string & __restrict args);
		process(const process&) = delete;
		~process();

		[[nodiscard]]
		std::vector<std::string> get_output() const __restrict;
		[[nodiscard]]
		int get_return_code() const __restrict;
	};
}
