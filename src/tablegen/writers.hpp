#pragma once

#include <span>

#include "tablegen.hpp"

namespace vemips::tablegen {
	struct options;

	namespace writers {
		void header(const options& options, std::span<const info_t* __restrict> proc_infos);
		void source(const options& options, std::span<const info_t* __restrict> proc_infos);
	}
}
