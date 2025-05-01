#include <libc/libc.h>

using namespace jackos;
using namespace jackos::libc;

void jackos::libc::strcpy(char* dest, char* src) {
    int i = 0;
    while(true) {
        dest[i] = src[i];
        if(dest[i] == '\0') {
            break;
        }
        ++i;
    }
}

jackos::common::size_t jackos::libc::strlen(const char* str) {
    const char* s = str;
    while(*s) {
        s++;
    }
    return s - str;
}

int jackos::libc::strcmp(const char *s1, const char *s2) {
    while(*s1 != '\0' && *s2 != '\0' && *s1 == *s2) {
        ++s1;
        ++s2;
    }
    return (*s1 - *s2);
}