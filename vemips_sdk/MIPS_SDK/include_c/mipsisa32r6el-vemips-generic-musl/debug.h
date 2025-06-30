#ifndef _DEBUG_H
#define _DEBUG_H

#	if _MUSL_VEMIPS

#ifdef __cplusplus
extern "C" {
#endif

void vemips_dbg(long value0, ...);

#ifdef __cplusplus
}
#endif

#	endif

#endif
