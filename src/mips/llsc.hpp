#pragma once

#include <common.hpp>


namespace mips {
	enum class llsc : uint32 {
		fine = 0,
		coarse,
		none
	};
}
