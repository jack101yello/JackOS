#include <common/types.h>
#include <gdt.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <hardware/pci.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::drivers;
using namespace jackos::hardware;

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
    uint8_t x, y;
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
            int oldx = x;
            int oldy = y;
            x += xoffset;
            if(x >= 80) x = 79;
            if(xoffset >= 80 && x > oldx) x = oldx;
            y += yoffset;
            if(y >= 25) y = 24;
            if(yoffset > -25 && y > oldy) y = oldy;
            // if(yoffset <= -25 && y < oldy) y = oldy;
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

extern "C" void kernel_main(void* multiboot_structure, uint32_t magicnumber) {
    printf("Initializing JackOS Kernel\n");

    printf("Setting up Global Descriptor Table (GDT).\n");
    GlobalDescriptorTable gdt;

    printf("Setting up Interrupt Descriptor table (IDT).\n");
    InterruptManager interrupts(&gdt);

    printf("Setting up drivers...\n");
    DriverManager drvManager;

    printf("\tInitiating mouse.\n");
    MouseToConsole mhandler;
    MouseDriver mouse(&interrupts, &mhandler);
    drvManager.AddDriver(&mouse);

    printf("\tInitiating keyboard.\n");
    PrintKeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);
    drvManager.AddDriver(&keyboard);

    printf("Initializing Peripheral Component Interconnect (PCI).\n");
    PCIController pcicontroller;
    pcicontroller.SelectDrivers(&drvManager);

    printf("Activating drivers.\n");
    drvManager.ActivateAll();

    printf("Enabling Interrupts.\n");
    interrupts.Activate();

    for(;;); // Infinite loop
}