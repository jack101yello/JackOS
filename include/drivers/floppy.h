#ifndef __jackos_drivers_floppy_H
#define __jackos_drivers_floppy_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardware/port.h>
#include <drivers/pit.h>
#include <filesystem/fat12.h>
#include <common/common.h>

#define FLOPPY_BASE 0x03F0
#define FLOPPY_IRQ 6
#define FLOPPY_DMA_LEN 0x4800
#define FAT_SIZE 9*512
#define ROOT_DIR_SIZE 14*512

namespace jackos {
    namespace drivers {
        enum floppy_register {
            FLOPPY_DOR = 2, // Digital output register
            FLOPPY_MSR = 4, // Master status register
            FLOPPY_FIFO = 5, // Data FIFO
            FLOPPY_CCR = 7, // Configuration control register
        };

        enum floppy_command {
            CMD_SPECIFY = 3,
            CMD_WRITE_DATA = 5,
            CMD_READ_DATA = 6,
            CMD_RECALIBRATE = 7,
            CMD_SENSE_INTERRUPT = 8,
            CMD_SEEK = 15,
        };

        enum floppy_motor_state {
            MOTOR_OFF = 0,
            MOTOR_ON = 1,
            MOTOR_WAIT = 2,
        };

        enum floppy_dir {
            DIR_READ = 1,
            DIR_WRITE = 2,
        };

        class FloppyDriver : public jackos::hardware::InterruptHandler, public jackos::drivers::Driver {
            private:
                bool freeze;
                jackos::drivers::PITEventHandler* system_clock;
                int motor_ticks;
                int motor_state;
                char dmabuf[FLOPPY_DMA_LEN] __attribute__((aligned(0x8000)));
                jackos::filesystem::FAT12Header* fatheader;

            public:
                FloppyDriver(jackos::hardware::InterruptManager* manager, jackos::drivers::PITEventHandler* i_system_clock);
                void Activate();
                void write_cmd(int base, char cmd);
                unsigned char read_data(int base);
                int floppy_reset(int base);
                jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);
                void check_interrupt(int base, int* st0, int* cyl);
                int calibrate(int base);
                void motor(int base, int onoff);
                void motor_kill(int base);
                int seek(int base, unsigned cyli, int head);
                void dma_init(floppy_dir dir);
                int do_track(int base, unsigned cyl, floppy_dir dir);
                int read_track(int base, unsigned cyl) { return do_track(base, cyl, DIR_READ); }
                int write_track(int base, unsigned cyl) { return do_track(base, cyl, DIR_WRITE); }
                char dma_elem(int index) { return (index >= 0 && index < FLOPPY_DMA_LEN) ? dmabuf[index] : (char)0; }
                void ParseFATHeader() { fatheader = (jackos::filesystem::FAT12Header*) dmabuf; }
                void read_root_directory();
        };
    }
}

#endif