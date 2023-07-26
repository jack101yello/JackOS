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

uint32_t freememstart = 0x10000; // This is somewhat arbitrary, and should be changed later if need be
uint32_t pagesize = 0x1000; // 4.096 KB, which is a standard page size

typedef struct Page {
    /* The memory bitmap for a page divides the ~4 KB space into 128 32-byte blocks. Objects cannot spread across blocks.
    The nth bit in the mth element of the array corresponds to the status of the nth byte of the mth block.
    0 -> unallocated    1 -> allocated
    */
    uint32_t memorybitmap[128];
    uint32_t start; // The address of the start of the page
    uint32_t end; // The address of the end of the page
} __attribute__((packed)) page;

typedef struct PageTable {
    /* The page bitmap works identically to the memory bitmap within a page. The nth bit in the mth element corresponds to
    the status of the nth page in the mth block, of which there are 32 (to get 1024=32*32 pages in all).
    */
    uint32_t pagebitmap[32];
    int lastpage;
    page *pages[1024]; // This is ~4 MB of memory, which should suffice for now. If not, this should be altered.
} __attribute__((packed)) pagetable;

pagetable table; // This is canonical pagetable that wil be used

// Get the memory address associated with the nth page
uint32_t locationofpage(int n) {
    return (uint32_t)((n * pagesize) + freememstart);
}

// Get the page number for a given memory address
int getpagenumber(uint32_t address) {
    return (int)((address - freememstart)/(pagesize));
}

// Return the rightmost 0 bit of a binary string (used for the bitmap heap implementation)
int rightmostbit(uint32_t binarystring) {
    long long int inverse = ~binarystring;
    int count, temp;
    for(count = 0, temp = ~inverse & (inverse - 1); temp > 0; temp >>= 1, ++count);
    return count;
}

page alloc_page() {
    int newpageindex = table.lastpage+1;
    page newpage;
    for(int i = 0; i < 32; i++) {
        newpage.memorybitmap[i] = 0b0;
    }
    newpage.start = locationofpage(newpageindex);
    newpage.end = newpage.start + pagesize;
    table.pages[newpageindex] = &newpage;
    table.pagebitmap[newpageindex/32] |= (1 << (newpageindex%32));
    table.lastpage = newpageindex;
    return newpage;
}

// Find a page that can be used (current implementation just returns the last page in the table)
page get_page() {
    return *(table.pages[table.lastpage]);
}


// Deallocate a page so that it can be overwritten
void unload_page(int index) {
    /*
    IMPLEMENTATION REQUIRED
    */
   if(index == 0) { // Dummy code to remove warnings
    return;
   }
}

// Allocate memory on the heap and return its location
uint32_t malloc(size_t size) { // Allocates memory on the heap
    page currentpage = get_page();
    for(int i = 0; i < 128; i++) { // Iterate over the 32-byte blocks, running the entire page
        for(long unsigned int j = 0; j <= (32 - size); j++) { // Iterate over each byte within the 32-byte block
            if(currentpage.memorybitmap[i] & (1 << j)) { // The memory there is in use
                continue; // Try the next bit of memory
            }
            else {
                // We need to find what the next used block of memory is to determine if we have enough space
                size_t distancetonextused = 0;
                for(int k = j+1; (j+k)<32; k++) { // Iterate through the rest of the block until we find a used byte
                    if(currentpage.memorybitmap[i] & (1 << (j+k))) { // We found a used byte!
                        distancetonextused = k - j;
                        break; // End the loop, since we've found the next used byte
                    }
                }
                if(distancetonextused > 0 && distancetonextused < size) { // True if a used block was found and it's too close
                    continue; // Try the next bit of memory (which will inevitably fail until we try the next 32-byte block)
                }
                /* At this point, we've found a block of memory which can accomodate the thing we want to allocate,
                since we have a distance which is greater than or equal to the size and which is entirely contained
                within one 32-byte block within the page
                */
               uint32_t addr = currentpage.start + 32*i + j; // The address of the start of the memory we can now allocate
               for(long unsigned int k = j; k < j+size; k++) { // Iterate through every byte that is now allocated
                    currentpage.memorybitmap[i] |= (1 << k);
               }
               return addr;
            }
        }
    }
    alloc_page(); // No memory was found in that page, so we need a new page
    return malloc(size); // We rerun this function now that we have a new page (yes, I agree that it is insane to make this recursive)
}

uint32_t realloc(size_t size) {
    /*
    IMPLEMENTATION REQUIRED
    */
   if(size == 0) { // Dummy code to remove warnings
    return 0;
   }
   return 0;
}

// Free memory on the heap (should be paired with malloc)
void free(uint32_t address) {
    int pageindex = getpagenumber(address); // The index for the page containing the address
    page *p = table.pages[pageindex]; // Create as a pointer because it's more memory efficient
    int relativelocation = address - p->start;
    int block = relativelocation/128; // The block containing the memory address
    int index = relativelocation%32; // The initial byte within the block containing the memory address
    for(long unsigned int i = 0; i < sizeof(*((void *)address)); i++) { // Iterate through the bytes taken up by the object
        p->memorybitmap[block] &= ~(1 << index); // Designate the bit unallocated
    }
}

// Function run by the kernel before the user does anything
void initialize_paging() {
    table.lastpage = 0;
}
