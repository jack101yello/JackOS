#ifndef __jackos_hardware_interrupts_H
#define __jackos_hardware_interrupts_H

#include <hardware/port.h>
#include <common/types.h>
#include <gdt.h>
#include <multitasking.h>

namespace jackos {
    namespace hardware {

        class InterruptManager;

        class InterruptHandler {
            protected:
                jackos::common::uint8_t interruptNumber;
                InterruptManager* interruptManager;
                InterruptHandler(jackos::common::uint8_t interruptNumber, InterruptManager* interruptManager);
                ~InterruptHandler();
            public:
                virtual jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);
        };

        class InterruptManager {
            friend class InterruptHandler;
            protected:
                static InterruptManager* ActiveInterruptManager;
                InterruptHandler* handlers[256]; // Array of interrupt handlers
                jackos::common::uint8_t hardwareoffset;
                TaskManager* taskManager;

                struct GateDescriptor {
                    jackos::common::uint16_t handlerAddressLowBits;
                    jackos::common::uint16_t gdt_codeSegmentSelector;
                    jackos::common::uint8_t reserved;
                    jackos::common::uint8_t access;
                    jackos::common::uint16_t handlerAddressHighBits;
                } __attribute__((packed));

                static GateDescriptor interruptDescriptorTable[256];

                struct InterruptDescriptorTablePointer {
                    jackos::common::uint16_t size;
                    jackos::common::uint32_t base;
                } __attribute__((packed));

                static void SetInterruptDescriptorTableEntry(
                    jackos::common::uint8_t interruptNumber,
                    jackos::common::uint16_t codeSegmentSelectorOffset,
                    void (*handler)(),
                    jackos::common::uint8_t DescriptorPrivilegeLevel,
                    jackos::common::uint8_t DescriptorType
                );

                Port8BitSlow picMasterCommand;
                Port8BitSlow picMasterData;
                Port8BitSlow picSlaveCommand;
                Port8BitSlow picSlaveData;

            public:
                InterruptManager(jackos::common::uint8_t hardwareoffset, GlobalDescriptorTable* gdt, jackos::TaskManager* taskManager);
                ~InterruptManager();

                void Activate();
                void Deactivate();

                static jackos::common::uint32_t handleInterrupt(jackos::common::uint8_t interruptNumber, jackos::common::uint32_t esp);
                jackos::common::uint32_t doHandleInterrupt(jackos::common::uint8_t interruptNumber, jackos::common::uint32_t esp);

                static void IgnoreInterruptRequest();
                static void HandleInterruptRequest0x00();
                static void HandleInterruptRequest0x01();
                static void HandleInterruptRequest0x02();
                static void HandleInterruptRequest0x03();
                static void HandleInterruptRequest0x04();
                static void HandleInterruptRequest0x05();
                static void HandleInterruptRequest0x06();
                static void HandleInterruptRequest0x07();
                static void HandleInterruptRequest0x08();
                static void HandleInterruptRequest0x09();
                static void HandleInterruptRequest0x0A();
                static void HandleInterruptRequest0x0B();
                static void HandleInterruptRequest0x0C();
                static void HandleInterruptRequest0x0D();
                static void HandleInterruptRequest0x0E();
                static void HandleInterruptRequest0x0F();

                static void HandleException0x00();
                static void HandleException0x01();
                static void HandleException0x02();
                static void HandleException0x03();
                static void HandleException0x04();
                static void HandleException0x05();
                static void HandleException0x06();
                static void HandleException0x07();
                static void HandleException0x08();
                static void HandleException0x09();
                static void HandleException0x0A();
                static void HandleException0x0B();
                static void HandleException0x0C();
                static void HandleException0x0D();
                static void HandleException0x0E();
                static void HandleException0x0F();
                static void HandleException0x10();
                static void HandleException0x11();
                static void HandleException0x12();
                static void HandleException0x13();
        };
    }
}

#endif