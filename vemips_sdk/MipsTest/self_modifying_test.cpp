#include <cstdio>
#include <cstdint>
#include <cstring>
#include <array>


namespace
{
	static constexpr std::array<uint32_t, 2> get_function_data(const int16_t return_value)
	{
		return {
			// jr      $ra
			0x03E00009U,
			// addiu   $2, $zero, return_value
			0x24020000U | uint16_t(return_value)
		};
	}

	static uint8_t func_array[sizeof(get_function_data(0))] __attribute__((__aligned__(2048)));
	static __attribute__((__always_inline__)) int call_func_array()
	{
		using func_array_func = int(*)();
		return func_array_func(func_array)();
	}

	static void update_func_array(const int16_t return_value)
	{
		auto&& instructions = get_function_data(return_value);
		
		std::memcpy(
			func_array,
			instructions.data(),
			sizeof(instructions)
		);
	}
}

int main()
{
	std::printf("main func: %p\n", (void*)&main);
	std::printf("test func: %p\n", (void*)func_array);
	
	const int expected0 = 2;
	const int expected1 = 3;
	update_func_array(expected0);
	const auto result0 = call_func_array();
	update_func_array(expected1);
	const auto result1 = call_func_array();
	
	bool error = false;
	
	if (result0 != expected0)
	{
		std::fprintf(stderr, "Test Failed: %d != %d\n", result0, expected0);
		error = true;
	}
	
	if (result1 != expected1)
	{
		std::fprintf(stderr, "Test Failed: %d != %d\n", result1, expected1);
		error = true;
	}
	
	if (!error)
	{
		std::printf("Test Passed\n");
		return 0;
	}
	else
	{
		std::printf("Test Failed\n");
		return 1;
	}
}
