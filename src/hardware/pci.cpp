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

void PCIController::SelectDrivers(DriverManager* driverManager) {
    for(int bus = 0; bus < 8; bus++) {
        for(int device = 0; device < 32; device++) {
            int numFunctions = DeviceHasFunctions(bus, device) ? 8 : 1;
            for(int function = 0; function < numFunctions; function++) {
                PCIDeviceDescriptor dev_desc = GetDeviceDescriptor(bus, device, function);
                if(dev_desc.vendor_id == 0x0000 || dev_desc.vendor_id == 0xFFFF) break;
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