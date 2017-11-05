#define _GNU_SOURCE || 1 // PC_MMK
#include <dlfcn.h>
#include "libc.h"

static int stub_dladdr(const void *addr, Dl_info *info)
{
	return 0;
}

weak_alias(stub_dladdr, dladdr);
