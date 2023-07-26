#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "terminal.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include <stdint.h>
#include "../cpu/timer.h"

void kernel_main() {
    clear_screen();
    kprint("Kernel booted.\n");
    kprint("Installing Interrupt Service Routines (ISR).\n");
    isr_install();
    kprint("Installing Interrupt Requests (IRQ).\n");
    irq_install();
    kprint("Initializing the heap.\n");
    initialize_paging();

    kprint("Startup complete. Booting terminal...\n");
    terminal_input("NOP");
}
