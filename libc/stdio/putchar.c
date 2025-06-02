#include <stdio.h>
#include <common/printf.h>

int putchar(int ic) {
    char c = (char) ic;
    char* foo = {' ', '\0'};
    foo[0] = c;
    printf(foo);
    return ic;
}