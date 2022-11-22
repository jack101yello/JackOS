#include "kernel.h"

void kernel_main(void)
{

	/*
	Kernel initialization
	*/

	terminal_initialize(); // Sets up graphical terminal for bootup

	terminal_writestring("Initializing Global Descriptor Table.\n");
	gdt_install(); // GDT

	terminal_writestring("Initializing Interrupt Descriptor Table.\n");
	idt_install(); // IDT

	terminal_writestring("Setting up Interrupt Service Routines.\n");
	isrs_install(); // ISR

	terminal_writestring("Setting up Interrupt Requests.\n");
	irq_install(); // IRQ

	/*
	Devices
	*/

	terminal_writestring("Setting up Programmable Interval Timer.\n");
	timer_install(); // PIT

	terminal_writestring("Setting up Keyboard.\n");
	keyboard_install(); // Keyboard

	/*
	This section is dedicated to runtime, and runs continuously while the OS is running
	*/

	for(;;); // Infinite halt so that the OS doesn't just crash

}