<<<<<<< Updated upstream
#include <filesystem/initrd.h>
#include <common/common.h>
#include <filesystem/elfheader.h>

extern void printf(const char* msg);
extern void printfhex(int hex);

using namespace jackos;
using namespace jackos::common;
using namespace jackos::filesystem;

jackos::filesystem::initrd_header_t* initrd_header; // The header
jackos::filesystem::initrd_file_header_t* file_headers; // The list of file headers
jackos::filesystem::fs_node_t* initrd_root; // The root directory node
jackos::filesystem::fs_node_t* initrd_dev; // Directory node for /dev
jackos::filesystem::fs_node_t* root_nodes; // List of file nodes
int nroot_nodes; // The number of file nodes
jackos::filesystem::dirent dirent_t;

static uint32_t initrd_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    initrd_file_header_t header = file_headers[node -> inode];
    if(offset > header.length) {
        return 0;
    }
    if(offset + size > header.length) {
        size = header.length - offset;
    }
    jackos::common::memcpy(buffer, (uint8_t*)(header.offset+offset+sizeof(Elf_Ehdr)), size);
    return size;
}

static struct dirent *initrd_readdir(fs_node_t* node, uint32_t index) {
    if(node == initrd_root && index == 0) {
        char foo[] = {'d', 'e', 'v', '\0'};
        jackos::libc::strcpy(dirent_t.name, foo);
        dirent_t.name[3] = '\0'; // Make sure we null-terminate
        dirent_t.ino = 0;
        return &dirent_t;
    }

    if(index - 1 >= nroot_nodes) {
        return 0;
    }

    jackos::libc::strcpy(dirent_t.name, root_nodes[index-1].name);
    dirent_t.name[jackos::libc::strlen(root_nodes[index-1].name)] = '\0'; // Null-terminate
    dirent_t.ino = root_nodes[index-1].inode;
    return &dirent_t;
}

static fs_node_t* initrd_finddir(fs_node_t* node, char *name) {
    if(node == initrd_root && !jackos::libc::strcmp(name, "dev")) {
        return initrd_dev;
    }
    for(int i = 0; i < nroot_nodes; i++) {
        if(!jackos::libc::strcmp(name, root_nodes[i].name)) {
            return &root_nodes[i];
        }
    }
    return 0;
}

fs_node_t *jackos::filesystem::initialize_initrd(uint32_t location) {
    // Initialize the main and file header pointers and populate the root directory
    initrd_header = (initrd_header_t *)(location+sizeof(Elf_Ehdr)); // We need to shift by the size of the elf header
    file_headers = (initrd_file_header_t *) (location + sizeof(initrd_header_t) + sizeof(Elf_Ehdr));

    // Initialize the root directory
    initrd_root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    char foo[] = {'i', 'n', 'i', 't', 'r', 'd', '\0'};
    jackos::libc::strcpy(initrd_root->name, foo);
    initrd_root->mask=initrd_root->uid=initrd_root->gid=initrd_root->inode=initrd_root->length = 0;
    initrd_root -> flags = FS_DIRECTORY;
    initrd_root -> read = 0;
    initrd_root -> write = 0;
    initrd_root -> open = 0;
    initrd_root -> close = 0;
    initrd_root -> readdir = &initrd_readdir;
    initrd_root -> finddir = &initrd_finddir;
    initrd_root -> ptr = 0;
    initrd_root -> impl = 0;

    // Initialize /dev
    initrd_dev = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    foo[0] = 'd'; foo[1] = 'e'; foo[2] = 'v'; foo[3] = '\0';
    jackos::libc::strcpy(initrd_dev->name, foo);
    initrd_dev->mask=initrd_dev->uid=initrd_dev->gid=initrd_dev->inode=initrd_dev->length = 0;
    initrd_dev -> flags = FS_DIRECTORY;
    initrd_dev -> read = 0;
    initrd_dev -> write = 0;
    initrd_dev -> open = 0;
    initrd_dev -> close = 0;
    initrd_dev -> readdir = &initrd_readdir;
    initrd_dev -> finddir = &initrd_finddir;
    initrd_dev -> ptr = 0;
    initrd_dev -> impl = 0;

    // Allocate space for files in the ramdisk
    root_nodes = (fs_node_t*) kmalloc(sizeof(fs_node_t) * initrd_header->nfiles);
    nroot_nodes = initrd_header -> nfiles;

    for(int i = 0; i < initrd_header->nfiles; i++) {
        file_headers[i].offset += location;
        /* In James Molloy's tutorial, this is, "&file_headers[i].name", but here that
        throws an error. The cause of this issue is unknown, but it should be noted. */
        jackos::libc::strcpy(root_nodes[i].name, file_headers[i].name);
        root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
        root_nodes[i].length = file_headers[i].length;
        root_nodes[i].inode = i;
        root_nodes[i].flags = FS_FILE;
        root_nodes[i].read = &initrd_read;
        root_nodes[i].write = 0;
        root_nodes[i].readdir = 0;
        root_nodes[i].finddir = 0;
        root_nodes[i].open = 0;
        root_nodes[i].close = 0;
        root_nodes[i].impl = 0;
    }

    return initrd_root;
=======
#include <filesystem/initrd.h>
#include <common/common.h>
#include <filesystem/ELF/elfheader.h>

extern void printf(const char* msg);
extern void printfhex(int hex);

using namespace jackos;
using namespace jackos::common;
using namespace jackos::filesystem;

jackos::filesystem::initrd_header_t* initrd_header; // The header
jackos::filesystem::initrd_file_header_t* file_headers; // The list of file headers
jackos::filesystem::fs_node_t* initrd_root; // The root directory node
jackos::filesystem::fs_node_t* initrd_dev; // Directory node for /dev
jackos::filesystem::fs_node_t* root_nodes; // List of file nodes
int nroot_nodes; // The number of file nodes
jackos::filesystem::dirent dirent_t;

static uint32_t initrd_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    initrd_file_header_t header = file_headers[node -> inode];
    if(offset > header.length) {
        return 0;
    }
    if(offset + size > header.length) {
        size = header.length - offset;
    }
    jackos::common::memcpy(buffer, (uint8_t*)(header.offset+offset+sizeof(Elf_Ehdr)), size);
    return size;
}

static struct dirent *initrd_readdir(fs_node_t* node, uint32_t index) {
    if(node == initrd_root && index == 0) {
        char foo[] = {'d', 'e', 'v', '\0'};
        jackos::libc::strcpy(dirent_t.name, foo);
        dirent_t.name[3] = '\0'; // Make sure we null-terminate
        dirent_t.ino = 0;
        return &dirent_t;
    }

    if(index - 1 >= nroot_nodes) {
        return 0;
    }

    jackos::libc::strcpy(dirent_t.name, root_nodes[index-1].name);
    dirent_t.name[jackos::libc::strlen(root_nodes[index-1].name)] = '\0'; // Null-terminate
    dirent_t.ino = root_nodes[index-1].inode;
    return &dirent_t;
}

static fs_node_t* initrd_finddir(fs_node_t* node, char *name) {
    if(node == initrd_root && !jackos::libc::strcmp(name, "dev")) {
        return initrd_dev;
    }
    for(int i = 0; i < nroot_nodes; i++) {
        if(!jackos::libc::strcmp(name, root_nodes[i].name)) {
            return &root_nodes[i];
        }
    }
    return 0;
}

fs_node_t *jackos::filesystem::initialize_initrd(uint32_t location) {
    // Initialize the main and file header pointers and populate the root directory
    initrd_header = (initrd_header_t *)(location+sizeof(Elf_Ehdr)); // We need to shift by the size of the elf header
    file_headers = (initrd_file_header_t *) (location + sizeof(initrd_header_t) + sizeof(Elf_Ehdr));

    // Initialize the root directory
    initrd_root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    char foo[] = {'i', 'n', 'i', 't', 'r', 'd', '\0'};
    jackos::libc::strcpy(initrd_root->name, foo);
    initrd_root->mask=initrd_root->uid=initrd_root->gid=initrd_root->inode=initrd_root->length = 0;
    initrd_root -> flags = FS_DIRECTORY;
    initrd_root -> read = 0;
    initrd_root -> write = 0;
    initrd_root -> open = 0;
    initrd_root -> close = 0;
    initrd_root -> readdir = &initrd_readdir;
    initrd_root -> finddir = &initrd_finddir;
    initrd_root -> ptr = 0;
    initrd_root -> impl = 0;

    // Initialize /dev
    initrd_dev = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    foo[0] = 'd'; foo[1] = 'e'; foo[2] = 'v'; foo[3] = '\0';
    jackos::libc::strcpy(initrd_dev->name, foo);
    initrd_dev->mask=initrd_dev->uid=initrd_dev->gid=initrd_dev->inode=initrd_dev->length = 0;
    initrd_dev -> flags = FS_DIRECTORY;
    initrd_dev -> read = 0;
    initrd_dev -> write = 0;
    initrd_dev -> open = 0;
    initrd_dev -> close = 0;
    initrd_dev -> readdir = &initrd_readdir;
    initrd_dev -> finddir = &initrd_finddir;
    initrd_dev -> ptr = 0;
    initrd_dev -> impl = 0;

    // Allocate space for files in the ramdisk
    root_nodes = (fs_node_t*) kmalloc(sizeof(fs_node_t) * initrd_header->nfiles);
    nroot_nodes = initrd_header -> nfiles;

    for(int i = 0; i < initrd_header->nfiles; i++) {
        file_headers[i].offset += location;
        /* In James Molloy's tutorial, this is, "&file_headers[i].name", but here that
        throws an error. The cause of this issue is unknown, but it should be noted. */
        jackos::libc::strcpy(root_nodes[i].name, file_headers[i].name);
        root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
        root_nodes[i].length = file_headers[i].length;
        root_nodes[i].inode = i;
        root_nodes[i].flags = FS_FILE;
        root_nodes[i].read = &initrd_read;
        root_nodes[i].write = 0;
        root_nodes[i].readdir = 0;
        root_nodes[i].finddir = 0;
        root_nodes[i].open = 0;
        root_nodes[i].close = 0;
        root_nodes[i].impl = 0;
    }

    return initrd_root;
>>>>>>> Stashed changes
}