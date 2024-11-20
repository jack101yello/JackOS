#ifndef __jackos_filesystem_FAT_H
#define __jackos_filesystem_FAT_H

#include <common/types.h>

namespace jackos {
    namespace filesystem {
        struct BOOT_SECTOR {
            jackos::common::uint8_t ignore[3]; // jmp instruction
            BIOSPARAMETERBLOCK bpb; // BPB structure
            BIOSPARAMETERBLOCKEXT bpbext; // Extended BPB
            jackos::common::uint8_t filler[448]; // Sector must be 512 bytes
        };

        struct BIOSPARAMETERBLOCK {
            jackos::common::uint8_t OEMName[8];
            jackos::common::uint16_t bytes_per_sector;
            jackos::common::uint8_t sectors_per_cluster;
            jackos::common::uint16_t reserved_sectors;
            jackos::common::uint8_t number_of_FATs;
            jackos::common::uint16_t num_dir_entries;
            jackos::common::uint16_t num_sectors;
            jackos::common::uint8_t media;
            jackos::common::uint16_t sectors_per_FAT;
            jackos::common::uint16_t sectors_per_track;
            jackos::common::uint16_t heads_per_cyl;
            jackos::common::uint32_t hidden_sectors;
            jackos::common::uint32_t long_sectors;
        };

        struct BIOSPARAMETERBLOCKEXT {
            jackos::common::uint32_t sectors_per_FAT32;
            jackos::common::uint16_t flags;
            jackos::common::uint16_t version;
            jackos::common::uint32_t root_cluster; // Starting root directory
            jackos::common::uint16_t info_cluster;
            jackos::common::uint16_t backup_boot; // Copy of the bootsector
            jackos::common::uint16_t reserved[6];
        };
    }
}

#endif