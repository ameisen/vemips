#pragma once

#if defined(_WIN32)
#  include "win32/process_win32.hpp"
#else
#  error "Unknown Platform"
#endif
