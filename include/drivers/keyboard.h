#ifndef __jackos_DRIVERS_keyboard_H
#define __jackos_DRIVERS_keyboard_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <hardware/port.h>
#include <drivers/driver.h>

namespace jackos {
    namespace drivers{
        class KeyboardEventHandler {
            public:
                KeyboardEventHandler();

                virtual void OnKeyDown(char);
                virtual void OnKeyUp(char);
        };

        class KeyboardDriver : public jackos::hardware::InterruptHandler, public Driver {
            jackos::hardware::Port8Bit dataport;
            jackos::hardware::Port8Bit commandport;

            KeyboardEventHandler* handler;
            public:
                KeyboardDriver(jackos::hardware::InterruptManager* manager, KeyboardEventHandler* handler);
                ~KeyboardDriver();
                virtual jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);
                virtual void Activate();
        };
    }
}

#endif