#pragma once

#include "project.hpp"

namespace buildcarbide {
	class vc_project final : public project {
	public:
		vc_project(const std::wstring & __restrict filename);
		virtual ~vc_project() override;

		[[nodiscard]]
		static project * create_or_null(const std::wstring & __restrict filename);
	};
}
