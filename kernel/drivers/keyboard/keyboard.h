#ifndef __keyboard_H
#define __keyboard_H

#include "../IO/io.h"
#include "../graphics/graphics.h"
#include "../irq/irq.h"

void keyboard_handler(struct regs *r);
void keyboard_install();
void enable_typing();
void disable_typing();

#endif