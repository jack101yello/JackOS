#include "keyboard.h"

bool typing_enabled;

unsigned char kb[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b' /*Backspace*/,
    '\t' /*Tab*/, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n'/*Enter*/, 0 /*CTRL*/,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0 /*Leftshift*/,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0 /*Rightshift*/,
    '*', 0 /*Alt*/, ' ' /*Spacebar*/, 0 /*Caps lock*/, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /*F1-10*/,
    0 /*Numlock*/, 0 /* Scroll lock*/, 0 /*Home key*/, 0 /*Up arrow*/, 0 /*Page up*/,
    '-', 0 /*Left arrow*/, 0, 0 /*Right arrow*/, '+', 0/*End*/, 0/*Down arrow*/, 0/*Page down*/,
    0 /*Insert*/, 0/*Delete*/, 0, 0, 0, 0, 0/*F11-12*/, 0,/*Other keys are undefined*/
};

void keyboard_handler(struct regs *r) {
    if(!typing_enabled) return; // Ensures that typing can only occur when allowed (e.g. the terminal is waiting for input)
    unsigned int scancode;
    scancode = inb(0x60);

    if(scancode & 0x80) { // Key release
        // Code here can determine whether the shift, alt, or control keys were released
    }
    else {
        char temp[2] = {kb[scancode], '\0'}; // This is the required format for a string. Otherwise a page fault ensues
        terminal_writestring(temp);
    }
}

void enable_typing() { typing_enabled = true; }
void disable_typing() { typing_enabled = false; }

void keyboard_install() {
    typing_enabled = false;
    irq_install_handler(1, keyboard_handler);
}
