#include "buildcarbide.hpp"

#include "project.hpp"

#include <array>

#include "vc_project.hpp"

using namespace buildcarbide;

namespace {
	using project_fp = project * (*)(const std::wstring & __restrict filename);

	template <typename... Ts>
	struct project_switcher final {
		project_switcher() = delete;

		static constexpr std::array<project_fp, sizeof...(Ts)> get_project_types() {
			return { { Ts::create_or_null... } };
		}
	};
}

project * buildcarbide::get_project(const std::wstring & __restrict filename)
{
	for (const project_fp fp : project_switcher<vc_project>::get_project_types()) {
		if (project * __restrict result = fp(filename)) {
			return result;
		}
	}
	return nullptr;
}
