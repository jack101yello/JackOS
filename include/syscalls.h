#ifndef __jackos_syscalls_H
#define __jackos_syscalls_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <multitasking.h>
#include <drivers/vga.h>
#include <terminal/terminal.h>
#include <gui/desktop.h>
#include <gui/window.h>

namespace jackos {
    enum syscall_label {
        PRINT = 0,
        ENTER_GRAPHICS_MODE = 1,
        EXIT_GRAPHICS_MODE = 2,
        CHECK_KEY = 3,
        PRINT_ADDR = 4,
        QUIT = 5,
        PUT_PIXEL = 6,
        DRAW_FRAME = 7
    };

    class SyscallHandler : public jackos::hardware::InterruptHandler {
        private:
            bool graphicsMode; // Indicates whether the current program is using graphical mode or text mode (terminal)
            jackos::drivers::VideoGraphicsArray* graphics;
            jackos::terminal::Terminal* terminal;
            jackos::gui::Desktop* desktop;
            void (*runtime_loop) (jackos::gui::Desktop* desktop, jackos::drivers::VideoGraphicsArray* graphics, jackos::terminal::Terminal* terminal);

        public:
            SyscallHandler(jackos::hardware::InterruptManager* interruptManager, jackos::common::uint8_t InterruptNumber);
            SyscallHandler(jackos::hardware::InterruptManager* interruptManager, jackos::common::uint8_t InterruptNumber, jackos::drivers::VideoGraphicsArray* i_graphics, jackos::terminal::Terminal* i_terminal, jackos::gui::Desktop* i_desktop, void (*i_runtime_loop)(jackos::gui::Desktop*, jackos::drivers::VideoGraphicsArray*, jackos::terminal::Terminal*));
            ~SyscallHandler();
            virtual jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);
    };
}

#endif