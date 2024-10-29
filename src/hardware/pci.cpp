#include <hardware/pci.h>

using namespace jackos::common;
using namespace jackos::hardware;
using namespace jackos::drivers;

PCIDeviceDescriptor::PCIDeviceDescriptor() {

}

PCIDeviceDescriptor::~PCIDeviceDescriptor() {

}

PCIController::PCIController()
: dataport(0xCFC),
commandport(0xCF8)
{

}

PCIController::~PCIController() {

}

uint32_t PCIController::Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset) {
    uint32_t id = 
    0x1 << 31
    | ((bus & 0xFF) << 16)
    | ((device & 0x1F) << 11)
    | ((function & 0x07) << 8)
    | ((registeroffset & 0xFC));
    commandport.Write(id);
    uint32_t result = dataport.Read();
    return result >> (8 * (registeroffset % 4));
}

void PCIController::Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registeroffset, uint32_t value) {
    uint32_t id = 
    0x1 << 31
    | ((bus & 0xFF) << 16)
    | ((device & 0x1F) << 11)
    | ((function & 0x07) << 8)
    | ((registeroffset & 0xFC));
    commandport.Write(id);
    dataport.Write(value);
}

bool PCIController::DeviceHasFunctions(uint16_t bus, uint16_t device) {
    return Read(bus, device, 0, 0x0E) & (1 << 7);
}

void printf(const char* str);
void printfhex(int val);

void PCIController::SelectDrivers(DriverManager* driverManager, InterruptManager* manager) {
    for(int bus = 0; bus < 8; bus++) {
        for(int device = 0; device < 32; device++) {
            int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;
            for(int function = 0; function < numFunctions; function++) {
                PCIDeviceDescriptor dev_desc = GetDeviceDescriptor(bus, device, function);
                if(dev_desc.vendor_id == 0x0000 || dev_desc.vendor_id == 0xFFFF) continue;

                for(int barNum = 0; barNum < 6; barNum++) {
                    BaseAddressRegister bar = GetBaseAddressRegister(bus, device, function, barNum);
                    if(bar.address && (bar.type == InputOutput)) {
                        dev_desc.portbase = (uint32_t)bar.address;
                    }
                    Driver* driver = GetDriver(dev_desc, manager);
                    if(driver != 0) {
                        driverManager -> AddDriver(driver);
                    }
                }

                printf("PCI BUS ");
                printfhex(bus & 0xFF);
                printf(", DEVICE ");
                printfhex(device & 0xFF);
                printf(", FUNCTION ");
                printfhex(function & 0xFF);
                printf(" = VENDOR ");
                printfhex((dev_desc.vendor_id & 0xFF00) >> 8);
                printfhex(dev_desc.vendor_id & 0XFF);
                printf(", DEVICE ");
                printfhex((dev_desc.device_id & 0xFF00) >> 8);
                printfhex(dev_desc.device_id & 0XFF);
                printf("\n");
            }
        }
    }
}

BaseAddressRegister PCIController::GetBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar) {
    BaseAddressRegister result;
    uint32_t headertype = Read(bus, device, function, 0x0E) & 0x7F;
    int maxBARs = 7 - (4 * headertype);
    if(bar >= maxBARs) return result;
    uint32_t bar_value = Read(bus, device, function, 0x10 + 4 * bar);
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    uint32_t temp;
    if(result.type == MemoryMapping) {
        switch(bar_value >> 1 & 0x3) {
            case 0: break; // 32 bit mode
            case 1: break; // 20 bit mode
            case 2: break; // 64 bit mode
        }
        result.prefetchable = ((bar_value >> 3) & 0x1) == 0x1;
    }
    else { // Input/Output
        result.address = (uint8_t*)(bar_value & ~0x3);
        result.prefetchable = false;
    }
    return result;
}

Driver* PCIController::GetDriver(PCIDeviceDescriptor dev_desc, InterruptManager* interruptManager) {
    Driver* driver;
    switch(dev_desc.vendor_id) {
        case 0x1022: // AMD
            switch(dev_desc.device_id) {
                case 0x2000: // am79c973
                    printf("AMD 79c973");
                    break;
            }
            break;
        case 0x8086: // Intel
            break;
    }
    switch(dev_desc.class_id) {
        case 0x03: // graphics
            switch(dev_desc.subclass_id) {
                case 0x00: // VGA
                    break;
            }
            break;
    }
    return driver; // Change later
}

PCIDeviceDescriptor PCIController::GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function) {
    PCIDeviceDescriptor result;
    result.bus = bus;
    result.device = device;
    result.function = function;
    result.vendor_id = Read(bus, device, function, 0x00);
    result.device_id = Read(bus, device, function, 0x02);
    result.class_id = Read(bus, device, function, 0x0b);
    result.subclass_id = Read(bus, device, function, 0xa);
    result.interface_id = Read(bus, device, function, 0x09);
    result.revision = Read(bus, device, function, 0x08);
    result.interrupt = Read(bus, device, function, 0x3c);
    return result;
}