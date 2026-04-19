#include <drivers/cdrom.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::drivers;
using namespace jackos::filesystem;

extern void printf(const char* msg);
extern void printfhex(int n);
extern void printaddr(int addr);

CDROMDriver::CDROMDriver(jackos::hardware::InterruptManager* i_manager, jackos::drivers::PITEventHandler* i_system_clock, jackos::GlobalDescriptorTable* i_gdt)
: jackos::hardware::InterruptHandler(0x2E, i_manager) {
    interrupt_manager = i_manager;
    system_clock = i_system_clock;
	gdt = i_gdt;
}

void CDROMDriver::Activate() {
    uint16_t buses[] = {0x1F0, 0x170};
    const char* bus_names[] = {"Primary", "Secondary"};

    for(int bus_index = 0; bus_index < 2; bus_index++) { // Check primary and secondary
        uint16_t base_port = buses[bus_index];
        for(int device = 0; device < 2; device++) { // Check master and slave
            jackos::hardware::Port8Bit device_port(base_port + 0x06);
            jackos::hardware::Port8Bit sector_count_port(base_port + 0x02);
            jackos::hardware::Port8Bit lba_low_port(base_port + 0x03);
            jackos::hardware::Port8Bit lba_mid_port(base_port + 0x04);
            jackos::hardware::Port8Bit lba_high_port(base_port + 0x05);
            jackos::hardware::Port8Bit status_port(base_port + 0x07);

            device_port.Write(0xA0 | (device << 4)); // Select device
            system_clock -> wait(10);
            status_port.Write(0x90);
            uint8_t sc = sector_count_port.Read();
            uint8_t lba_low = lba_low_port.Read();
            uint8_t lba_mid = lba_mid_port.Read();
            uint8_t lba_high = lba_high_port.Read();
            if(sc == 0x01 && lba_low == 0x01 && lba_mid == 0x14 && lba_high == 0xEB) {
                printf("ATAPI device found.\n");
                port = base_port;
                Identify();
                return;
            }
            else if(sc == 0x01 && lba_low == 0x01 && lba_mid == 0x00 && lba_high == 0x00) {
                printf("ATA device (hard disk) found.\n");
            }
            else if(status_port.Read() == 0xFF) {
                printf("No device present.\n");
            }
            else {
                printf("Unknown device.\n");
                printf("sc -> ");
                printaddr(sc);
                printf(" LBA -> ");
                printfhex(lba_low);
                printfhex(lba_mid);
                printfhex(lba_high);
                printf("\n");
            }
        }
    }
    printf("No ATAPI device found on any bus.\n");
}

void CDROMDriver::insw(uint16_t port, void* buffer, unsigned long count) {
    asm volatile(
        "cld; rep; insw"
        : "+D" (buffer), "+c" (count)
        : "d" (port)
    );
}

void CDROMDriver::outsw(uint16_t port, const void* buffer, unsigned long count) {
    asm volatile(
        "cld; rep; outsw"
        : "+S" (buffer), "+c" (count)
        : "d" (port)
    );
}

void CDROMDriver::Identify() {
    jackos::hardware::Port8Bit primary_port(port);
    jackos::hardware::Port8Bit device_port(port + 0x6);
    jackos::hardware::Port8Bit com_port(port + 0x07);

    is_slave = false; // Should be changed in future
    device_port.Write(0xA0);
    system_clock -> wait(10); // Wait has been finicky with timing; this may be too long
    com_port.Write(0xA1); // Identify packet device
    while(com_port.Read() & 0x80); // Wait for BSY clear
    while(!(com_port.Read() & 0x08)); // Wait for DRQ set
    int DATA_BUFFER_SIZE = 256;
    uint16_t identify_data[DATA_BUFFER_SIZE];
    for(int i = 0; i < DATA_BUFFER_SIZE; i++) {
        identify_data[i] = primary_port.Read();
    }
    printf("ATAPI device identified successfully.\n");   
}

void CDROMDriver::io_wait() {
    jackos::hardware::Port8Bit pt(port + 0x206);
    for(int i = 0; i < 4; i++) {
        pt.Read();
    }
}

int CDROMDriver::send_packet(uint8_t* cmd, uint16_t max_bytes, bool use_dma, uint16_t* buffer, uint32_t buffer_size) {
    jackos::hardware::Port8Bit features_port(port + 0x1);
    jackos::hardware::Port8Bit lba_low_port(port + 0x03);
    jackos::hardware::Port8Bit lba_mid_port(port + 0x04);
    jackos::hardware::Port8Bit lba_high_port(port + 0x05);
    jackos::hardware::Port8Bit device_port(port + 0x6);
    jackos::hardware::Port8Bit com_port(port + 0x07);

    device_port.Write(0xA0 | (is_slave << 4)); // Select device
    io_wait();
    features_port.Write(use_dma ? 1 : 0);
    lba_mid_port.Write(max_bytes & 0xFF);
    lba_high_port.Write(max_bytes >> 8);

    com_port.Write(0xA0); // Packet command

    while(1) {
        uint8_t status = com_port.Read();
        if(status & 0x01) return -1; // Error
        if(!(status & 0x80) && (status & 0x08)) break; // It's ready
    }

    outsw(port, (uint16_t*)cmd, 6);

    if(use_dma) {
        // Wait for IRQ
        return 0;
    }
	uint8_t* buf_ptr = (uint8_t*)buffer;
	uint32_t total_read = 0;
	io_wait();


    while(total_read < buffer_size) {
        uint8_t status = com_port.Read();
        if(status & 0x01) return -1; // Error

		if(status & 0x08) {
			uint16_t byte_count = (lba_high_port.Read() << 8) | lba_mid_port.Read();
			if(byte_count == 0) break;
			if(buf_ptr && byte_count > 0) {
				insw(port, buf_ptr, byte_count / 2);
				buf_ptr += byte_count;
				total_read += byte_count;
			}
		}
    }


//  uint16_t byte_count = (lba_high_port.Read() << 8) | lba_mid_port.Read();
//  if(buffer && byte_count > 0) {
//      insw(port, buffer, byte_count/2);
//  }

    int timeout = 1000000;
    while(timeout-- > 0) {
        uint8_t status = com_port.Read();
        if(status & 0x01) return -1; // Error
        if(!(status & 0x80) && (status & 0x08)) break; // It's ready
    }

    return 0;
}

int CDROMDriver::read_sectors(uint32_t lba, uint32_t sector_count, uint16_t* buffer) {
    uint8_t command[12] {
        0xA8, // Read opcode
        0, // Flags
        (lba >> 24) & 0xFF,
        (lba >> 16) & 0xFF,
        (lba >> 8) & 0xFF,
        lba & 0xFF,
        (sector_count >> 24) & 0xFF,
        (sector_count >> 16) & 0xFF,
        (sector_count >> 8) & 0xFF,
        sector_count & 0xFF,
        0,
        0
    };

    return send_packet(command, 2048, false, buffer, sector_count * 2048);
}

int CDROMDriver::read_capacity(capacity_info_t* info) {
	printf("Reading capacity.\n");
    uint8_t command[12] = {
        0x25, // Read capacity opcode
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    uint8_t response[8];
    int result = send_packet(command, 8, false, (uint16_t*)response, 8);
    if(result == 0) {
        info->last_lba = (response[0] << 24) | (response[1] << 16) | (response[2] << 8) | response[3];
        info->block_size = (response[4] << 24) | (response[5] << 16) | (response[6] << 8) | response[7];
    }
    return result;
}

int CDROMDriver::test_unit_ready() {
    uint8_t command[12] = {0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return send_packet(command, 0, false, nullptr, 0);
}

fs_node_t CDROMDriver::cdrom_open(char* filename) {
	struct fs_node filesystem_node; // The node to be returned
	jackos::common::memset((uint8_t*)&filesystem_node, 0, sizeof(fs_node)); // Initialize to zero
	// First we must locate the file on disk
	uint8_t pvd_buffer[4096];
	read_sectors(CDROM_DATA_SECTOR_START, 1, (uint16_t*)pvd_buffer);
	PrimaryVolumeDescriptor* pvd = (PrimaryVolumeDescriptor*)pvd_buffer;
	DirectoryEntry* rootdir = (DirectoryEntry*) pvd -> root_dir_entry;
	uint8_t root_buffer[8192];
	read_sectors(rootdir -> extent_location, (rootdir -> data_length + 2047) / 2048, (uint16_t*)root_buffer);
	uint8_t* ptr = root_buffer;
	uint8_t* end = root_buffer + rootdir -> data_length;
	bool found = false;
	while(ptr < end) {
		uint8_t length = ptr[0];
		if(length == 0) {
			ptr = (uint8_t*)(((uint32_t)ptr + 2047) & ~2047);
			continue;
		}
		DirectoryEntry* entry = (DirectoryEntry*)ptr;
		char* name = (char*)(ptr + 33);
		uint8_t name_len = *(ptr + 32);
		for(int i = 0; i < name_len; i++) {
			if(name[i] == ';') name[i] = '\0'; // Remove the version padding
		}
		if(jackos::libc::strcmp(name, filename) == 0) {
			found = true;
			break;
		}
		ptr += length;
	}
	if(!found) {
		printf("File not found\n");
		return filesystem_node; // Empty file because ours wasn't found
	}
	
	// Now we can construct the struct
	uint8_t name_len = *(ptr + 32);
	for(int i = 0; i < name_len; i++) {
		filesystem_node.name[i] = (char)(*(ptr + 33 + i));
	}
	filesystem_node.read = jackos::drivers::cdrom_read;
	filesystem_node.length = *(uint32_t*)(ptr + 10);
	filesystem_node.extent_location = *(uint32_t*)(ptr + 2); // The LBA of the file
	filesystem_node.cdrom_driver = this; // I personally dislike this implementation, but I don't see a better way
	return filesystem_node;
}

void CDROMDriver::run() {
	// First we must find an ELF to run
	struct fs_node filesystem_node;
	char* filename;
	jackos::common::memset((uint8_t*)&filesystem_node, 0, sizeof(fs_node));
	uint8_t pvd_buffer[4096];
	read_sectors(CDROM_DATA_SECTOR_START, 1, (uint16_t*)pvd_buffer);
	PrimaryVolumeDescriptor* pvd = (PrimaryVolumeDescriptor*)pvd_buffer;
	DirectoryEntry* rootdir = (DirectoryEntry*) pvd -> root_dir_entry;
	uint8_t root_buffer[8192];
	read_sectors(rootdir -> extent_location, (rootdir -> data_length + 2047) / 2048, (uint16_t*)root_buffer);
	uint8_t* ptr = root_buffer;
	uint8_t* end = root_buffer + rootdir -> data_length;
	bool found = false;
	while(ptr < end) {
		uint8_t length = ptr[0];
		if(length == 0) {
			ptr = (uint8_t*)(((uint32_t)ptr + 2047) & ~2047);
			continue;
		}
		DirectoryEntry* entry = (DirectoryEntry*)ptr;
		char* name = (char*)(ptr + 33);
		uint8_t name_len = *(ptr + 32);
		for(int i = 0; i < name_len; i++) {
			if(name[i] == ';') name[i] = '\0';
		}
		for(int i = 0; i < name_len-4; i++) {
			if(name[i] == '.' && name[i+1] == 'E' && name[i+2] == 'L' && name[i+3] == 'F') {
				found = true;
				filename = name;
				break;
			}
		}
		if(found == true) break;
		ptr += length;
	}
	if(!found) {
		printf("CD not executable.\n");
		return;
	}
	filesystem_node = cdrom_open(filename);
	uint8_t file_buffer[512 * 1024];
	read_sectors(filesystem_node.extent_location, (filesystem_node.length + 2047) / 2048, (uint16_t*)file_buffer);
	jackos::filesystem::elf::Elf_File elf_executable((Elf_Ehdr*)file_buffer, gdt);
	elf_executable.run();
}

uint32_t jackos::drivers::cdrom_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
	printf("Reading file: "); printf(node -> name); printf("\n");
	printf("Length of file: "); printaddr(node -> length); printf(" bytes.\n");
	printf("File location: "); printaddr(node -> extent_location); printf("\n");
	node -> cdrom_driver -> read_sectors(node -> extent_location, (node -> length + 2047) / 2048, (uint16_t*)buffer); // Pointer hell
	printf("Contents:\n");
	for(int i = 0; i < node -> length; i++) {
		char foo[2] = {(char)buffer[i], '\0'};
		printf(foo);
	}

	return 0;
}
