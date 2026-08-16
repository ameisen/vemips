__attribute__((noinline))
int foo0()
{
	volatile int a;
	return a += 1;
}

int bar0() 
{
	[[clang::musttail]] return foo0();
}
