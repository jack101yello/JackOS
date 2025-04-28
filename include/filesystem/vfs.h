#ifndef __jackos_filesystem_vfs_H
#define __jackos_filesystem_vfs_H

#include <common/types.h>

#define FS_FILE 0x01
#define FS_DIRECTORY 0X02
#define FS_CHARDEVICE 0X03
#define FS_BLOCKDEVICE 0X04
#define FS_PIPE 0X05
#define FS_SYMLINK 0X06
#define FS_MOUNTPOINT 0X08

namespace jackos {
    namespace filesystem {

        typedef jackos::common::uint32_t (*read_type_t)(struct fs_node*, jackos::common::uint32_t, jackos::common::uint32_t, jackos::common::uint8_t*);
        typedef jackos::common::uint32_t (*write_type_t)(struct fs_node*, jackos::common::uint32_t, jackos::common::uint32_t, jackos::common::uint8_t*);
        typedef void (*open_type_t)(struct fs_node*);
        typedef void (*close_type_t)(struct fs_node*);
        typedef struct dirent * (*readdir_type_t)(struct fs_node*, jackos::common::uint32_t);
        typedef struct fs_node * (*finddir_type_t)(struct fs_node*, char* name);

        struct dirent {
            char name[128]; // filename
            jackos::common::uint32_t ino; // Inode number, as required by POSIX
        };

        typedef struct fs_node {
            char name[128];
            jackos::common::uint32_t mask; // The permissions mask
            jackos::common::uint32_t uid; // The owning user
            jackos::common::uint32_t gid; // The owning group
            jackos::common::uint32_t flags;
            jackos::common::uint32_t inode;
            jackos::common::uint32_t length; // The size of the file (bytes)
            jackos::common::uint32_t impl;
            read_type_t read;
            write_type_t write;
            open_type_t open;
            close_type_t close;
            readdir_type_t readdir;
            finddir_type_t finddir;
            struct fs_node* ptr; // Used by mountpoints and symbolic links/shortcuts
        } fs_node_t;

        extern fs_node_t *fs_root; // The filesystem root
        jackos::common::uint32_t read_fs(fs_node_t* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
        jackos::common::uint32_t write_fs(fs_node_t* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
        void open_fs(fs_node_t* node, jackos::common::uint8_t read, jackos::common::uint8_t write);
        void close_fs(fs_node_t* node);
        struct dirent *readdir_fs(fs_node_t* node, jackos::common::uint32_t index);
        fs_node_t* finddir_fs(fs_node_t* node, char *name);
    }
}

#endif