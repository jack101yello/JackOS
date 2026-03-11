#ifndef __jackos_drivers_cdrom_H
#define __jackos_drivers_cdrom_H

#include <common/types.h>
#include <common/common.h>
#include <drivers/driver.h>
#include <hardware/interrupts.h>
#include <drivers/pit.h>
#include <filesystem/iso9660.h>
#include <filesystem/vfs.h>
#include <libc/libc.h>

namespace jackos {
    namespace drivers {
        typedef struct {
            jackos::common::uint32_t last_lba;
            jackos::common::uint32_t block_size;
        } capacity_info_t;

        class CDROMDriver : public jackos::hardware::InterruptHandler, public jackos::drivers::Driver {
            private:
                jackos::hardware::InterruptManager* interrupt_manager;
                jackos::drivers::PITEventHandler* system_clock;
                int port;
                bool is_slave;
            
            public:
                CDROMDriver(jackos::hardware::InterruptManager* i_manager, jackos::drivers::PITEventHandler* i_system_clock);
                void Activate();
                void Identify();
                void io_wait();
                void insw(jackos::common::uint16_t port, void* buffer, unsigned long count);
                void outsw(jackos::common::uint16_t port, const void* buffer, unsigned long count);
                int send_packet(jackos::common::uint8_t* cmd, jackos::common::uint16_t max_bytes, bool use_dma, jackos::common::uint16_t* buffer, jackos::common::uint32_t buffer_size);
                int read_sectors(jackos::common::uint32_t lba, jackos::common::uint32_t sector_count, jackos::common::uint16_t* buffer);
                int read_capacity(capacity_info_t* info);
                int test_unit_ready();
				jackos::filesystem::fs_node_t cdrom_open(char* filename); // Return a filesystem node for a given file

        };

		jackos::common::uint32_t cdrom_read(jackos::filesystem::fs_node* node, jackos::common::uint32_t offset, jackos::common::uint32_t size, jackos::common::uint8_t* buffer);	
    }
}

#endif
