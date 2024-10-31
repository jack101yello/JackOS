#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <hardware/pci.h>
#include <drivers/vga.h>
#include <drivers/ata.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <multitasking.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::drivers;
using namespace jackos::hardware;
using namespace jackos::gui;

void printf(const char* str) {
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    static uint8_t x = 0, y = 0;

    for(int i = 0; str[i] != '\0'; ++i) {
        switch(str[i]) {
            case '\n':
                y++;
                x = 0;
                break;
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

void printfhex(int val) {
    char msg[] = "00";
    char hex[] = "0123456789ABCDEF";
    msg[0] = hex[(val >> 4) & 0x0F];
    msg[1] = hex[val & 0x0F];
    printf(msg);
}

class PrintKeyboardEventHandler : public KeyboardEventHandler {
    public:
        void OnKeyDown(char c) {
            char foo[] = " ";
            foo[0] = c;
            printf(foo);
        }
};

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

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors() {
    for(constructor* i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

// #define GRAPHICS_MODE

extern "C" void kernel_main(void* multiboot_structure, uint32_t magicnumber) {
    printf("Initializing JackOS Kernel\n");

    printf("Setting up Global Descriptor Table (GDT).\n");
    GlobalDescriptorTable gdt;

    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);
    printf("Heap: 0x");
    printfhex((heap >> 24) & 0xFF);
    printfhex((heap >> 16) & 0xFF);
    printfhex((heap >>  8) & 0xFF);
    printfhex((heap      ) & 0xFF);
    void* allocated = memoryManager.malloc(1024);
    printf("\nAllocated: 0x");
    printfhex(((size_t)allocated >> 24) & 0xFF);
    printfhex(((size_t)allocated >> 16) & 0xFF);
    printfhex(((size_t)allocated >>  8) & 0xFF);
    printfhex(((size_t)allocated      ) & 0xFF);
    printf("\n");

    TaskManager taskManager;

    printf("Setting up Interrupt Descriptor table (IDT).\n");
    InterruptManager interrupts(0x20, &gdt, &taskManager);

    #ifdef GRAPHICS_MODE
    printf("Instantiating Desktop.\n");
    Desktop desktop(320, 200, 0x00, 0x00, 0xA8);
    #endif

    printf("Setting up drivers...\n");
    DriverManager drvManager;

    printf("\tInitiating mouse.\n");
    // MouseToConsole mhandler;
    #ifdef GRAPHICS_MODE
    MouseDriver mouse(&interrupts, &desktop);
    #else
    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);
    #endif
    drvManager.AddDriver(&mouse);

    printf("\tInitiating keyboard.\n");
    // PrintKeyboardEventHandler kbhandler;
    #ifdef GRAPHICS_MODE
    KeyboardDriver keyboard(&interrupts, &desktop);
    #else
    PrintKeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);
    #endif
    drvManager.AddDriver(&keyboard);

    printf("Initializing Peripheral Component Interconnect (PCI).\n");
    PCIController pcicontroller;
    pcicontroller.SelectDrivers(&drvManager, &interrupts);

    #ifdef GRAPHICS_MODE
    printf("Initializing graphics.\n");
    VideoGraphicsArray vga;
    #endif

    printf("Activating drivers.\n");
    drvManager.ActivateAll();

    #ifdef GRAPHICS_MODE
    printf("Switching to graphics mode.\n");
    vga.SetMode(320, 200, 8);
    Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
    desktop.AddChild(&win1);
    Window win2(&desktop, 40, 15, 30, 30, 0x00, 0xA8, 0x00);
    desktop.AddChild(&win2);
    #endif

    // Interrupt 14
    AdvancedTechnologyAttachment ata0m(0x1F0, true);
    printf("ATA Primary master: ");
    ata0m.Identify();
    AdvancedTechnologyAttachment ata0s(0x1F0, false);
    printf("ATA Primary slave: ");
    ata0s.Identify();

    char atabuffer[] = "JACKOS";
    ata0s.Write28(0, (uint8_t*)atabuffer, 7);
    ata0s.Flush();

    ata0s.Read28(0, (uint8_t*)atabuffer, 7);

    // Interrupt 15
    AdvancedTechnologyAttachment ata1m(0x170, true);
    AdvancedTechnologyAttachment ata1s(0x170, false);
    // third, 0x1E8
    // fourth, 0x168

    printf("Enabling Interrupts.\n");
    interrupts.Activate();

    for(;;) { // Infinite loop
        #ifdef GRAPHICS_MODE
        desktop.Draw(&vga);
        vga.DrawFrame(320, 200);
        #endif
    }
}