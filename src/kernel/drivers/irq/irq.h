#ifndef __irq_H
#define __irq_H

#include "../idt/idt.h"
#include "../isr/isr.h"
#include "../IO/io.h"
#include "../graphics/graphics.h"

void irq_install_handler(int irq, void (*handler)(struct regs *r));
void irq_uninstall_handler(int irq);
void irq_install();

#endif