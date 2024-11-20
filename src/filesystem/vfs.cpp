#include <filesystem/vfs.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::filesystem;

fs_node* fs_root = 0; // The filesystem root

uint32_t VFS::read_fs(fs_node* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if(node -> read != 0) {
        return node -> read(node, offset, size, buffer);
    }
    return 0;
}

uint32_t VFS::write_fs(fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer) {
    if(node -> write != 0) {
        return node -> write(node, offset, size, buffer);
    }
    return 0;
}

void* VFS::open_fs(fs_node* node, jackos::common::uint8_t read, jackos::common::uint8_t write) {
    if(node -> open != 0) {
        return node -> open(node, read, write);
    }
    return 0;
}

void* VFS::close_fs(fs_node* node) {
    if(node -> close != 0) {
        return node -> close(node);
    }
    return 0;
}

fs_node VFS::volOpenFile(const char* filename) {
    if(filename) {
        char device = 'a'; // Default to a: drive
        char* fname = (char*)filename;
        if(filename[1] == ':') {
            device = filename[0];
            fname += 2; // Strip out the a:
        }
        if(file_systems[device - 'a']) {
            fs_node file = file_systems[device - 'a'] -> Open(fname);
            file.device = device;
            return file;
        }
    }
    fs_node file;
    file.flags = FS_INVALID;
    return file;
}

void VFS::volRegisterFileSystem(file_system* fs, jackos::common::uint32_t deviceID) {
    if(deviceID < 0 || deviceID > DEVICE_MAX) {
        return;
    }
    if(fs) {
        file_systems[deviceID] = fs;
    }
}