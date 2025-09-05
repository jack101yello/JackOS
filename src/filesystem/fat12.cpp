#include <filesystem/fat12.h>

using namespace jackos;
using namespace jackos::common;

uint16_t read_fat_entry(uint8_t* fat, int cluster) {
    int offset = cluster + cluster/2; // 1.5 bytes per entry
    if(cluster & 1) {
        return ((fat[offset] >> 4) | (fat[offset+1] << 4)) & 0x0FFF;
    }
    return (fat[offset] | ((fat[offset+1] & 0x0F) << 8)) & 0x0FFF;
}