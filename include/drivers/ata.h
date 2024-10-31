#ifndef __jackos_drivers_ata_H
#define __jackos_drivers_ata_H

#include <common/types.h>
#include <hardware/port.h>

namespace jackos {
    namespace drivers {
        class AdvancedTechnologyAttachment {
            protected:
                jackos::hardware::Port16Bit dataPort;
                jackos::hardware::Port8Bit errorPort;
                jackos::hardware::Port8Bit sectorCountPort;
                jackos::hardware::Port8Bit lbaLowPort;
                jackos::hardware::Port8Bit lbaMidPort;
                jackos::hardware::Port8Bit lbaHiPort;
                jackos::hardware::Port8Bit devicePort;
                jackos::hardware::Port8Bit commandPort;
                jackos::hardware::Port8Bit controlPort;
                bool master;
                jackos::common::uint16_t bytesPerSector;

            public:
                AdvancedTechnologyAttachment(jackos::common::uint16_t portBase, bool master);
                ~AdvancedTechnologyAttachment();

                void Identify();
                void Read28(jackos::common::uint32_t sector, jackos::common::uint8_t* data, int count);
                void Write28(jackos::common::uint32_t sector, jackos::common::uint8_t* data, int count);
                void Flush();
        };
    }
}

#endif