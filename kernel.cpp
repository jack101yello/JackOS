#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "driver.h"
#include "keyboard.h"
#include "mouse.h"

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

    printf("Setting up GDT.\n");
    GlobalDescriptorTable gdt;

    printf("Setting up IDT.\n");
    InterruptManager interrupts(&gdt);

    printf("Setting up drivers...\n");
    DriverManager drvManager;
    /* The mouse has to be initiated before the keyboard,
    or else the keyboard does not work. Reason unknown.
    */
    printf("\tInitiating mouse.\n");
    MouseDriver mouse(&interrupts);
    drvManager.AddDriver(&mouse);
    printf("\tInitiating keyboard.\n");
    KeyboardDriver keyboard(&interrupts);
    drvManager.AddDriver(&keyboard);

    printf("Activating drivers.\n");
    drvManager.ActivateAll();

    printf("Enabling Interrupts.\n");
    interrupts.Activate();

    for(;;); // Infinite loop
}