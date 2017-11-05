#include <string.h>
#include <errno.h>
#include "libc.h"

// PC_MMK
char *strerror_r(int err, char *buf, size_t buflen)
{
	char *msg = strerror(err);
	size_t l = strlen(msg);
	if (l >= buflen) {
		if (buflen) {
			memcpy(buf, msg, buflen-1);
			buf[buflen-1] = 0;
		}
      return buf;
	}
	memcpy(buf, msg, l+1);
	return buf;
}

weak_alias(strerror_r, __xpg_strerror_r);
