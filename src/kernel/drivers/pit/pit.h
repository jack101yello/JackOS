#ifndef __pit_H
#define __pit_H

#include "../IO/io.h"
#include "../graphics/graphics.h"
#include "../irq/irq.h"

void timer_handler(struct regs *r);
void timer_install();
void timer_wait(int seconds);

#endif