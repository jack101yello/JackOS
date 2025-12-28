#ifndef __jackos_terminal_terminal_H
#define __jackos_terminal_terminal_H

#include <drivers/vga.h>
#include <drivers/keyboard.h>
#include <multitasking.h>
#include <drivers/pit.h>
#include <gdt.h>
#include <libc/libc.h>
#include <multiboot.h>
#include <filesystem/ELF/elfheader.h>
#include <filesystem/ELF/elfloader.h>

#define TERMINAL_WIDTH 33

namespace jackos {
    namespace terminal {
        class Terminal : public jackos::drivers::KeyboardEventHandler {
            private:
                bool active;
                int x,y; // The position of the cursor on the screen
                jackos::drivers::VideoGraphicsArray* graphics;
                jackos::drivers::PITEventHandler* system_clock;
                jackos::common::uint8_t buffer_len;
                char buffer[TERMINAL_WIDTH];
                void command_help(const char* message);
                void command_clear();
                void newline();
                multiboot* mb;
                void list_files();
                void run_file();
                char last_key;
            
            public:
                Terminal(multiboot* i_mb);
                void draw();
                void OnKeyDown(char);
                void parse_command();
                void print(const char* message);
                char getLastKey() { return last_key; }
        };
    }
}

#endif