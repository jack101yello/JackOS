#include "drivers/graphics/graphics.h"
#include "drivers/gdt/gdt.h"
#include "drivers/idt/idt.h"
#include "drivers/IO/io.h"

void kernel_main(void)
{
	terminal_initialize(); // Terminal

	terminal_writestring("Initializing Global Descriptor Table.\n");
	gdt_install(); // GDT

	terminal_writestring("Initializing Interrupt Descriptor Table.\n");
	idt_install(); // IDT

	terminal_writestring("Testing wait command\n");
	
	for(int i = 0; i < 5000000; i++) {
		io_wait();
	}

	terminal_writestring("Complete!\n");
}