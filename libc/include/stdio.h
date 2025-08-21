#ifndef __stdio_H
#define __stdio_H

#include <sys/cdefs.h>

#define EOF (-1)
#define SEEK_SET 0

#ifdef __cplusplus
extern "C" {
#endif

int print(const char* msg);
int putchar(int);
int puts(const char*);

// Things that are required by GCC
typedef struct { int unused; } FILE;
extern FILE* stderr;
int fflush(FILE*);
int fclose(FILE*);
FILE* fopen(const char*, const char*);
int fprintf(FILE*, const char*, ...);
size_t fread(void*, size_t, size_t, FILE*);
int fseek(FILE*, long, int);
long ftell(FILE*);
size_t fwrite(const void*, size_t, size_t, FILE*);
void setbuf(FILE*, char*);
int vfprintf(FILE*, const char*, va_list);

#ifdef __cplusplus
}
#endif

#endif
