#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardware/interrupts.h>
#include <syscalls.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/pit.h>
#include <hardware/pci.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>
#include <multiboot.h>
#include <filesystem/vfs.h>
#include <filesystem/initrd.h>
#include <libc/libc.h>
#include <filesystem/ELF/elfloader.h>
#include <common/common.h>
#include <terminal/terminal.h>
#include <drivers/floppy.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::drivers;
using namespace jackos::hardware;
using namespace jackos::gui;
using namespace jackos::filesystem;

void printf(const char* str) {
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x = 0, y = 0;

    for(int i = 0; str[i] != '\0'; ++i) {
        switch(str[i]) {
            case '\n':
                y++;
                x = 0;
                break;
            case '\0':
                return;
            default:
                VideoMemory[80*y + x] = (VideoMemory[80*y + x] & 0xFF00) | str[i];
                x++;
                break;
        }

        if(x >= 80) { // Newline
            y++;
            x = 0;
        }
        if(y >= 25) { // Scrolling
            for(y = 0; y < 25; y++) {
                for(x = 0; x < 80; x++) {
                    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                }
            }
            x = 0;
            y = 0;
        }
    }
}

void clear_screen() {
    for(int i = 0; i < 25; i++) {
        for(int j = 0; j < 80; j++) {
            printf(" ");
        }
        printf("\n");
    }
}

void printfhex(int val) {
    char msg[] = "00";
    char hex[] = "0123456789ABCDEF";
    msg[0] = hex[(val >> 4) & 0x0F];
    msg[1] = hex[val & 0x0F];
    printf(msg);
}

void printaddr(int addr) {
    char msg[] = "0x00000000";
    char hex[] = "0123456789ABCDEF";
    for(int i = 2; i <= 9; i++) {
        msg[i] = hex[(addr >> (36-(4*i))) & 0x0F];
    }
    printf(msg);
}

class MouseToConsole : public MouseEventHandler {
    int8_t x, y;
    public:
        MouseToConsole() {
            uint16_t* VideoMemory = (uint16_t*)0xb8000;
            x = 40;
            y = 12;
            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) | ((VideoMemory[80*y+x] & 0x0F00) << 4) | ((VideoMemory[80*y+x] & 0x00FF));
        }
        void OnMouseMove(int xoffset, int yoffset) {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;
            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) | ((VideoMemory[80*y+x] & 0x0F00) << 4) | ((VideoMemory[80*y+x] & 0x00FF));
            x += xoffset;
            if(x < 0) x = 0;
            if(x >= 80) x = 79;
            y += yoffset;
            if(y < 0) y = 0;
            if(y >= 25) y = 24;
            VideoMemory[80*y+x] = ((VideoMemory[80*y+x] & 0xF000) >> 4) | ((VideoMemory[80*y+x] & 0x0F00) << 4) | ((VideoMemory[80*y+x] & 0x00FF));
        }
};

MemoryManager* kmm;

void sysprintf(const char* str) {
    asm("int $0x80" : : "a" (4), "b" (str));
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors() {
    for(constructor* i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

extern "C" void* heap;

// #define GRAPHICS_MODE

void breakpoint() {
    printf("");
}

extern "C" void kernel_main(struct multiboot* multiboot_structure, uint32_t magicnumber) {
    clear_screen();

    printf("Initializing JackOS Kernel\n");

    printf("Setting up Global Descriptor Table (GDT).\n");
    GlobalDescriptorTable gdt;

    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);
    kmm = &memoryManager; // Set the global memory manager to this memory manager, so that everyone can call kmalloc
    // printf("Heap: 0x");
    // printfhex((heap >> 24) & 0xFF);
    // printfhex((heap >> 16) & 0xFF);
    // printfhex((heap >>  8) & 0xFF);
    // printfhex((heap      ) & 0xFF);
    void* allocated = memoryManager.malloc(1024);
    // printf("\nAllocated: 0x");
    // printfhex(((size_t)allocated >> 24) & 0xFF);
    // printfhex(((size_t)allocated >> 16) & 0xFF);
    // printfhex(((size_t)allocated >>  8) & 0xFF);
    // printfhex(((size_t)allocated      ) & 0xFF);
    // printf("\n");

    TaskManager taskManager;
    
    printf("Setting up Interrupt Descriptor table (IDT).\n");
    InterruptManager interrupts(0x20, &gdt, &taskManager);
    
    printf("Setting up syscalls.\n");
    SyscallHandler syscalls(&interrupts, 0x80);


    DriverManager drvManager;
    #ifdef GRAPHICS_MODE
    printf("Initializing graphics.\n");
    VideoGraphicsArray vga;

    printf("Instantiating Desktop.\n");
    Desktop desktop(SCREEN_WIDTH, SCREEN_HEIGHT, 0x00, 0x00, 0x00);

    #else
    printf("Setting up drivers...\n");

    printf("\tInitiating mouse.\n");
    MouseToConsole mhandler;
    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);
    drvManager.AddDriver(&mouse);
    printf("\tInitiating keyboard.\n");
    KeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);
    drvManager.AddDriver(&keyboard);
    #endif

    printf("\tInitiating PIT.\n");
    PITEventHandler system_clock;
    PITDriver pit_driver(&interrupts, &system_clock);
    drvManager.AddDriver(&pit_driver);

    printf("Initializing Peripheral Component Interconnect (PCI).\n");
    PCIController pcicontroller;
    pcicontroller.SelectDrivers(&drvManager, &interrupts);

    printf("Initializing Floppy Disk.\n");
    FloppyDriver floppy_driver(&interrupts, &system_clock);
    drvManager.AddDriver(&floppy_driver);

    printf("Activating drivers.\n");
    drvManager.ActivateAll();

    interrupts.Activate();
    
    printf("Checking which elf modules are loaded:\n");
    multiboot_module_t* elf_modules = (multiboot_module_t*) multiboot_structure -> mods_addr;
    for(int i = 0; i < multiboot_structure -> mods_count; i++) {
        printf("\t");
        printf((const char*)elf_modules[i].string);
        printf("\n");
    }

    // // printf("Setting Up Ramdisk.\n");
    // uint32_t initrd_location = *((uint32_t*)multiboot_structure->mods_addr);
    // uint32_t initrd_end = *(uint32_t*)(multiboot_structure->mods_addr+4);
    // /* There is a risk of this being overwritten, in which case we should think about
    // moving the heap to ensure that it doesn't intersect this. */
    // fs_root = initialize_initrd(initrd_location);

    // int i = 0;
    // struct dirent* node = 0;
    // while((node = readdir_fs(fs_root, i)) != 0) {
    //     printf("Found node: ");
    //     printf(node -> name);
    //     fs_node_t* fsnode = finddir_fs(fs_root, node -> name);
    //     if((fsnode -> flags % 0x7) == FS_DIRECTORY) {
    //         printf(" (directory)");
    //     }
    //     printf("\n");
    //     ++i;
    // }

    // printf("Loading program.\n");
    // jackos::filesystem::elf::Elf_File hello_program((Elf_Ehdr*)elf_modules[0].mod_start);
    // if(!hello_program.check_file()) {
    //     printf("Invalid ELF file.\n");
    // }
    // else {
    //     hello_program.run();
    // }

    // jackos::filesystem::elf::Elf_File p1((Elf_Ehdr*)elf_modules[0].mod_start);
    // jackos::filesystem::elf::Elf_File p2((Elf_Ehdr*)elf_modules[1].mod_start);
    // p1.run();
    // p2.run();

    printf("Initiating floppy test...\n");
    if(floppy_driver.floppy_reset(FLOPPY_BASE) == 0) {
        printf("Floppy reset and calibrated.\n");
    }
    else {
        printf("There was an issue calibrating the floppy...\n");
        for(;;);
    }

    floppy_driver.dma_init(DIR_READ);
    floppy_driver.do_track(FLOPPY_BASE, 0, DIR_READ);
    floppy_driver.ParseFATHeader();
    floppy_driver.read_root_directory();

    FAT12DirectoryEntry program_file = floppy_driver.get_file();
    uint8_t* file = (uint8_t*)0x10000;
    floppy_driver.load_file(program_file.firstCluster, program_file.fileSize, file);
    jackos::filesystem::elf::Elf_File loaded_program((Elf_Ehdr*)file);
    loaded_program.header_dump();
    loaded_program.run();

    #ifdef GRAPHICS_MODE
    printf("Switching to graphics mode.\n");
    system_clock.wait(10);

    vga.SetMode(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DEPTH);
    
    jackos::terminal::Terminal terminal(&vga, &system_clock);
    KeyboardDriver keyboard_driver(&interrupts, &terminal);
    drvManager.ActivateAll();
    // drvManager.AddDriver(&keyboard_driver);

    // Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
    // desktop.AddChild(&win1);
    #endif

    for(;;) { // Infinite loop
        #ifdef GRAPHICS_MODE
        desktop.Draw(&vga);

        terminal.draw();

        vga.DrawFrame(SCREEN_WIDTH, SCREEN_HEIGHT);
        #endif
    }
}