#ifndef __jackos_hardware_port_H
#define __jackos_hardware_port_H

#include <common/types.h>

namespace jackos {
    namespace hardware {
        class Port {
            protected:
                jackos::common::uint16_t portnumber;
                Port(jackos::common::uint16_t portnumber);
                ~Port();
        };

        class Port8Bit : public Port {
            public:
                Port8Bit(jackos::common::uint16_t portnumber);
                ~Port8Bit();
                virtual void Write(jackos::common::uint8_t data);
                virtual jackos::common::uint8_t Read();
        };

        class Port8BitSlow : public Port8Bit {
            public:
                Port8BitSlow(jackos::common::uint16_t portnumber);
                ~Port8BitSlow();
                virtual void Write(jackos::common::uint8_t data);
        };

        class Port16Bit : public Port {
            public:
                Port16Bit(jackos::common::uint16_t portnumber);
                ~Port16Bit();
                virtual void Write(jackos::common::uint16_t data);
                virtual jackos::common::uint16_t Read();
        };

        class Port32Bit : public Port {
            public:
                Port32Bit(jackos::common::uint16_t portnumber);
                ~Port32Bit();
                virtual void Write(jackos::common::uint32_t data);
                virtual jackos::common::uint32_t Read();
        };
    }
}

#endif