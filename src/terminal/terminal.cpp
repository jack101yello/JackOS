#include <terminal/terminal.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::terminal;

extern void printf(const char* msg);

jackos::terminal::Terminal::Terminal(multiboot* i_mb) {
    mb = i_mb;
    active = true;
    graphics = nullptr;
    system_clock = nullptr;
    buffer_len = 0;
}

void Terminal::draw() {
    newline();
}

void Terminal::newline() {
    printf("\n> ");
}

void Terminal::OnKeyDown(char key) {
    switch(key) {
        case '\n':
            parse_command();
            newline();
            break;
        default:
            if(buffer_len >= TERMINAL_WIDTH) break;
            buffer[buffer_len++] = key;
            buffer[buffer_len] = '\0';
            const char foo[2] = {key, '\0'};
            printf(foo);
            break;
    }
}

void Terminal::parse_command() {
    printf("\n");
    char temp[buffer_len+1];
    for(int i = 0; i < buffer_len; i++) {
        temp[i] = buffer[i];
    }
    temp[buffer_len] = '\0';

    int argc = 0; // The number of arguments
    for(int i = 0; i <= buffer_len; i++) {
        if(temp[i] == ' ') argc++;
        else if(temp[i] == '\0') {
            argc++;
            break;
        }
    }

    char* argv[argc];
    int arg_index = 0;
    char* ptr = temp;
    while(*ptr != '\0') {
        while(*ptr == ' ') ++ptr; // Ignore leading/double spaces
        if(*ptr == '\0') break;
        argv[arg_index++] = ptr;
        while(*ptr != '\0' && *ptr != ' ') ++ptr; // Find the end of the argument
        if(*ptr == ' ') {
            *ptr = '\0';
            ++ptr;
        }
    }

    if(jackos::libc::strcmp(argv[0], "help") == 0) {
        (argc == 1) ? command_help("") : command_help(argv[1]);
    }
    else if(jackos::libc::strcmp(argv[0], "clear") == 0) {
        command_clear();
    }
    else if(jackos::libc::strcmp(argv[0], "list") == 0) {
        list_files();
    }
    else if(jackos::libc::strcmp(argv[0], "run") == 0) {
        run_file();
    }
    else {
        printf("Unknown command: ");
        printf(argv[0]);
    }
    
    buffer[0] = '\0';
    buffer_len = 0;
}