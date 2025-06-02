#ifndef __stdio_H
#define __stdio_H

#include <sys/cdefs.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int print(const char* __restrict, ...);
int putchar(int);
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif