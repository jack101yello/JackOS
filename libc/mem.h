#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <stddef.h>

#include "../drivers/screen.h"
#include "string.h"

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes);
void memory_set(uint8_t *dest, uint8_t val, uint32_t len);

uint32_t malloc(size_t size);
uint32_t realloc(uint32_t address, size_t size);
void free(uint32_t address);
void initialize_paging();

#endif
