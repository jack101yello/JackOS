#include <terminal/terminal.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::terminal;
using namespace jackos::drivers;

extern void printf(const char* msg);

extern "C" void enter_usermode();

jackos::terminal::Terminal::Terminal(multiboot* i_mb, GlobalDescriptorTable* i_gdt, FloppyDriver* i_floppy_driver, PITEventHandler* i_system_clock) {
    mb = i_mb;
    graphics = nullptr;
    system_clock = i_system_clock;
    buffer_len = 0;
    gdt = i_gdt;
    command_pending = false;
    active = false;
    floppy_driver = i_floppy_driver;
}

void Terminal::initialize() {
    active = true;
    buffer[0] = '\0';
    buffer_len = 0;
    command_clear();
    newline();
    for(int i = 0; i < 0xFF; i++) {
        keyboard_statemap[i] = false;
    }
}

void Terminal::prompt() {
    printf("> ");
}

void Terminal::newline() {
    printf("\n");
    prompt();
}

void Terminal::OnKeyDown(char key) {
    keyboard_statemap[(int)key] = true;
    if(!active) return;
    switch(key) {
        case '\n':
            command_pending = true;
            // parse_command();
            // prompt();
            break;
        case 0x0E: // Backspace
            if(buffer_len > 0) {
                buffer_len--;
                buffer[buffer_len] = '\0';
                printf("\b");
            }
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

void Terminal::OnKeyUp(char key) {
    keyboard_statemap[(int)key] = false;
}

void Terminal::service() {
    if(!command_pending) return;
    command_pending = false;
    parse_command();
    prompt();
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
    else if(jackos::libc::strcmp(argv[0], "dlist") == 0) {
        printf("Reading disk...\n");
        floppy_driver -> dma_init(DIR_READ);
        floppy_driver -> do_track(FLOPPY_BASE, 0, DIR_READ);
        floppy_driver -> ParseFATHeader();
        floppy_driver -> read_root_directory();
        for(int i = 0; i < floppy_driver->get_file_count(); i++) {
            jackos::filesystem::FAT12DirectoryEntry file = floppy_driver->get_file(i);
            printf(file.name);
            printf("\n");
        }
    }
    else if(jackos::libc::strcmp(argv[0], "drun") == 0) {
        drun_file(argv[1]);
    }
    else {
        printf("Unknown command: ");
        printf(argv[0]);
        printf("\nRun [help] to see commands.\n");
    }
    
    buffer[0] = '\0';
    buffer_len = 0;
}

bool Terminal::check_key(char key) {
    return keyboard_statemap[(int)key];
}