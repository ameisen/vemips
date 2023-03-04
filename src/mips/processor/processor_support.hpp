#pragma once

#include <type_traits>

namespace mips {
	class processor;
	class coprocessor;

	extern _nothrow processor * get_current_processor();
	extern _nothrow coprocessor * get_current_coprocessor();
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<coprocessor, T>>>
	static inline _nothrow T* get_current_coprocessor() {
		return raw_ptr<T>(get_current_coprocessor());
	}
	extern _nothrow void set_current_coprocessor(coprocessor * __restrict cop);
}
