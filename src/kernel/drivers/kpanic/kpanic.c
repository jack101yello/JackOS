#include "kpanic.h"

void kpanic(const char* message) {
	terminal_initialize(); // Clears the screen
	terminal_writestring("Kernel panic!\n");
	terminal_writestring("A catastrophic error has occurred in the operating system.\n");
	terminal_writestring("Error message: ");
	terminal_writestring(message);
	terminal_writestring("\nPlease restart the operating system.");
	for(;;); // Indefinite halt
}
