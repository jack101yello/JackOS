#include <common/common.h>

using namespace jackos;
using namespace jackos::common;

void jackos::common::memcpy(uint8_t* dest, const uint8_t* src, uint32_t len) {
    const uint8_t* sp = src;
    uint8_t *dp = dest;
    for(; len != 0; len--) {
        *dp++ = *sp++;
    }
}

void jackos::common::memset(uint8_t* dest, uint8_t val, uint32_t len) {
    uint8_t* temp = dest;
    for(; len != 0; len--) {
        *temp++ = val;
    }
}