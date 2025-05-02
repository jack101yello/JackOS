#include <filesystem/vfs.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::filesystem;

fs_node_t* jackos::filesystem::fs_root = 0; // The root of the filesystem

uint32_t jackos::filesystem::read_fs(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if(node -> read != 0) {
        return node -> read(node, offset, size, buffer);
    }
    else {
        return 0;
    }
}

uint32_t jackos::filesystem::write_fs(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if(node -> write != 0) {
        return node -> write(node, offset, size, buffer);
    }
    else {
        return 0;
    }
}

void jackos::filesystem::open_fs(fs_node_t* node, uint8_t read, uint8_t write) {
    if(node -> open != 0) {
        node -> open(node);
    }
}

void jackos::filesystem::close_fs(fs_node_t* node) {
    if(node -> close != 0) {
        node -> close(node);
    }
}

struct dirent* jackos::filesystem::readdir_fs(fs_node_t* node, uint32_t index) {
    if((node -> flags & 0x7) == FS_DIRECTORY && node -> readdir != 0) {
        return node -> readdir(node, index);
    }
    else {
        return nullptr;
    }
}

fs_node_t* jackos::filesystem::finddir_fs(fs_node_t* node, char* name) {
    if((node -> flags & 0x7) == FS_DIRECTORY && node -> finddir != 0) {
        return node -> finddir(node, name);
    }
    else {
        return nullptr;
    }
}