#include <gdt.h>

using namespace jackos;
using namespace jackos::common;

GlobalDescriptorTable::GlobalDescriptorTable()
: nullSegmentSelector(0, 0, 0),
unusedSegmentSelector(0, 0, 0),
codeSegmentSelector(0, 64*1024*1024, 0x9A),
dataSegmentSelector(0, 64*1024*1024, 0x92),
userCodeSegmentSelector(0, 0xFFFFFFFF, 0xFA),
userDataSegmentSelector(0, 0xFFFFFFFF, 0xF2),
tssSegmentSelector((uint32_t)&tss, sizeof(tss), 0x89)
{
    for(int i = 0; i < sizeof(TaskStateSegment); i++) {
        ((uint8_t*)&tss)[i] = 0;
    }
    static uint8_t kernel_stack[0x4000]; // 16 kB kernel stack
    tss.ss0 = DataSegmentSelector();
    tss.esp0 = (uint32_t)(kernel_stack + sizeof(kernel_stack));
    uint32_t i[2];
    i[1] = (uint32_t)this;
    i[0] = sizeof(GlobalDescriptorTable) << 16;

    asm volatile("lgdt (%0)": :"p" (((uint8_t *) i) + 2));
    // asm volatile("jmp 0x08"); // Long jump after loading GDT
    uint16_t tss_selector = TSSSegmentSelector();
    asm volatile("ltr %0" : : "r"(tss_selector));
}

GlobalDescriptorTable::~GlobalDescriptorTable() {
    
}

uint16_t GlobalDescriptorTable::DataSegmentSelector() {
    return (uint8_t *)&dataSegmentSelector - (uint8_t*)this;
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector() {
    return (uint8_t *)&codeSegmentSelector - (uint8_t*)this;
}

uint16_t GlobalDescriptorTable::UserDataSegmentSelector() {
    return ((uint8_t*)&userDataSegmentSelector - (uint8_t*)this) | 3;
}

uint16_t GlobalDescriptorTable::UserCodeSegmentSelector() {
    return ((uint8_t*)&userCodeSegmentSelector - (uint8_t*)this) | 3;
}

uint16_t GlobalDescriptorTable::TSSSegmentSelector() {
    return (uint8_t*)&tssSegmentSelector - (uint8_t*)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t flags) {
    uint8_t* target = (uint8_t*)this;

    if(limit <= 65536) {
        target[6] = 0x40;
    }
    else {
        if((limit & 0xFFF) != 0xFFF) {
            limit = (limit >> 12) - 1;
        }
        else {
            limit = (limit >> 12);
        }
        target[6] = 0xC0;
    }
    target[0] = limit & 0xFF;
    target[1] = limit >> 8 & 0xFF;
    target[6] |= (limit >> 16) & 0xF;
    target[2] = base & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;
    target[5] = flags;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base() {
    uint8_t* target = (uint8_t*)this;
    uint32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit() {
    uint8_t* target = (uint8_t*)this;
    uint32_t result = target[6] * 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];
    
    if((target[6] & 0xC0) == 0xC0) {
        result = (result << 12) | 0xFFF;
    }

    return result;
}

void GlobalDescriptorTable::SetKernelStack(uint32_t esp0) {
    tss.esp0 = esp0;
    tss.ss0 = DataSegmentSelector();
}