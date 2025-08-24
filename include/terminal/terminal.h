#ifndef __jackos_terminal_terminal_H
#define __jackos_terminal_terminal_H

#include <drivers/vga.h>
#include <drivers/keyboard.h>
#include <multitasking.h>
#include <drivers/pit.h>
#include <gdt.h>
#include <libc/libc.h>

#define TERMINAL_WIDTH 33
#define TERMINAL_HEIGHT 11

namespace jackos {
    namespace terminal {
        class Terminal : public jackos::drivers::KeyboardEventHandler {
            private:
                bool active;
                int x,y; // The position of the cursor on the screen
                jackos::drivers::VideoGraphicsArray* graphics;
                jackos::drivers::PITEventHandler* system_clock;
                jackos::common::uint8_t buffer_len;
                char full_buffer[TERMINAL_HEIGHT][TERMINAL_WIDTH];
                char line_buffer[TERMINAL_WIDTH];
                void command_help(const char* message);
                void command_clear();
                void newline();
            
            public:
                Terminal(jackos::drivers::VideoGraphicsArray* i_graphics, jackos::drivers::PITEventHandler* i_system_clock);
                void draw();
                void OnKeyDown(char);
                void parse_command();
                void print(const char* message);
        };
    }
}

#endif