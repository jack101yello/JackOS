#ifndef __jackos_programs_terminal_H
#define __jackos_programs_terminal_H

#include <drivers/keyboard.h>
#include <std/string.h>

extern void printf(jackos::std::string str);
extern void printf(const char* msg);
extern void printfhex(int hex);

namespace jackos {
    namespace programs {
        class Terminal {
            private:
                char* buffer;
                jackos::common::uint8_t buffer_length;
                jackos::common::uint8_t word_count;
                void initialize_buffer();
                void destroy_buffer();
                int command_index();
                bool check_command(char* command, int len);
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