<<<<<<< Updated upstream
#ifndef __jackos_common_common_H
#define __jackos_common_common_H

#include <common/types.h>

namespace jackos {
    namespace common {
        void memcpy(uint8_t* dest, const uint8_t* src, uint32_t len) {
            const uint8_t* sp = src;
            uint8_t *dp = dest;
            for(; len != 0; len--) {
                *dp++ = *sp++;
            }
        }

        void memset(uint8_t* dest, uint8_t val, uint32_t len) {
            uint8_t* temp = dest;
            for(; len != 0; len--) {
                *temp++ = val;
            }
        }
    }
}

=======
#ifndef __jackos_common_common_H
#define __jackos_common_common_H

#include <common/types.h>

namespace jackos {
    namespace common {
        extern void memcpy(uint8_t* dest, const uint8_t* src, uint32_t len);
        extern void memset(uint8_t* dest, uint8_t val, uint32_t len);
    }
}

>>>>>>> Stashed changes
#endif