#include "terminal.h"

void terminal_input(char *input) {
    // Command/// EXIT
    if (strcmp(input, "EXIT") == 0) {
        kprint("Exiting...\n");
        asm volatile("hlt");
    }
    // Command/// CLEAR
    else if (strcmp(input, "CLEAR") == 0) {
        clear_screen();
    }
    else if(strcmp(input, "NOP") == 0) {}
    // Command/// MAN
    else if(strcmp(input, "MAN") == 0) {
        kprint("The \"man\" command must be followed by the name of a command.\n");
    }
    else if(startswith(input, "MAN ") == 0) {
        char commandname[strlen(input)-3];
        for(int i = 4; i < strlen(input); i++) {
            commandname[i-4] = input[i];
        }
        commandname[strlen(input)-4] = '\0';
        print_man_page(commandname);
    }
    // Command/// HELP
    else if(strcmp(input, "HELP") == 0) {
        kprint("Use \"man\" followed by the name of a command to view its manual page.\n");
        kprint("EXIT - Quit the terminal\n");
        kprint("CLEAR - Clear the screen\n");
        kprint("MAN - Read the manual page for a command\n");
        kprint("HELP - View this page\n");
        kprint("NOP - No operation\n");
    }
    // Not one of the commands
    else {
        kprint("Command \"");
        kprint(input);
        kprint("\" not found.\n");
    }
    kprint("> ");
}
