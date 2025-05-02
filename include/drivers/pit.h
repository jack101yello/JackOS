#ifndef __jackos_drivers_pit_H
#define __jackos_drivers_pit_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>

namespace jackos {
    namespace drivers {
        class PITEventHandler {
            private:
                jackos::common::uint32_t increment;
                jackos::common::uint32_t internal_wait(jackos::common::uint32_t start_time, jackos::common::uint32_t duration);

            public:
                PITEventHandler();

                virtual void OnPulse();
                virtual jackos::common::uint32_t get_increment();
                virtual jackos::common::uint32_t wait(jackos::common::uint32_t duration);
        };

        class PITDriver : public jackos::hardware::InterruptHandler, public Driver {
            PITEventHandler* handler;

            public:
                PITDriver(jackos::hardware::InterruptManager* manager, PITEventHandler* handler);
                ~PITDriver();
                virtual jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);
                virtual void Activate();
        };
    }
}

#endif