#include <drivers/floppy.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::drivers;

extern void printf(const char* msg);
extern void printfhex(int hex);
extern void printaddr(int addr);

FloppyDriver::FloppyDriver(jackos::hardware::InterruptManager* manager, jackos::drivers::PITEventHandler* i_system_clock)
: InterruptHandler(0x26, manager) {
    freeze = false;
    system_clock = i_system_clock;
    // motor_ticks = 0;
    motor_state = 0;
}

void FloppyDriver::Activate() {
    static const char* drive_types[8] = {
        "none",
        "360kB 5.25",
        "1.2MB 5.25",
        "720kB 3.5",
        "1.44MB 3.5",
        "2.88MB 3.5",
        "unknown type",
        "unknown type"
    };

    jackos::hardware::Port8Bit port1(0x70);
    jackos::hardware::Port8Bit port2(0x71);
    port1.Write(0x10);
    unsigned int drives = port2.Read();
    printf("Floppy drive 0: ");
    printf(drive_types[drives >> 4]);
    printf("\nFloppy drive 1: ");
    printf(drive_types[drives & 0xF]);
    printf("\n");
}

void FloppyDriver::write_cmd(int base, char cmd) {
    jackos::hardware::Port8Bit MSR_Port(base + FLOPPY_MSR);
    jackos::hardware::Port8Bit FIFO_Port(base + FLOPPY_FIFO);
    for(int i = 0; i < 100; i++) {
        system_clock -> wait(1);
        if(0x80 & MSR_Port.Read()) {
            FIFO_Port.Write(cmd);
            return;
        }
    }
    printf("Floppy disk write timeout.\n");
}

unsigned char FloppyDriver::read_data(int base) {
    jackos::hardware::Port8Bit MSR_Port(base + FLOPPY_MSR);
    jackos::hardware::Port8Bit FIFO_Port(base + FLOPPY_FIFO);
    for(int i = 0; i < 600; i++) {
        system_clock -> wait(1);
        if(0x80 & MSR_Port.Read()) {
            return FIFO_Port.Read();
        }
    }
    printf("Floppy disk read timeout.\n");
    return 0;
}

void FloppyDriver::check_interrupt(int base, int* st0, int* cyl) {
    write_cmd(base, CMD_SENSE_INTERRUPT);
    *st0 = read_data(base);
    *cyl = read_data(base);
}

int FloppyDriver::calibrate(int base) {
    printf("Calibrating floppy.\n");
    int i, st0, cyl = -1; // Bogus cylinder
    motor(base, MOTOR_ON);
    // freeze = true;
    for(int i = 0; i < 10; i++) {
        write_cmd(base, CMD_RECALIBRATE);
        // printf("Waiting for interrupt...\n");
        // while(freeze) {} // Wait for interrupt on IRQ 6
        // printf("Interrupt fired!\n");
        check_interrupt(base, &st0, &cyl);
        if(st0 & 0xC0) {
            static const char* status[] = {
                0, "error", "invalid", "drive"
            };
            printf("floppy calibration status: ");
            printf(status[st0 >> 6]);
            continue;
        }

        if(!cyl) {
            motor(base, MOTOR_OFF);
            return 0;
        }
    }

    printf("10 retries exhausted.\n");
    motor(base, MOTOR_OFF);
    return -1;
}

int FloppyDriver::floppy_reset(int base) {
    printf("Resetting floppy.\n");
    freeze = true;
    jackos::hardware::Port8Bit DOR_Port(base + FLOPPY_DOR);
    DOR_Port.Write(0x00); // Disable controller
    DOR_Port.Write(0x0C); // Enable controller
    
    printf("Waiting for IRQ 6\n");
    while(freeze) {} // Wait for interrupt on IRQ 6
    printf("Continuing...\n");
    
    int st0, cyl;
    check_interrupt(base, &st0, &cyl);

    jackos::hardware::Port8Bit CCR_Port(base + FLOPPY_CCR);
    CCR_Port.Write(0x00); // Set transfer speed to 500 kbps

    jackos::hardware::Port8Bit Base_Port(base);
    Base_Port.Write(CMD_SPECIFY);
    Base_Port.Write(0xdf); // Steprate = 3ms, Unload time = 240ms
    Base_Port.Write(0x02); // Load time = 16ms, no-DMA = 0

    printf("Reset complete!\n");
    return calibrate(base);
}

jackos::common::uint32_t FloppyDriver::HandleInterrupt(jackos::common::uint32_t esp) {
    freeze = false;
    return esp;
}

void FloppyDriver::motor(int base, int onoff) {
    jackos::hardware::Port8Bit DOR_Port(base + FLOPPY_DOR);
    if(onoff) {
        if(!motor_state) {
            DOR_Port.Write(0x1C); // Turn the motor on
            system_clock -> wait(1); // The drive needs a moment
        }
        motor_state = MOTOR_ON;
    }
    else {
        if(motor_state == MOTOR_WAIT) {
            printf("The floppy is already waiting?\n");
        }
        // motor_ticks = 300;
        motor_state = MOTOR_WAIT;
    }
}

void FloppyDriver::motor_kill(int base) {
    jackos::hardware::Port8Bit DOR_Port(base + FLOPPY_DOR);
    DOR_Port.Write(0x0C);
    motor_state = MOTOR_OFF;
}

int FloppyDriver::seek(int base, unsigned cyli, int head) {
    int i, st0, cyl = -1; // Bogus cylinder
    motor(base, MOTOR_ON);
    for(int i = 0; i < 10; i++) {
        write_cmd(base, CMD_SEEK);
        write_cmd(base, head << 2);
        write_cmd(base, cyli);

        freeze = true;
        while(freeze == false) {} // Wait for IRQ 6
        check_interrupt(base, &st0, &cyl);
        if(st0 & 0xC0) {
            static const char* status[] = {
                "normal", "error", "invalid", "drive"
            };
            printf("Floppy seek status: ");
            printf(status[st0 >> 6]);
            continue; // I suspect this should be a break
        }

        if(cyl == cyli) {
            motor(base, MOTOR_OFF);
            return 0;
        }
    }
    printf("Floppy seek: 10 retries\n");
    motor(base, MOTOR_OFF);
    return -1;
}

void FloppyDriver::dma_init(floppy_dir dir) {
    // printf("Initializing DMA\n");
    union {
        unsigned char b[4]; // 4 bytes
        unsigned long l; // 32-bit
    } a, c; // Address and count
    a.l = (unsigned) &dmabuf;
    c.l = (unsigned) FLOPPY_DMA_LEN - 1; // We have to subtract 1 because of how DMA counts

    if((a.l >> 24) || (c.l >> 16) || (((a.l&0xffff)+c.l)>>16)) {
        printf("Static buffer problem when initializing floppy DMA.\n");
        return;
    }

    unsigned char mode;
    switch(dir) {
        case DIR_READ: mode = 0x46; break;
        case DIR_WRITE: mode = 0x4A; break;
        default: printf("Floppy: Invalid direction.\n"); return; break;
    }

    jackos::hardware::Port8Bit Channel2(0x0A);
    jackos::hardware::Port8Bit Reset(0x0C);
    jackos::hardware::Port8Bit Addr(0x04);
    jackos::hardware::Port8Bit External_Page_Register(0x81);
    jackos::hardware::Port8Bit Count(0x05);
    jackos::hardware::Port8Bit Set_Mode(0x0B);

    Channel2.Write(0x06); // Mask
    Reset.Write(0xff);
    Addr.Write(a.b[0]); // Low byte
    Addr.Write(a.b[1]); // High byte
    External_Page_Register.Write(a.b[2]);
    Reset.Write(0xff);
    Count.Write(c.b[0]); // Low byte
    Count.Write(c.b[1]); // High byte
    Set_Mode.Write(mode);
    Channel2.Write(0x02); // Unmask
}

int FloppyDriver::do_track(int base, unsigned logical_sector, floppy_dir dir) {
    int sector = (logical_sector % 18) + 1;
    int head = (logical_sector / 18) % 2;
    int cyl = logical_sector / 36;
    unsigned char cmd;

    static const int flags = 0xC0;
    switch(dir) {
        case DIR_READ:
            cmd = CMD_READ_DATA | flags;
            break;
        case DIR_WRITE:
            cmd = CMD_WRITE_DATA | flags;
            break;
        default:
            printf("Floppy track: invalid direction.\n");
            return 0;
    }

    if(seek(base, cyl, head)) return -1;

    for(int i = 0; i < 20; i++) {
        // printf("Firing motor\n");
        motor(base, MOTOR_ON);
        dma_init(dir);
        system_clock -> wait(1); // Give some time to settle after seeking
        // printf("Attempting read\n");

        freeze = true;
        write_cmd(base, cmd); // Set above for current direction
        write_cmd(base, (head << 2) | 0); // Head and drive
        write_cmd(base, cyl); // Cylinder
        write_cmd(base, head); // First head
        write_cmd(base, sector); // First sector (they count from 1)
        write_cmd(base, 2); // Bytes per sector, 128*2^x => 512
        write_cmd(base, 18); // Number of tracks
        write_cmd(base, 0x1B); // GAP3 length; this is default for 3.5"
        write_cmd(base, 0xFF); // Data lengths

        while(freeze); // Wait for IRQ 6

        unsigned char st0, st1, st2, rcy, rhe, rse, bps;
        st0 = read_data(base); // Status information
        st1 = read_data(base); // Status information
        st2 = read_data(base); // Status information
        rcy = read_data(base); // Cylinder
        rhe = read_data(base); // Head
        rse = read_data(base); // Sector
        bps = read_data(base); // Bytes per sector
        
        int error = 0;
        if(st0 & 0xC0) {
            static const char* status[] = {
                0, "error", "invalid command", "drive not ready"
            };
            printf("Floppy do sector: ");
            printf(status[st0 >> 6]);
            printf("\n");
            error = 1;
        }
        if(st0 & 0x80) {
            printf("Floppy do sector: end of cylinder\n");
            error = 1;
        }
        if(st0 & 0x08) {
            printf("Floppy do sector: drive not ready\n");
            error = 1;
        }
        if(st0 & 0x20) {
            printf("Floppy do sector: CRC error\n");
            error = 1;
        }
        if(st1 & 0x10) {
            printf("floppy_do_sector: controller timeout\n");
            error = 1;
        }
        if(st1 & 0x04) {
            printf("floppy_do_sector: no data found\n");
            error = 1;
        }
        if((st1|st2) & 0x01) {
            printf("floppy_do_sector: no address mark found\n");
            error = 1;
        }
        if(st2 & 0x40) {
            printf("floppy_do_sector: deleted address mark\n");
            error = 1;
        }
        if(st2 & 0x20) {
            printf("floppy_do_sector: CRC error in data\n");
            error = 1;
        }
        if(st2 & 0x10) {
            printf("floppy_do_sector: wrong cylinder\n");
            error = 1;
        }
        if(st2 & 0x04) {
            printf("floppy_do_sector: uPD765 sector not found\n");
            error = 1;
        }
        if(st2 & 0x02) {
            printf("floppy_do_sector: bad cylinder\n");
            error = 1;
        }
        if(bps != 0x2) {
            printf("floppy_do_sector: wanted 512B/sector, got ");
            printfhex(1 << (bps+7));
            printf("\n");
            error = 1;
        }
        if(st1 & 0x02) {
            printf("floppy_do_sector: not writable\n");
            error = 2;
        }

        if(!error) {
            motor(base, MOTOR_OFF);
            return 0;
        }
        if(error > 1) {
            printf("Floppy do sector: not retrying...\n");
            motor(base, MOTOR_OFF);
            return -2;
        }
    }

    printf("Floppy do sector: 20 retries exhausted\n");
    motor(base, MOTOR_OFF);
    return -1;
}

void FloppyDriver::read_root_directory() {
    // Read the first 9 sectors to fill FAT
    for(int i = 0; i < 9; i++) {
        int sector = i+1;
        int track = sector / 18;
        int cyl = track;

        do_track(FLOPPY_BASE, cyl, DIR_READ);
        memcpy(fat + i*512, (uint8_t*)dmabuf, 512); // Load the dmabuf into fat
    }

    uint8_t root_dir_sector_buffer[ROOT_DIR_SIZE];
    
    // Read the 14 sectors starting from sector 19 (the root directory)
    for(int i = 0; i < 14; i++) {
        int logical_sector = 19+i;

        do_track(FLOPPY_BASE, logical_sector, DIR_READ);
        memcpy(root_dir_sector_buffer + i*512, (uint8_t*)dmabuf, 512);
    }

    jackos::filesystem::FAT12DirectoryEntry* entries = (jackos::filesystem::FAT12DirectoryEntry*)root_dir_sector_buffer;

    for(int i = 0; i < 224; i++) {
        printfhex(i);
        system_clock -> wait(1);
        if(entries[i].name[0] == 0x00) break; // No more entries
        if(entries[i].name[0] == 0xE5) continue; // Deleted
        if(entries[i].attributes == 0x0F) continue; // Long file name entry

        printf("File found: ");
        for(int j = 0; j < 8; j++) {
            char c = entries[i].name[j];
            if(c == ' ') continue;
            char foo[] = {c, '\0'};
            printf(foo);
        }
        printf(".");
        for(int j = 0; j < 3; j++) {
            char c = entries[i].ext[j];
            if(c == ' ') continue;
            char foo[] = {c, '\0'};
            printf(foo);
        }
        printf("\nOf size ");
        printaddr(entries[i].fileSize);
        printf(" bytes.\n");

        file = entries[i];
    }
}

void FloppyDriver::load_file(uint16_t start_cluster, uint32_t size, uint8_t* destination) {
    uint16_t cluster = start_cluster;
    uint32_t bytes_read = 0;

    while(cluster >= 0x002 && cluster <= 0xFEF && bytes_read < size) {
        int logical_sector = DATA_AREA_START + (cluster - 2);

        // Read the sector into the DMA buffer
        if(do_track(FLOPPY_BASE, logical_sector, DIR_READ) != 0) {
            printf("Failed to read cluster ");
            printfhex(cluster);
            printf("\n");
            break;
        }

        uint32_t copy_size = 512;
        if(bytes_read + copy_size > size) {
            copy_size = size - bytes_read;
        }
        memcpy(destination + bytes_read, (uint8_t*)dmabuf, copy_size);
        bytes_read += copy_size;

        cluster = fat12_get_entry(cluster);
    }

    if(bytes_read < size) {
        printf("Warning: only read ");
        printaddr(bytes_read);
        printf(" bytes out of ");
        printaddr(size);
        printf("\n");
    }
    else {
        printf("File loaded: ");
        printaddr(bytes_read);
        printf(" bytes.\n");
    }
}

uint16_t FloppyDriver::fat12_get_entry(uint16_t cluster) {
    uint16_t offset = (cluster * 3) / 2;
    uint16_t value;

    if(cluster & 1) {
        return ((fat[offset] >> 4) | (fat[offset + 1] << 4)) & 0x0FFF;
    }
    return (fat[offset] | ((fat[offset + 1] & 0x0F) << 8)) & 0x0FFF;
}
