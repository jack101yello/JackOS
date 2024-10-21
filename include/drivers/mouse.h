#ifndef __jackos__drivers_mouse_H
#define __jackos_drivers_mouse_H

#include <common/types.h>
#include <hardware/port.h>
#include <drivers/driver.h>
#include <hardware/interrupts.h>

namespace jackos {
    namespace drivers { 
        class MouseEventHandler {
            public:
                MouseEventHandler();
                virtual void OnActivate();
                virtual void OnMouseDown(jackos::common::uint8_t button);
                virtual void OnMouseUp(jackos::common::uint8_t button);
                virtual void OnMouseMove(int xoffset, int yoffset);
        };

        class MouseDriver : public jackos::hardware::InterruptHandler, public Driver {
            jackos::hardware::Port8Bit dataport;
            jackos::hardware::Port8Bit commandport;

            jackos::common::uint8_t buffer[3];
            jackos::common::uint8_t offset;
            jackos::common::uint8_t buttons;
            MouseEventHandler* handler;

            public:
                MouseDriver(jackos::hardware::InterruptManager* manager, MouseEventHandler* handler);
                ~MouseDriver();
                virtual jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);
                virtual void Activate();
        };
    }
}

#endif