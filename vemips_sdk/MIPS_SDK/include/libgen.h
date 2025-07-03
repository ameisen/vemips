#ifndef _LIBGEN_H
#define _LIBGEN_H

#ifdef __cplusplus
extern "C" {
#endif

char *dirname(char *);
#ifdef _GNU_SOURCE
char *basename(const char *);
#else
char *basename(char *);
#endif

#ifdef __cplusplus
}
#endif

#endif
