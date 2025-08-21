#ifndef __jackos_common_common_H
#define __jackos_common_common_H

#include <common/types.h>

namespace jackos {
    namespace common {
        extern void memcpy(uint8_t* dest, const uint8_t* src, uint32_t len);
        extern void memset(uint8_t* dest, uint8_t val, uint32_t len);
    }
}

#endif