#include "pit.h"

int timer_ticks = 0;

void timer_handler(struct regs *r) {
    ++timer_ticks;
}

void timer_install() {
    irq_install_handler(0, timer_handler);
}

void timer_wait(int seconds) {
    int ticks = seconds * 18;
    unsigned long eticks;
    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}
