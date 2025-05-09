#ifndef __jackos_hardware_pci_H
#define __jackos_hardware_pci_H

#include <hardware/port.h>
#include <common/types.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>
#include <memorymanagement.h>

namespace jackos {
    namespace hardware {
        enum BaseAddressRegisterType {
            MemoryMapping = 0,
            InputOutput = 1
        };

        class BaseAddressRegister {
            public:
                bool prefetchable;
                jackos::common::uint8_t* address;
                jackos::common::uint32_t size;
                BaseAddressRegisterType type;
        };

        class PCIDeviceDescriptor {
            public:
                jackos::common::uint32_t portbase;
                jackos::common::uint32_t interrupt;

                jackos::common::uint16_t bus;
                jackos::common::uint16_t device;
                jackos::common::uint16_t function;
                
                jackos::common::uint16_t vendor_id;
                jackos::common::uint16_t device_id;

                jackos::common::uint8_t class_id;
                jackos::common::uint8_t subclass_id;
                jackos::common::uint8_t interface_id;

                jackos::common::uint8_t revision;

                PCIDeviceDescriptor();
                ~PCIDeviceDescriptor();
        };

        class PCIController {
            Port32Bit dataport;
            Port32Bit commandport;
            public:
                PCIController();
                ~PCIController();
                jackos::common::uint32_t Read(jackos::common::uint16_t bus, jackos::common::uint16_t device, jackos::common::uint16_t function, jackos::common::uint32_t registeroffset);
                void Write(jackos::common::uint16_t bus, jackos::common::uint16_t device, jackos::common::uint16_t function, jackos::common::uint32_t registeroffset, jackos::common::uint32_t value);
                bool DeviceHasFunctions(jackos::common::uint16_t bus, jackos::common::uint16_t device);
                void SelectDrivers(jackos::drivers::DriverManager* driverManager, jackos::hardware::InterruptManager* manager);
                jackos::drivers::Driver* GetDriver(PCIDeviceDescriptor dev_desc, jackos::hardware::InterruptManager* interruptManager);
                PCIDeviceDescriptor GetDeviceDescriptor(jackos::common::uint16_t bus, jackos::common::uint16_t device, jackos::common::uint16_t function);
                BaseAddressRegister GetBaseAddressRegister(jackos::common::uint16_t bus, jackos::common::uint16_t device, jackos::common::uint16_t function, jackos::common::uint16_t bar);
        };
    }
}

#endif