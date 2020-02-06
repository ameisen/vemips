#pragma once

#include <common.hpp>

namespace mips {
	enum class mmu : uint32 {
		emulated = 0,
		host,
		none
	};
}
