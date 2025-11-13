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
    return (elf_phdr*)((char*)header + header -> e_phoff);
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
    printf("IDTR Base:");
    printaddr(idtr.base);
    printf(" IDTR Limit:");
    printaddr(idtr.limit);
    printf("\n");
    uint32_t eflags;
    asm volatile("pushf; pop %0" : "=r"(eflags));
    printf("EFLAGS: ");
    printaddr(eflags);
    for(int i = 0; i < header -> e_phnum; i++) {
        elf_phdr* ph = get_phdr(i);
        if(ph -> type != 1) continue;
        memcpy((uint8_t*)ph->vaddr, (uint8_t*)header + ph->offset, ph->filesz);
        memset((uint8_t*)(ph->vaddr + ph->filesz), 0, ph->memsz - ph->filesz);
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
    printf("IDTR Base:");
    printaddr(idtr.base);
    printf(" IDTR Limit:");
    printaddr(idtr.limit);
    printf("\n");
    uint16_t cs, ds, ss;
    asm volatile("mov %%cs, %0" : "=r"(cs));
    asm volatile("mov %%ds, %0" : "=r"(ds));
    asm volatile("mov %%ss, %0" : "=r"(ss));
    printf("CS: ");
    printfhex(cs);
    printf(" DS: ");
    printfhex(ds);
    printf(" SS: ");
    printfhex(ss);
    asm volatile("pushf; pop %0" : "=r"(eflags));
    printf("EFLAGS: ");
    printaddr(eflags);
}

void Elf_File::entry_dump() {
    printf("Entry dump: ");
    for(int i = 0; i < 16; i++) {
        printfhex(*(uint8_t*)(header -> e_entry + i));
    }
    printf("\n");
}