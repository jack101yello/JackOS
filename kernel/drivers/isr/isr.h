#ifndef __isr_H
#define __isr_H

#include "../idt/idt.h"
#include "../graphics/graphics.h"

struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

void isrs_install();
void fault_handler();

#endif