#ifndef __jackos_filesystem_initrd_H
#define __jackos_filesystem_initrd_H

#include <common/types.h>
#include <std/c_string.h>
#include <filesystem/vfs.h>
#include <common/common.h>
#include <std/c_string.h>
#include <memorymanagement.h>

namespace jackos {
    namespace filesystem {
        struct initrd_header {
            jackos::common::uint32_t nfiles; // Number of files
        };

        struct initrd_file_header {
            jackos::common::uint8_t magic;
            char name[64];
            jackos::common::uint32_t offset;
            jackos::common::uint32_t length;
        };

        class initrd {
            private:
                initrd_header* initrd_h;
                initrd_file_header* file_headers; // List of file headers
                fs_node* initrd_root; // Root directory node
                fs_node* initrd_dev; // /dev
                fs_node* root_nodes; // List of file nodes
                int nroot_nodes; // Number of file nodes

            public:
                initrd();
                ~initrd();
                fs_node* initialize_initrd(jackos::common::uint32_t location);
                jackos::common::uint32_t initrd_read(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
                dirent* initrd_readdir(fs_node* node, jackos::common::uint32_t index);
                fs_node* initrd_finddir(fs_node* node, char* name);
        };
    }
}

#endif