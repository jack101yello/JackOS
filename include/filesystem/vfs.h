#ifndef __jackos_filesystem_vfs_H
#define __jackos_filesystem_vfs_H

#include <common/types.h>

#define FS_FILE         0x00
#define FS_DIRECTORY    0x01
#define FS_INVALID      0x02

#define DEVICE_MAX 26 // We use letter assignments, like in Windows

namespace jackos {
    namespace filesystem {
        struct fs_node { // Files and directories
            char name[128];
            jackos::common::uint32_t flags; // Including filetype
            jackos::common::uint32_t length; // Filesize, in bytes
            jackos::common::uint32_t id;
            jackos::common::uint32_t eof;
            jackos::common::uint32_t position;
            jackos::common::uint32_t currentCluster;
            jackos::common::uint32_t device;
            // These functions are implemented by the various filesystems.
            void* (*open)(fs_node* node, jackos::common::uint8_t read, jackos::common::uint8_t write);
            void* (*close)(fs_node* node);
            jackos::common::uint32_t (*read)(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
            jackos::common::uint32_t (*write)(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
        };

        struct file_system {
            char name[8];
            fs_node (*Directory) (const char* DirectoryName);
            void (*Mount) ();
            void (*Read) (fs_node* file, jackos::common::uint8_t* buffer, jackos::common::uint32_t len);
            void (*Close) (fs_node* file);
            fs_node (*Open) (const char* filename);
        };

        class VFS {
            private:
                file_system *file_systems[DEVICE_MAX];

            public:
                jackos::common::uint32_t read_fs(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
                jackos::common::uint32_t write_fs(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);
                void* open_fs(fs_node* node, jackos::common::uint8_t read, jackos::common::uint8_t write);  
                void* close_fs(fs_node* node);
                fs_node volOpenFile(const char* filename);
                void volReadFile(fs_node* file, jackos::common::uint8_t* buffer, jackos::common::uint32_t length);
                void volCloseFile(fs_node* file);
                void volRegisterFileSystem(file_system* fs, jackos::common::uint32_t deviceID);
                void volUnregisterFileSystem(file_system* fs);
                void volUnregisterFileSystem(jackos::common::uint32_t deviceID);
        };
    }
}

#endif