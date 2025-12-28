#include <terminal/terminal.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::terminal;

void Terminal::command_help(const char* argument) {
    if(jackos::libc::strcmp(argument, "") == 0) {
        printf("Valid commands:\n");
        printf("help    clear");
        printf("\nAlso use \"help + command\"\n");
    }
    else if(jackos::libc::strcmp(argument, "help") == 0) {
        printf("You think you're smart, huh?\n");
    }
    else if(jackos::libc::strcmp(argument, "clear") == 0) {
        printf("Clears the screen.\n");
    }
    else if(jackos::libc::strcmp(argument, "list") == 0) {
        printf("Lists the ELF modules in memory.\n");
    }
    else if(jackos::libc::strcmp(argument, "run") == 0) {
        printf("Runs the first ELF file in memory.\n");
    }
    else {
        printf("Unknown command: ");
        printf(argument);
        printf("\n");
    }
}

void Terminal::command_clear() {
    
}

void Terminal::list_files() {

}

void Terminal::run_file() {
    
}