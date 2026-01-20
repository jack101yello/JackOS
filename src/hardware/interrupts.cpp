#include <hardware/interrupts.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::hardware;

void printf(const char* str);

InterruptHandler::InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager) {
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler() {
    if(interruptManager->handlers[interruptNumber] == this) {
        interruptManager->handlers[interruptNumber] = 0;
    }
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp) {
    return esp;
}

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager* InterruptManager::ActiveInterruptManager = 0;

void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interruptNumber,
    uint16_t codeSegmentSelectorOffset,
    void (*handler)(),
    uint8_t DescriptorPrivilegeLevel,
    uint8_t DescriptorType
) {
    const uint8_t IDT_DESC_PRESENT = 0x80;

    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel&3) << 5);
    interruptDescriptorTable[interruptNumber].reserved = 0;
}

InterruptManager::InterruptManager(uint8_t hardwareoffset, GlobalDescriptorTable* gdt, TaskManager* taskManager)
: picMasterCommand(0x20),
picMasterData(0x21),
picSlaveCommand(0xA0),
picSlaveData(0xA1)
{
    this->taskManager = taskManager;
    this->hardwareoffset = hardwareoffset;
    uint16_t CodeSegment = gdt -> CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;
    const uint8_t IDT_TRAP_GATE = 0xF;

    for(uint16_t i = 0; i < 256; i++) {
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
        handlers[i] = 0;
    }

    SetInterruptDescriptorTableEntry(0x00, CodeSegment, &HandleException0x00, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x01, CodeSegment, &HandleException0x01, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x02, CodeSegment, &HandleException0x02, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x03, CodeSegment, &HandleException0x03, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x04, CodeSegment, &HandleException0x04, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x05, CodeSegment, &HandleException0x05, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x06, CodeSegment, &HandleException0x06, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x07, CodeSegment, &HandleException0x07, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x08, CodeSegment, &HandleException0x08, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x09, CodeSegment, &HandleException0x09, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0A, CodeSegment, &HandleException0x0A, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0B, CodeSegment, &HandleException0x0B, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0C, CodeSegment, &HandleException0x0C, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0D, CodeSegment, &HandleException0x0D, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0E, CodeSegment, &HandleException0x0E, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x0F, CodeSegment, &HandleException0x0F, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x10, CodeSegment, &HandleException0x10, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x11, CodeSegment, &HandleException0x11, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x12, CodeSegment, &HandleException0x12, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(0x13, CodeSegment, &HandleException0x13, 0, IDT_INTERRUPT_GATE);

    SetInterruptDescriptorTableEntry(hardwareoffset + 0x00, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE); // PIT
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x01, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE); // PS/2 Keyboard
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x02, CodeSegment, &HandleInterruptRequest0x02, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x03, CodeSegment, &HandleInterruptRequest0x03, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x04, CodeSegment, &HandleInterruptRequest0x04, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x05, CodeSegment, &HandleInterruptRequest0x05, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x06, CodeSegment, &HandleInterruptRequest0x06, 0, IDT_INTERRUPT_GATE); // Floppy disk
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x07, CodeSegment, &HandleInterruptRequest0x07, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x08, CodeSegment, &HandleInterruptRequest0x08, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x09, CodeSegment, &HandleInterruptRequest0x09, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x0A, CodeSegment, &HandleInterruptRequest0x0A, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x0B, CodeSegment, &HandleInterruptRequest0x0B, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x0C, CodeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE); // PS/2 Mouse
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x0D, CodeSegment, &HandleInterruptRequest0x0D, 0, IDT_INTERRUPT_GATE);
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x0E, CodeSegment, &HandleInterruptRequest0x0E, 0, IDT_INTERRUPT_GATE); // Primary ATA channel
    SetInterruptDescriptorTableEntry(hardwareoffset + 0x0F, CodeSegment, &HandleInterruptRequest0x0F, 0, IDT_INTERRUPT_GATE); // Secondary ATA channel

    SetInterruptDescriptorTableEntry(0x80, CodeSegment, &HandleInterruptRequest0x80, 3, IDT_TRAP_GATE); // Syscalls

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);
    picMasterData.Write(0x20);
    picSlaveData.Write(0x28);
    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);
    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);
    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);

    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));
}

InterruptManager::~InterruptManager() {
    Deactivate();
}

void InterruptManager::Activate() {
    if(ActiveInterruptManager != 0) {
        ActiveInterruptManager -> Deactivate();
    }
    ActiveInterruptManager = this;
    asm("sti"); // Enable interrupts
}

void InterruptManager::Deactivate() {
    if(ActiveInterruptManager == this) {
        ActiveInterruptManager = 0;
        asm("cli");
    }
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp) {
    if(ActiveInterruptManager != 0) {
        return ActiveInterruptManager -> doHandleInterrupt(interruptNumber, esp);
    }
    return esp;
}

void SetPixel(int x, int y) {
    uint8_t* framebuffer = (uint8_t*)0xA0000;
    if(x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    framebuffer[x + y * SCREEN_WIDTH] = 0x3F;
}

void print_message(const char* msg) {
    #ifdef GRAPHICS_MODE
    bluescreen(msg);
    #else
    printf(msg);
    #endif
}

void kpanic(uint8_t interruptNumber, uint32_t esp) {
    // Need to adapt this for graphics mode. Some sort of a bluescreen would suffice.
    switch(interruptNumber) { // Check for exceptions
        case 0x00: print_message("\n/// Exception 0x00: Single-step interrupt.\n"); break; // Single-step interrupt
        case 0x02: print_message("\n/// Exception 0x02: Non-maskable interrupt (NMI).\n"); break; // Non-maskable interrupt (NMI)
        case 0x03: print_message("\n/// Exception 0x03: Breakpoint.\n"); break; // Breakpoint
        case 0x04: print_message("\n/// Exception 0x04: Overflow.\n"); break; // Overflow
        case 0x05: print_message("\n/// Exception 0x05: Bound range exceeded.\n"); break; // Bound range exceeded
        case 0x06: print_message("\n/// Exception 0x06: Invalid opcode.\n"); break; // Invalid opcode
        case 0x07: print_message("\n/// Exception 0x07: Coprocessor unavailable.\n"); break; // Coprocessor not available
        case 0x08: print_message("\n/// Exception 0x08: Double fault.\n"); break; // Double fault
        case 0x09: print_message("\n/// Exception 0x09: Coprocessor segment overrun.\n"); break; // Coprocessor segment overrun
        case 0x0A: print_message("\n/// Exception 0x0A: Invalid task state segment.\n"); break; // Invalid task state segment
        case 0x0B: print_message("\n/// Exception 0x0B: Segment not present.\n"); break; // Segment not present
        case 0x0C: print_message("\n/// Exception 0x0C: Stack segment fault.\n"); break; // Stack segment fault
        case 0x0D: print_message("\n/// Exception 0x0D: General protection fault.\n"); break; // General protection fault
        case 0x0E: print_message("\n/// Exception 0x0E: Page fault.\n"); break; // Page fault
        case 0x0F: print_message("\n/// Exception 0x0F: Reserved exception.\n"); break; // Reserved
        case 0x10: print_message("\n/// Exception 0x10: x87 floating point exception.\n"); break; // x87 floating point exception
        case 0x11: print_message("\n/// Exception 0x11: Alignment check.\n"); break; // Alignment check
        case 0x12: print_message("\n/// Exception 0x12: Machine check.\n"); break; // Machine check
        case 0x13: print_message("\n/// Exception 0x13: SIMD floating point exception.\n"); break; // SIMD floating point exception

        default: break; // This should never run; a mistake has been made.
    }
    for(;;); // Infinite halt
}

void breakpoint() {
    printf("break");
}

uint32_t InterruptManager::doHandleInterrupt(uint8_t interruptNumber, uint32_t esp) {
    if(handlers[interruptNumber] != 0) {
        esp = handlers[interruptNumber]->HandleInterrupt(esp);
    }
    else if(interruptNumber > 0x13 && interruptNumber != hardwareoffset) { // Check that this isn't the timer
        char msg[] = "/// UNHANDLED INTERRUPT 0x00";
        char hex[] = "0123456789ABCDEF";
        msg[26] = hex[(interruptNumber >> 4) & 0x0F];
        msg[27] = hex[interruptNumber & 0x0F];
        printf(msg);
    }

    if(interruptNumber == hardwareoffset) { // PIT
        esp = (uint32_t)taskManager->Schedule((CPUState*)esp);
    }

    if(interruptNumber >= 0x00 && interruptNumber <= 0x13) { // Exception
        kpanic(interruptNumber, esp);
    }

    // Hardware interrupts much be acknowledged
    if(0x20 <= interruptNumber && interruptNumber < 0x30) {
        picMasterCommand.Write(0x20); // The response that the PIC wants
        if(0x28 <= interruptNumber) { // Check if the interrupt came from the slave
            picSlaveCommand.Write(0x20);
        }
    }

    return esp;
}