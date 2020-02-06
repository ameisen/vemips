#pragma once

#include "project.hpp"

namespace buildcarbide
{
	class vc_project : public project
	{
	public:
		vc_project(const std::string & __restrict filename);
		virtual ~vc_project();

		static project * __restrict create_or_null(const std::string & __restrict filename);
	};
}
