<<<<<<< Updated upstream
#ifndef __jackos_filesystem_elf_elfloader_H
#define __jackos_filesystem_elf_elfloader_H

#include <common/types.h>
#include <filesystem/ELF/elfheader.h>
#include <common/common.h>

extern void printf(const char* msg);
extern void printfhex(int hex);
extern void printaddr(int addr);

namespace jackos {
    namespace filesystem {
        namespace elf {
            class Elf_File {
                private:
                    Elf_Ehdr* header;
                    void entry_dump();

                public:
                    Elf_File(Elf_Ehdr* i_header);
                    bool check_file();
                    void header_dump();
                    elf_phdr* get_phdr(long unsigned int index);
                    void phdr_dump();
                    void phdr_dump(long unsigned int index);
                    void run();
            };
        }
    }
}

=======
#ifndef __jackos_filesystem_elf_elfloader_H
#define __jackos_filesystem_elf_elfloader_H

#include <common/types.h>
#include <filesystem/ELF/elfheader.h>
#include <common/common.h>

extern void printf(const char* msg);
extern void printfhex(int hex);
extern void printaddr(int addr);

namespace jackos {
    namespace filesystem {
        namespace elf {
            class Elf_File {
                private:
                    Elf_Ehdr* header;
                    void entry_dump();

                public:
                    Elf_File(Elf_Ehdr* i_header);
                    bool check_file();
                    void header_dump();
                    elf_phdr* get_phdr(long unsigned int index);
                    void phdr_dump();
                    void phdr_dump(long unsigned int index);
                    void run();
            };
        }
    }
}

>>>>>>> Stashed changes
#endif