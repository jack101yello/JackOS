<<<<<<< Updated upstream
<<<<<<< Updated upstream:include/filesystem/elfheader.h
#ifndef __jackos_filesystem_elfheader_H
#define __jackos_filesystem_elfheader_H

#include <common/types.h>

#define EI_NIDENT 16

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    jackos::common::uint16_t e_type;
    jackos::common::uint16_t e_machine;
    jackos::common::uint32_t e_version;
    jackos::common::uint64_t e_entry;
    jackos::common::uint64_t e_phoff;
    jackos::common::uint64_t e_shoff;
    jackos::common::uint32_t e_flags;
    jackos::common::uint16_t e_ehsize;
    jackos::common::uint16_t e_phentsize;
    jackos::common::uint16_t e_phnum;
    jackos::common::uint16_t e_shentsize;
    jackos::common::uint16_t e_shnum;
    jackos::common::uint16_t e_shstrndx;
} Elf_Ehdr;


=======
=======
>>>>>>> Stashed changes
#ifndef __jackos_filesystem_elfheader_H
#define __jackos_filesystem_elfheader_H

#include <common/types.h>

#define EI_NIDENT 16

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    jackos::common::uint16_t e_type;
    jackos::common::uint16_t e_machine;
    jackos::common::uint32_t e_version;
    jackos::common::uint32_t e_entry;
    jackos::common::uint32_t e_phoff;
    jackos::common::uint32_t e_shoff;
    jackos::common::uint32_t e_flags;
    jackos::common::uint16_t e_ehsize;
    jackos::common::uint16_t e_phentsize;
    jackos::common::uint16_t e_phnum;
    jackos::common::uint16_t e_shentsize;
    jackos::common::uint16_t e_shnum;
    jackos::common::uint16_t e_shstrndx;
} Elf_Ehdr;

typedef struct {
    jackos::common::uint32_t type;
    jackos::common::uint32_t offset;
    jackos::common::uint32_t vaddr;
    jackos::common::uint32_t paddr;
    jackos::common::uint32_t filesz;
    jackos::common::uint32_t memsz;
    jackos::common::uint32_t flags;
    jackos::common::uint32_t align;
} elf_phdr;

<<<<<<< Updated upstream
>>>>>>> Stashed changes:include/filesystem/ELF/elfheader.h
=======
>>>>>>> Stashed changes
#endif