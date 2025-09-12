#include <terminal/terminal.h>

using namespace jackos;
using namespace jackos::terminal;
using namespace jackos::drivers;


jackos::terminal::Terminal::Terminal(jackos::drivers::VideoGraphicsArray* i_graphics, PITEventHandler* i_system_clock, multiboot* i_mb) {
    active = true;
    graphics = i_graphics;
    line_buffer[0] = '\0';
    system_clock = i_system_clock;
    mb = i_mb;
    buffer_len = 0;
    x = 1;
    y = 0;
    for(int i = 0; i < TERMINAL_HEIGHT; i++) {
        for(int j = 0; j < TERMINAL_WIDTH; j++) {
            full_buffer[i][j] = ' ';
        }
    }
}

void Terminal::draw() {
    if((system_clock -> get_increment()/12) % 2 == 0) { // Cursor
        graphics -> Print("\b", (x+1)*9, y*16, GREEN);
    }
    for(int i = 0; i < TERMINAL_HEIGHT; i++) {
        for(int j = 0; j < TERMINAL_WIDTH; j++) {
            const char temp[] = {full_buffer[i][j], '\0'};
            graphics -> Print(temp, (j+1)*9, i*16, GREEN);
        }
    }
    graphics -> Print(">", 0, y*16, GREEN);
}

void Terminal::OnKeyDown(char key) {
    switch(key) {
        case 0x0E: // Backspace
            if(buffer_len == 0 || x == 1) break;
            line_buffer[buffer_len - 1] = '\0';
            full_buffer[y][x-1] = ' ';
            buffer_len--;
            x -= 1;
            break;
        case '\n': // Enter
            parse_command();
            break;
        default:
            if(buffer_len >= TERMINAL_WIDTH) break; // Cannot exceed screen length
            line_buffer[buffer_len] = key;
            full_buffer[y][x] = key;
            line_buffer[buffer_len+1] = '\0';
            buffer_len++;
            x += 1;
            break;
    }
}

void Terminal::newline() {
    x = 1;
    y += 1;
    if(y >= TERMINAL_HEIGHT) {
        for(int i = 0; i < TERMINAL_HEIGHT - 1; i++) {
            for(int j = 0; j < TERMINAL_WIDTH; j++) {
                full_buffer[i][j] = full_buffer[i+1][j];
            }
        }
        for(int j = 0; j < TERMINAL_WIDTH; j++) {
            full_buffer[TERMINAL_HEIGHT-1][j] = ' ';
        }
        y--;
    }
}

void Terminal::print(const char* message) {
    for(int i = 0; message[i] != '\0'; i++) {
        if(x >= TERMINAL_WIDTH) {
            newline();
        }
        if(message[i] == '\n') {
            newline();
        }
        else {
            full_buffer[y][x++] = message[i];
        }
    }
}

void Terminal::parse_command() {
    newline();

    char temp[buffer_len+1];
    for(int i = 0; i < buffer_len; i++) {
        temp[i] = line_buffer[i];
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
        print("Unknown command: ");
        print(argv[0]);
        newline();
    }
    
    line_buffer[0] = '\0';
    buffer_len = 0;
}

void Terminal::command_clear() {
    for(int i = 0; i < TERMINAL_HEIGHT; i++) {
        for(int j = 0; j < TERMINAL_WIDTH; j++) {
            full_buffer[i][j] = ' ';
        }
    }
    x = 1;
    y = 0;
}

void Terminal::list_files() {
    multiboot_module_t* elf_modules = (multiboot_module_t*) mb -> mods_addr;
    for(int i = 0; i < mb -> mods_count; i++) {
        print("\t");
        print((const char*)elf_modules[i].string);
        newline();
    }
}

void Terminal::run_file() {
    multiboot_module_t* elf_modules = (multiboot_module_t*) mb -> mods_addr;
    jackos::filesystem::elf::Elf_File elf_program((Elf_Ehdr*)elf_modules[0].mod_start);
    if(!elf_program.check_file()) return;
    elf_program.run();
}

void Terminal::command_help(const char* argument) {
    if(jackos::libc::strcmp(argument, "") == 0) {
        print("Valid commands:\n");
        print("help    clear");
        print("\nAlso use \"help + command\"\n");
    }
    else if(jackos::libc::strcmp(argument, "help") == 0) {
        print("You think you're smart, huh?\n");
    }
    else if(jackos::libc::strcmp(argument, "clear") == 0) {
        print("Clears the screen.\n");
    }
    else if(jackos::libc::strcmp(argument, "list") == 0) {
        print("Lists the ELF modules in memory.\n");
    }
    else if(jackos::libc::strcmp(argument, "run") == 0) {
        print("Runs the first ELF file in memory.\n");
    }
    else {
        print("Unknown command: ");
        print(argument);
        print("\n");
    }
}