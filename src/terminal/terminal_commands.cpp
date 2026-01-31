#include <terminal/terminal.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::terminal;
using namespace jackos::drivers;

extern void printf(const char* msg);
extern void clear_screen();

void Terminal::command_help(const char* argument) {
    if(jackos::libc::strcmp(argument, "") == 0) {
        printf("Valid commands:\n");
        printf("help    clear\n");
        printf("list    run\n");
        printf("Also use \"help + command\"\n");
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
        printf("\n");
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

extern "C" void enter_usermode();

// Run an ELF executable
void Terminal::run_file() {
    multiboot_module_t* elf_modules = (multiboot_module_t*) mb -> mods_addr;
    jackos::filesystem::elf::Elf_File elf_program((Elf_Ehdr*)elf_modules[0].mod_start, gdt);
    if(!elf_program.check_file()) return;
    active = false;
    elf_program.run();
}

void Terminal::drun_file(const char* filename) {
    printf("Loading...\n");

    floppy_driver -> motor(FLOPPY_BASE, MOTOR_ON);
    floppy_driver -> dma_init(DIR_READ);
    floppy_driver -> do_track(FLOPPY_BASE, 0, DIR_READ);
    floppy_driver -> ParseFATHeader();
    floppy_driver -> read_root_directory();
    system_clock -> wait(1);
    jackos::filesystem::FAT12DirectoryEntry file;
    bool file_found = false;
    for(int i = 0; i < floppy_driver->get_file_count(); i++) {
        file = floppy_driver->get_file(i);
        char* tmp_filename = file.name;
        tmp_filename[8] = '\0';
        for(int i = 0; i < 8; i++) {
            if(tmp_filename[i] == ' ') {
                tmp_filename[i] = '\0';
                break;
            }
        }
        printf(tmp_filename);
        if(jackos::libc::strcmp(filename, tmp_filename) == 0) {
            file_found = true;
            printf("File found: size=");
            printaddr(file.fileSize);
            printf(" start cluster=");
            printaddr(file.firstCluster);
            printf("\n");
            break;
        }
    }
    if(!file_found) {
        printf("File not found.\n");
        return;
    }

    uint8_t* file_ptr = (uint8_t*)0x10000;
    floppy_driver -> load_file(file.firstCluster, file.fileSize, file_ptr);
    floppy_driver -> motor(FLOPPY_BASE, MOTOR_OFF);
    jackos::filesystem::elf::Elf_File loaded_program((Elf_Ehdr*)file_ptr, gdt);
    loaded_program.run();
}