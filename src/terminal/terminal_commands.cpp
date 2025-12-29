#include <terminal/terminal.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::terminal;

extern void printf(const char* msg);
extern void clear_screen();

void Terminal::command_help(const char* argument) {
    if(jackos::libc::strcmp(argument, "") == 0) {
        printf("Valid commands:\n");
        printf("help    clear");
        printf("\nAlso use \"help + command\"\n");
    }
    else if(jackos::libc::strcmp(argument, "help") == 0) {
        printf("You think you're smart, huh?\n");
    }
    else if(jackos::libc::strcmp(argument, "clear") == 0) {
        printf("Clears the screen.\n");
    }
    else if(jackos::libc::strcmp(argument, "list") == 0) {
        printf("Lists the ELF modules in memory.\n");
    }
    else if(jackos::libc::strcmp(argument, "run") == 0) {
        printf("Runs the first ELF file in memory.\n");
    }
    else {
        printf("Unknown command: ");
        printf(argument);
        printf("\n\n");
    }
}

// Clear the screen
void Terminal::command_clear() {
    clear_screen();
}

// List the ELF modules in memory
void Terminal::list_files() {
    multiboot_module_t* elf_modules = (multiboot_module_t*) mb -> mods_addr;
    for(int i = 0; i < mb -> mods_count; i++) {
        printf("\t");
        printf((const char*)elf_modules[i].string);
        printf("\n");
    }
}

// Run an ELF executable
void Terminal::run_file() {
    multiboot_module_t* elf_modules = (multiboot_module_t*) mb -> mods_addr;
    jackos::filesystem::elf::Elf_File elf_program((Elf_Ehdr*)elf_modules[0].mod_start);
    if(!elf_program.check_file()) return;
    elf_program.header_dump();
    elf_program.phdr_dump();
    elf_program.run();
}