#include "project.hpp"

#include <array>

#include "vc_project.hpp"

using namespace buildcarbide;

namespace
{
	using project_fp = project * __restrict (*)(const std::string & __restrict filename);

	template <typename... Ts>
	struct project_switcher
	{
		static const constexpr std::array<project_fp, sizeof...(Ts)> get_project_types()
		{
			return { { Ts::create_or_null... } };
		}
	};
}

project * __restrict buildcarbide::get_project(const std::string & __restrict filename)
{
	for (project_fp fp : project_switcher<vc_project>::get_project_types())
	{
		project * __restrict ret = fp(filename);
		if (ret)
		{
			return ret;
		}
	}
	return nullptr;
}
