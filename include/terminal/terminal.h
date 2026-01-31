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
#include <multitasking.h>
#include <drivers/floppy.h>
#include <filesystem/fat12.h>

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
                void prompt();
                void drun_file(const char* filename);
                jackos::GlobalDescriptorTable* gdt;
                volatile bool command_pending;
                jackos::drivers::FloppyDriver* floppy_driver;
            
            public:
                Terminal(multiboot* i_mb, jackos::GlobalDescriptorTable* i_gdt, jackos::drivers::FloppyDriver* i_floppy_driver, jackos::drivers::PITEventHandler* i_system_clock);
                void draw();
                void OnKeyDown(char);
                void OnKeyUp(char);
                void parse_command();
                void print(const char* message);
                void initialize();
                void service();
                bool check_key(char key);
        };
    }
}

#endif