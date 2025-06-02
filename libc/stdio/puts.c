#include <stdio.h>

int puts(const char* string) {
    /* This behavior may not be standard. This is an issue worth
    looking into in the future, but the ports that are expected
    to come soon are unlikely to use this, so it's probably fine.
    */
    return print(string);
}