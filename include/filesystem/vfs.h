#ifndef __jackos_filesystem_vfs_H
#define __jackos_filesystem_vfs_H

#include <common/types.h>

#define FS_FILE         0x01
#define FS_DIRECTORY    0x02
#define FS_CHARDEVICE   0x03
#define FS_BLOCKDEVICE  0x04
#define FS_PIPE         0x05
#define FS_SYMLINK      0x06
#define FS_MOUNTPOINT   0x08

namespace jackos {
    namespace filesystem {
        struct fs_node {
            char name[128];
            jackos::common::uint32_t mask; // Permissions mask
            jackos::common::uint32_t uid; // The owning user
            jackos::common::uint32_t gid; // The owning group
            jackos::common::uint32_t flags; // Including filetype
            jackos::common::uint32_t inode; // Device-specific
            jackos::common::uint32_t length; // Filesize, in bytes
            /* These function pointers are to be defined later.
            They'll rely on the specific filesystem.
            */
            jackos::common::uint32_t impl; // Implementation-defined
            void* (*open)(fs_node* node, jackos::common::uint8_t read, jackos::common::uint8_t write);
            void* (*close)(fs_node* node);
            jackos::common::uint32_t (*read)(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
            jackos::common::uint32_t (*write)(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
            dirent* (*readdir)(fs_node* node, jackos::common::uint32_t index);
            fs_node* (*finddir)(fs_node* node, char* name);
            fs_node* ptr; // Used by mountpoints and symlinks (shortcuts)
        };

        extern fs_node* fs_root;

        struct dirent { // To be returned by readdir
            char name[128];
            jackos::common::uint32_t inode; // Required by POSIX compliance.
        };

        class VFS {
            private:

            public:
                jackos::common::uint32_t read_fs(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
                jackos::common::uint32_t write_fs(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
                void* open_fs(fs_node* node, jackos::common::uint8_t read, jackos::common::uint8_t write);  
                void* close_fs(fs_node* node);
                dirent* readdir_fs(fs_node* node, jackos::common::uint32_t index);
                fs_node* finddir_fs(fs_node* node, char* name);
        };
    }
}

#endif