#ifndef __mouse_H
#define __mouse_H

#include "types.h"
#include "interrupts.h"
#include "port.h"

class MouseDriver : public InterruptHandler {
    Port8Bit dataport;
    Port8Bit commandport;

    uint8_t buffer[3];
    uint8_t offset;
    uint8_t buttons;
    int8_t x, y;

    public:
        MouseDriver(InterruptManager* manager);
        ~MouseDriver();
        virtual uint32_t HandleInterrupt(uint32_t esp);
};

#endif