#include "man.h"

void print_man_page(char* command) {
    kprint("Manual page for: ");
    kprint(command);
    kprint("\n");
    if(strcmp(command, "EXIT") == 0) {
        kprint(
            "The \"EXIT\" command quits the terminal and "
        "begins an infinite halt. It is useful for some "
        "debugging purposes, but is otherwise quite limited."
        );
    }
    else if(strcmp(command, "PAGE") == 0) {
        kprint(
            "The \"PAGE\" command is used to test the memory "
            "allocation functionality of the operating system. \n"
            "It simply allocates a new blank page with malloc, "
            "just to prove that it can."
        );
    }
    else if(strcmp(command, "CLEAR") == 0) {
        kprint(
            "The \"CLEAR\" command clears the screen completely "
            "and moves the cursor back to the top-left."
        );
    }
    else if(strcmp(command, "NOP") == 0) {
        kprint(
            "The \"NOP\" command (no-operation) does, well, "
            "nothing. It is used for debugging purposes to run a "
            "useless command."
        );
    }
    else if(strcmp(command, "MAN") == 0) {
        kprint(
            "The \"MAN\" command displays the manual page for a command.\n\n"
            "The proper formatting is:\n"
            "MAN [COMMAND NAME]\n\n"
            "What you are currently reading is the manual page for the MAN "
            "command."
        );
    }
    else if(strcmp(command, "HELP") == 0) {
        kprint(
            "The \"HELP\" command lists all of the available command, "
            "along with a brief explanation of what they do."
        );
    }
    else {
        kprint("No manual page found for that command.");
    }
    kprint("\n\n");
}