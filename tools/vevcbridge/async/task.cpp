#include "buildcarbide.hpp"

#include "task.hpp"

using namespace buildcarbide;

const uint32_t async::task::num_cores = []() -> uint32_t {
	return 0;
}();
