#ifndef __gdt_H
#define __gdt_H

#include <stdint.h>

void gdt_set_gate(int num, unsigned long base, unsigned long limit,
                           unsigned char access, unsigned char gran);
void gdt_install();

#endif
