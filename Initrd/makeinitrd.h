#ifndef __makeelfinitrd_H
#define __makeelfinitrd_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define EI_NIDENT 16

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf_Ehdr;

struct initrd_header {
    unsigned char magic; // The magic number, for checking consistency
    char name[64];
    unsigned int offset; // Offset in the initrd where the file starts
    unsigned int length; // The length of the file
};

#endif