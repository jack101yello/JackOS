#ifndef __jackos_syscalls_H
#define __jackos_syscalls_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <multitasking.h>
#include <drivers/vga.h>
#include <terminal/terminal.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <memorymanagement.h>
#include <drivers/cdrom.h>

namespace jackos {
    enum syscall_label {
        PRINT = 0x0,
        ENTER_GRAPHICS_MODE = 0x1,
        EXIT_GRAPHICS_MODE = 0x2,
        CHECK_KEY = 0x3,
        PRINT_ADDR = 0x4,
        QUIT = 0x5,
        PUT_PIXEL = 0x6,
        DRAW_FRAME = 0x7,
        MALLOC = 0x08,
        FREE = 0x09,
        CALLOC = 0x0A,
        REALLOC = 0x0B,
        GET_TICKS = 0x0C,
        SWAP_FRAMEBUFFER = 0x0D,
        FOPEN = 0x0E,
        FCLOSE = 0x0F,
    };

    class SyscallHandler : public jackos::hardware::InterruptHandler {
        private:
            bool graphicsMode; // Indicates whether the current program is using graphical mode or text mode (terminal)
            jackos::drivers::VideoGraphicsArray* graphics;
            jackos::terminal::Terminal* terminal;
            jackos::gui::Desktop* desktop;
            void (*runtime_loop) (jackos::gui::Desktop* desktop, jackos::drivers::VideoGraphicsArray* graphics, jackos::terminal::Terminal* terminal);
			jackos::drivers::CDROMDriver* cdrom_driver;

        public:
            SyscallHandler(jackos::hardware::InterruptManager* interruptManager, jackos::common::uint8_t InterruptNumber);
            SyscallHandler(jackos::hardware::InterruptManager* interruptManager, jackos::common::uint8_t InterruptNumber, jackos::drivers::VideoGraphicsArray* i_graphics, jackos::terminal::Terminal* i_terminal, jackos::gui::Desktop* i_desktop, void (*i_runtime_loop)(jackos::gui::Desktop*, jackos::drivers::VideoGraphicsArray*, jackos::terminal::Terminal*), jackos::drivers::CDROMDriver* i_cdrom_driver);
            ~SyscallHandler();
            virtual jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);
    };
}

#endif
