#pragma once

#include "common.hpp"
#include "tablegen.hpp"

#include <optional>
#include <span>

namespace vemips::tablegen {
	struct option final {
		const crstring tag;
		crstring value = nullptr;
	};

	struct options final {
		const crstring file_out = nullptr;
		const bool write_header = false;
		const bool write_source = false;
	};
}
