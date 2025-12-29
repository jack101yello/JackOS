#include <filesystem/ELF/elfloader.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::filesystem;
using namespace jackos::filesystem::elf;

Elf_File::Elf_File(Elf_Ehdr* i_header) {
    header = i_header;
}

bool Elf_File::check_file() {
    if(!header) return false;
    if(header -> e_ident[0] != 0x7F || header -> e_ident[1] != 'E' || header -> e_ident[2] != 'L' || header -> e_ident[3] != 'F') {
        return false; // Invalid magic number
    }
    return true;
}

void Elf_File::header_dump() {
    printf("ELF Header Dump:");
    printf("\n\te_ident: ");
    for(int i = 0; i < EI_NIDENT; i++) {
        printfhex(header -> e_ident[i]);
    }
    printf("\n\te_entry: ");
    printaddr(header -> e_entry);
    printf("\n\te_phoff: ");
    printaddr(header -> e_phoff);
    printf("\n\te_phnum: ");
    printfhex(header -> e_phnum);
    printf("\n");
}

elf_phdr* Elf_File::get_phdr(long unsigned int index) {
    if(index >= header -> e_phnum) return nullptr;
    return (elf_phdr*)((char*)header + header -> e_phoff + index * sizeof(elf_phdr));
}

void Elf_File::phdr_dump(long unsigned int index) {
    if(index >= header -> e_phnum) return;
    elf_phdr* ph = get_phdr(index);
    printf("PHDR Index: ");
    printfhex(index);
    printf("\n\toffset: ");
    printaddr(ph -> offset);
    printf("\n\tvaddr: ");
    printaddr(ph -> vaddr);
    printf("\n\tfilesz: ");
    printaddr(ph -> filesz);
    printf("\n\tmemsz: ");
    printaddr(ph -> memsz);
    printf("\n");
}

void Elf_File::phdr_dump() {
    for(int i = 0; i < header -> e_phnum; i++) {
        phdr_dump(i);
    }
}

struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr;

void Elf_File::run() {
    printf("Running ELF.\n");
    asm volatile("sidt %0" : "=m"(idtr));
    uint32_t eflags;
    asm volatile(
        "pushf\n\t"
        "pop %0"
        : "=r"(eflags)
    );
    printf("\n");
    for(int i = 0; i < header -> e_phnum; i++) {
        elf_phdr* ph = get_phdr(i);
        if(ph -> type != 1) continue;
        printf("Source bytes (ELF offset):\n");
        memcpy((uint8_t*)ph->vaddr, (uint8_t*)header + ph->offset, ph->filesz);
        memset((uint8_t*)(ph->vaddr + ph->filesz), 0, ph->memsz - ph->filesz);
        printf("Post-load dump @ vaddr:\n");
        for(int j = 0; j < 80; j++) {
            uint8_t b = *(uint8_t*)(ph->vaddr + j);
            printfhex(b);
        }
        printf("\n");
        printf("Rodata dump:\n");
        for(int j = 0; j < 32; j++) {
            char c = *(char*)(0x002af040 + j);
            if (c >= 32 && c <= 126) {
                const char foo[2] = {c, '\0'};
                printf(foo);
            }
            else {
                printf(".");
            }
        }
        printf("\n");
    }
    typedef void (*entry_point_t)(void);
    entry_point_t entry = (entry_point_t)header->e_entry;
    jackos::hardware::Port8Bit kbport(0x21);
    kbport.Write(kbport.Read() & ~(1 << 1));
    asm("sti");
    uint32_t* new_stack = (uint32_t*)0x007FFFF0;
    asm volatile("mov %0, %%esp" : : "r"(new_stack));
    entry();
    printf("Returning from ELF.\n");
    asm volatile("sidt %0" : "=m"(idtr));
}

void Elf_File::entry_dump() {
    printf("Entry dump: ");
    for(int i = 0; i < 16; i++) {
        printfhex(*(uint8_t*)(header -> e_entry + i));
    }
    printf("\n");
}