#include <stdint.h>
#include <errno.h>
#include "syscall.h"

void *sbrk(intptr_t inc)
{
	return (void *)__syscall(SYS_sbrk, inc);
}
