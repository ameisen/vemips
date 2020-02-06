#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace buildcarbide::process
{
	class process
	{
		void * m_StdInRd = nullptr;
		void * m_StdInWr = nullptr;
		void * m_StdOutRd = nullptr;
		void * m_StdOutWr = nullptr;
		void * m_Process = nullptr;
		void * m_Thread = nullptr;

	public:
		process(const std::string & __restrict executable, const std::string & __restrict args);
		~process();

		std::vector<std::string> get_output() const __restrict;
		int get_return_code() const __restrict;
	};
}
