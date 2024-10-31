#ifndef __jackos_syscalls_H
#define __jackos_syscalls_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <multitasking.h>

namespace jackos {
    class SyscallHandler : public jackos::hardware::InterruptHandler {
        public:
            SyscallHandler(jackos::hardware::InterruptManager* interruptManager, jackos::common::uint8_t InterruptNumber);
            ~SyscallHandler();
            virtual jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);
    };
}

#endif