#include <stdio.h>
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
} Elf64_Ehdr;

int main() {
    Elf64_Ehdr header;
    FILE *f = fopen("blank.elf", "wb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    // Zero out header
    memset(&header, 0, sizeof(header));

    // Set ELF magic number and class
    header.e_ident[0] = 0x7f;
    header.e_ident[1] = 'E';
    header.e_ident[2] = 'L';
    header.e_ident[3] = 'F';
    header.e_ident[4] = 1; // 32-bit
    header.e_ident[5] = 1; // Little-endian
    header.e_ident[6] = 1; // ELF version (original)

    // Header values
    header.e_type = 2;       // Executable file
    header.e_machine = 3;   // x86
    header.e_version = 1;
    header.e_ehsize = sizeof(Elf64_Ehdr);

    // Write header to file
    fwrite(&header, 1, sizeof(header), f);

    fclose(f);
    printf("Minimal blank ELF file 'blank.elf' created.\n");
    return 0;
}
