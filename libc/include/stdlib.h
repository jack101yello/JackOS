#ifndef __stdlib_H
#define __stdlib_H

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__))
void abort(void);
int atexit(void (*)(void));
int atoi(const char*);
void free(void*);
char* getenv(const char*);
void* malloc(size_t;

#ifdef __cplusplus
}
#endif

#endif
