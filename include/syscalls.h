#ifndef __jackos_syscalls_H
#define __jackos_syscalls_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <multitasking.h>
#include <drivers/vga.h>
#include <terminal/terminal.h>

namespace jackos {
    class SyscallHandler : public jackos::hardware::InterruptHandler {
        private:
            bool graphicsMode; // Indicates whether the current program is using graphical mode or text mode (terminal)
            jackos::drivers::VideoGraphicsArray* graphics;
            jackos::terminal::Terminal* terminal;

        public:
            SyscallHandler(jackos::hardware::InterruptManager* interruptManager, jackos::common::uint8_t InterruptNumber);
            SyscallHandler(jackos::hardware::InterruptManager* interruptManager, jackos::common::uint8_t InterruptNumber, jackos::drivers::VideoGraphicsArray* i_graphics, jackos::terminal::Terminal* i_terminal);
            ~SyscallHandler();
            virtual jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);
    };
}

#endif