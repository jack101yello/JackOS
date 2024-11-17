#include <programs/terminal.h>
#include <std/std.h>

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
    buffer = new char;
    buffer_size = 0;
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
            char* temp = new char[buffer_size + 1];
            for(int i = 0; i < buffer_size; i++) {
                temp[i] = buffer[i];
            }
            delete[] buffer;
            buffer[buffer_size] = c;
            buffer_size++;
            break;
    }
}

uint8_t Terminal::putntharg(int arg, char* dest) {
    // Ensure enough arguments
    if(word_count < arg) {
        return -1;
    }
    int count = 0;
    for(int i = 0; i < buffer_size; i++) {
        if(count == arg) {
            int size = 0;
            for(int j = i; j < buffer_size; j++) {
                if(buffer[j] == ' ') {
                    break;
                }
                size++;
            }
            delete[] dest;
            dest = new char[size];
            for(int j = 0; j < size; j++) {
                char* foo = " ";
                dest[j] = buffer[i + j];
            }
            return size;
        }
        if(buffer[i] == ' ') {
            count++;
        }
    }
    return -2;
}

void Terminal::run_command() {
    char* command = new char[0];
    uint8_t size = putntharg(0, command);
    
    /*
    In future, once the filesystem is up and running, this should be modified
    to run a particular executable file, as per how Unix operates.
    */
    if(jackos::std::streq(command, "help", size, 4)) {
        printf("You have run the help command.\n");
    }
    else {
        printf("Command not found.\n");
    }

    delete[] command;
}


void TerminalKeyboardEventHandler::OnKeyDown(char c) {
    char foo[] = " ";
    foo[0] = c;
    printf(foo);
    terminal -> appendBuffer(c);
}