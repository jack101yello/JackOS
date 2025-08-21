#include <stdio.h>

int print(const char* data) {
    asm("int $0x80" : : "a" (4), "b" (data));
    return 0;
}
