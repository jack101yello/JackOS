#include <programs/terminal.h>
#include <std/string.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::programs;

Terminal::Terminal() {
    initialize_buffer();
}

Terminal::~Terminal() {
    destroy_buffer();
}

void Terminal::initialize_buffer() {
    word_count = 1;
    buffer = new char[1];
    buffer[0] = '\0';
    buffer_length = 0;
    printf("> ");
}

void Terminal::destroy_buffer() {
    delete[] buffer;
}

void Terminal::appendBuffer(char c) {
    switch(c) {
        case '\n':
            run_command();
            destroy_buffer();
            initialize_buffer();
            break;
        case ' ':
            word_count++; // Intentionally no break
        default:
            char* new_buffer = new char[buffer_length + 2];
            for(int i = 0; i < buffer_length; i++) {
                new_buffer[i] = buffer[i];
            }
            delete[] buffer;
            buffer = new_buffer;
            buffer[buffer_length] = c;
            buffer[buffer_length + 1] = '\0';
            buffer_length++;
            break;
    }
}

bool Terminal::check_command(char* command, int len) {
    int count = 0;
    for(int i = 0; i < buffer_length; i++) {
        if(buffer[i] == ' ') {
            break;
        }
        count++;
    }
    if(count != len) {
        return false;
    }
    for(int i = 0; i < count; i++) {
        if(buffer[i] != command[i]) {
            return false;
        }
    }
    return true;
}

int Terminal::command_index() {
    if(check_command("help", 4)) return 0;
    else return -1;
}

void Terminal::run_command() {
    switch(command_index()) {
        case 0: // Help
            printf("HELP Window:\n");
            break;
        default: // Command not found
            printf("Command not found.\n");
            break;
    }
}

void TerminalKeyboardEventHandler::OnKeyDown(char c) {
    char foo[] = " ";
    foo[0] = c;
    printf(foo);
    terminal -> appendBuffer(c);
}