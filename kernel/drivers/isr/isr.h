#ifndef __isr_H
#define __isr_H

#include "../idt/idt.h"
#include "../graphics/graphics.h"

void isrs_install();
void fault_handler();

#endif