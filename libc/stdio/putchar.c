#include <stdio.h>

int putchar(int ic) {
    char c = (char) ic;
    const char foo[2] = {c, '\0'};
    print(foo);
    return ic;
}
