#ifndef __jackos_programs_terminal_H
#define __jackos_programs_terminal_H

#include <drivers/keyboard.h>

extern void printf(const char* msg);
extern void printfhex(int hex);

namespace jackos {
    namespace programs {
        class Terminal {
            private:
                char* buffer;
                jackos::common::uint8_t buffer_size;
                jackos::common::uint8_t word_count;
                jackos::common::uint8_t putntharg(int arg, char* dest);
                void initialize_buffer();
                void destroy_buffer();
                void run_command();
            public:
                Terminal();
                ~Terminal();
                void appendBuffer(char c);
        };

        class TerminalKeyboardEventHandler : public jackos::drivers::KeyboardEventHandler {
            private:
                Terminal* terminal;
            public:
                TerminalKeyboardEventHandler(Terminal* terminal) {
                    this -> terminal = terminal;
                }

                void OnKeyDown(char c);
        };
    }
}

#endif