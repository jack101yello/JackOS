#include "mem.h"

void memory_copy(uint8_t *source, uint8_t *dest, int nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

uint32_t memstart = 0x10000;
uint32_t freespace;

// Allocate memory on the heap and return its location
uint32_t malloc(size_t size) {
    uint32_t loc = freespace;
    freespace += size;
    kprint("Putting the variable at: ");
    char s[16];
    int_to_ascii(loc, s);
    kprint(s);
    kprint("\n");
    return loc;
}

// Reallocate a section of memory
uint32_t realloc(uint32_t address, size_t size) {
    if(size == 0 || address >= memstart) return (uint32_t) NULL;
    return address;
}

// Free memory on the heap (should be paired with malloc)
void free(uint32_t address) {
    if(address < memstart) return; // Make sure that the memory address isn't before the heap.
    // This implementation has no way to keep track of what is available
}

// Function run by the kernel before the user does anything
void initialize_paging() {
    freespace = memstart;
}
