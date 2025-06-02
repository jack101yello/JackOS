#ifndef __jackos_libc_H
#define __jackos_libc_H

#include <common/types.h>

namespace jackos {
    namespace libc {
        void strcpy(char* dest, char* src);
        jackos::common::size_t strlen(const char* str);
        int strcmp(const char *s1, const char *s2);
    }
}

#endif