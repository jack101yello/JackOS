#ifndef __jackos_filesystem_fat12_H
#define __jackos_filesystem_fat12_H

#define DATA_AREA_START 33 // 1 boot, 9*2 FAT, 14 root

#include <common/types.h>

namespace jackos {
    namespace filesystem {
        struct __attribute__((packed)) FAT12Header {
            jackos::common::uint8_t jmpboot[3]; // Jump instruction
            char OEMName[8]; // OEM Identifier
            jackos::common::uint16_t bps; // Bytes per sector (usually 512)
            jackos::common::uint8_t spc; // Sectors per cluster (usually 1)
            jackos::common::uint16_t reservedSectors; // Usually 1
            jackos::common::uint8_t numFATs; // Usually 2
            jackos::common::uint16_t rootEntryCount; // Usually 224
            jackos::common::uint16_t totalSectors16; // For 1.44MB 3.5", usually 2880
            jackos::common::uint8_t mediaType;
            jackos::common::uint16_t sectorsPerFAT; // Usually 9
            jackos::common::uint16_t sectorsPerTrack;
            jackos::common::uint16_t numberOfHeads;
            jackos::common::uint32_t hiddenSectors;
            jackos::common::uint32_t totalSectors32;

            jackos::common::uint8_t driveNumber;
            jackos::common::uint8_t reserved1;
            jackos::common::uint8_t bootSignature;
            jackos::common::uint32_t volumeID;
            char volumeLabel[11];
            char fileSystemType[8];
        };

        struct __attribute__((packed)) FAT12DirectoryEntry {
            char name[8]; // Filename
            char ext[3]; // Extension
            jackos::common::uint8_t attributes;
            jackos::common::uint8_t reserved[10];
            jackos::common::uint16_t time;
            jackos::common::uint16_t date;
            jackos::common::uint16_t firstCluster;
            jackos::common::uint16_t fileSize;
        };

        jackos::common::uint16_t read_fat_entry(jackos::common::uint8_t* fat, int cluster);
    }
}

#endif