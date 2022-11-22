#include "idt.h"

struct idt_entry {
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr _idtp;

extern void _idt_load();

void idt_set_gate(unsigned char num, unsigned long base,
                    unsigned short sel, unsigned char flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].sel = sel;
}


void idt_install() {
    _idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    _idtp.base = (unsigned int)&idt;

    for(int i = 0; i < 256; i++) { // Clear IDT to 0
        idt_set_gate(i, 0, 0, 0);
    }

    // Add ISRs here using idt_set_gate()
    
    _idt_load();
}

void cli() {
    asm("cli");
    return;
}
