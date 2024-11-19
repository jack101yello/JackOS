#include <filesystem/initrd.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::std;
using namespace jackos::filesystem;

initrd::initrd() {

}

initrd::~initrd() {

}

fs_node* initrd::initialize_initrd(uint32_t location) {
    initrd_header* loc = (initrd_header*) location;
    file_headers = (initrd_file_header *)(location + sizeof(initrd_header));

    // Initialize the root directory
    initrd_root = new fs_node;
    strcpy(initrd_root -> name, "initrd");
    initrd_root -> mask = initrd_root -> uid = initrd_root -> gid = initrd_root -> inode = initrd_root -> length = 0;
}

uint32_t initrd::initrd_read(fs_node* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    initrd_file_header header = file_headers[node -> inode];
    if(offset > header.length) {
        return 0;
    }
    if(offset + size > header.length) {
        size = header.length - offset;
    }
    memcpy(buffer, (uint8_t*)(header.offset + offset), size);
    return size;
}

dirent* initrd::initrd_readdir(fs_node* node, uint32_t index) {
    dirent* dir = new dirent;
    if(node == initrd_root && index == 0) {
        strcpy(dir -> name, "dev");
        dir -> name[3] = '\0'; // Ensure termination
        dir -> inode = 0;
        return dir;
    }
    if(index - 1 >= nroot_nodes) {
        return 0;
    }
    strcpy(dir -> name, root_nodes[index-1].name);
    dir -> name[char_array_size(root_nodes[index-1].name)] = '\0';
    dir -> inode = root_nodes[index - 1].inode;
    return dir;
}

fs_node* initrd::initrd_finddir(fs_node* node, char* name) {
    if(node == initrd_root && !cmp(name, "dev")) {
        return initrd_dev;
    }
    for(int i = 0; i < nroot_nodes; i++) {
        if(!cmp(name, root_nodes[i].name)) {
            return &root_nodes[i];
        }
    }
    return 0;
}