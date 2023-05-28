#pragma once

#include "project/project.hpp"
#include "file_utils/file_utils.hpp"
#include "configuration.hpp"

#include <unordered_set>
#include <string>
#include <functional>

namespace buildcarbide {
	class build {
		struct build_element final {
			project::element proj_element;
			std::unordered_set<std::wstring> includes;
			file_utils::modtime_t mod_time;
		};

		std::vector<build_element> build_elements;

	public:
		build(const configuration & __restrict configuration, const project & __restrict project);
		build(const configuration & __restrict configuration, std::function<project * ()>&& project_getter);
		~build();

	private:
		build(const configuration & __restrict configuration, const project & __restrict project, bool owned);
	};
}
