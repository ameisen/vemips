#include "task.hpp"

using namespace buildcarbide;

const uint32_t async::task::sm_num_cores = []() -> uint32_t {
	return 0;
}();
