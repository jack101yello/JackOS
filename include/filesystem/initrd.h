#ifndef __jackos_filesystem_initrd_H
#define __jackos_filesystem_initrd_H

#include <common/types.h>
#include <filesystem/vfs.h>
#include <memorymanagement.h>
#include <libc/libc.h>

namespace jackos {
    namespace filesystem {

		jackos::common::uint32_t initrd_read(fs_node_t* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
		struct dirent *initrd_readdir(fs_node_t* node, jackos::common::uint32_t index);
		fs_node_t* initrd_finddir(fs_node_t* node, char *name);

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
		
		extern initrd_header_t* initrd_header;
		extern initrd_file_header_t* file_headers;
		extern fs_node_t* root_nodes;
    }
}

#endif
