#ifndef __jackos_filesystem_initrd_H
#define __jackos_filesystem_initrd_H

#include <common/types.h>
#include <filesystem/vfs.h>
#include <memorymanagement.h>
#include <libc/libc.h>

namespace jackos {
    namespace filesystem {

        typedef struct {
            jackos::common::uint32_t nfiles; // The number of files in the ramdisk
        } initrd_header_t;

        typedef struct {
            jackos::common::uint8_t magic;
            char name[64];
            jackos::common::uint32_t offset; // Offset in initrd where file starts
            jackos::common::uint32_t length; // Length of the file
        } initrd_file_header_t;

        fs_node_t* initialize_initrd(jackos::common::uint32_t location);
    }
}

#endif