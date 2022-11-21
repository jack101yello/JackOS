#include "drivers/graphics/graphics.h"
#include "drivers/gdt/gdt.h"
#include "drivers/idt/idt.h"
#include "drivers/IO/io.h"
#include "drivers/isr/isr.h"

void kernel_main(void)
{
	terminal_initialize(); // Terminal

	terminal_writestring("Initializing Global Descriptor Table.\n");
	gdt_install(); // GDT

	terminal_writestring("Initializing Interrupt Descriptor Table.\n");
	idt_install(); // IDT

	terminal_writestring("Setting up Interrupt Service Routines.\n");
	isrs_install();

}