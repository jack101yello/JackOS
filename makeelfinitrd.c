#include "makeelfinitrd.h"

int main(char argc, char **argv) {
    Elf_Ehdr elf_header;
    FILE *file = fopen("elfinitrd.elf", "w");
    if(!file) {
        perror("fopen");
        return 1;
    }

    memset(&elf_header, 0, sizeof(elf_header)); // Zero out the header

    // Elf magic number and class
    elf_header.e_ident[0] = 0x7f;
    elf_header.e_ident[1] = 'E';
    elf_header.e_ident[2] = 'L';
    elf_header.e_ident[3] = 'F';
    elf_header.e_ident[4] = 1; // 32-bit
    elf_header.e_ident[5] = 1; // Little-endian
    elf_header.e_ident[6] = 1; // Elf version (original)

    // Header values
    elf_header.e_type = 2; // Executable file
    elf_header.e_machine = 3; // x86
    elf_header.e_version = 1;
    elf_header.e_ehsize = sizeof(Elf_Ehdr);

    fwrite(&elf_header, 1, sizeof(elf_header), file); // Write elf header to file

    int nheaders = (argc-1)/2;
    struct initrd_header headers[64];
    printf("Size of header: %lu\n", sizeof(struct initrd_header));
    unsigned int off = sizeof(struct initrd_header) * 64 + sizeof(int); // This should be recomputed to account for the ELF header
    for(int i = 0; i < nheaders; i++) {
        printf("writing file %s->%s at 0x%x\n", argv[i*2+1], argv[i*2+2], off);
        strcpy(headers[i].name, argv[i*2+2]);
        headers[i].offset = off;
        FILE *stream = fopen(argv[i*2+2], "r");
        if(stream == 0) {
            printf("Error: file not found %s\n", argv[i*2+1]);
            return 2;
        }
        fseek(stream, 0, SEEK_END);
        headers[i].length = ftell(stream);
        off += headers[i].length;
        fclose(stream);
        headers[i].magic = 0xbf;
    }

    /* As it stands, this will overwrite the elf header, causing GRUB to reject it.
    This should be modified to come after the elf header, and then afterwards, the
    code which reads the file will need to take the elf header into account. */
    unsigned char *data = (unsigned char *)malloc(off);
    fwrite(&nheaders, sizeof(int), 1, file);
    fwrite(headers, sizeof(struct initrd_header), 64, file);

    for(int i = 0; i < nheaders; i++) {
        FILE *stream = fopen(argv[i*2+1], "r");
        unsigned char *buf = (unsigned char *)malloc(headers[i].length);
        fread(buf, 1, headers[i].length, stream);
        fwrite(buf, 1, headers[i].length, file);
        fclose(stream);
        free(buf);
    }

    fclose(file);
    free(data);

    return 0;
}