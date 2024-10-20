#ifndef __keyboard_H
#define __keyboard_H

#include "types.h"
#include "interrupts.h"
#include "port.h"
#include "driver.h"

class KeyboardDriver : public InterruptHandler, public Driver {
    Port8Bit dataport;
    Port8Bit commandport;
    public:
        KeyboardDriver(InterruptManager* manager);
        ~KeyboardDriver();
        virtual uint32_t HandleInterrupt(uint32_t esp);
        virtual void Activate();
};

#endif