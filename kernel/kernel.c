#include "drivers/graphics/graphics.h"
#include "drivers/gdt/gdt.h"
#include "drivers/idt/idt.h"
#include "drivers/IO/io.h"
#include "drivers/isr/isr.h"
#include "drivers/irq/irq.h"
#include "drivers/pit/pit.h"

void kernel_main(void)
{
	terminal_initialize(); // Terminal

	terminal_writestring("Initializing Global Descriptor Table.\n");
	gdt_install(); // GDT

	terminal_writestring("Initializing Interrupt Descriptor Table.\n");
	idt_install(); // IDT

	terminal_writestring("Setting up Interrupt Service Routines.\n");
	isrs_install(); // ISR

	terminal_writestring("Setting up Interrupt Requests.\n");
	irq_install(); // IRQ
	__asm__ __volatile__ ("sti");

	terminal_writestring("Setting up Programmable Interval Timer.\n");
	timer_install();

}