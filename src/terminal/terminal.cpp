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
	// Set up the ramdisk
	// First we have to allocate the necessary space for the files
	//kfree(jackos::filesystem::file_headers);
	//kfree(jackos::filesystem::root_nodes);
	jackos::filesystem::initrd_header = (jackos::filesystem::initrd_header_t*)kmalloc(sizeof(jackos::filesystem::initrd_header_t));
	jackos::filesystem::initrd_header -> nfiles = 0;
	jackos::filesystem::file_headers = (jackos::filesystem::initrd_file_header_t*)kmalloc(sizeof(jackos::filesystem::initrd_file_header_t) * mb->mods_count); // Allocate the necessary space for file headers
	jackos::filesystem::root_nodes = (jackos::filesystem::fs_node_t*)kmalloc(sizeof(jackos::filesystem::fs_node_t) * mb->mods_count); // Allocate the necessary space for the files
	multiboot_module_t* modules = (multiboot_module_t*) mb -> mods_addr;

	for(int i = 0; i < mb -> mods_count; i++) {
		// Create a new file header
		jackos::filesystem::initrd_file_header_t new_file_header;
		jackos::libc::strcpy(new_file_header.name, (char*)modules[i].string);
		new_file_header.length = modules[i].mod_end - modules[i].mod_start;
		new_file_header.offset = sizeof(jackos::filesystem::initrd_header) + sizeof(int); // This may not be right, but I don't think it's used anyway
		jackos::filesystem::file_headers[i] = new_file_header;
		jackos::filesystem::initrd_header -> nfiles++;

		// Create a new file
		jackos::filesystem::fs_node new_file;
		jackos::libc::strcpy(new_file.name, new_file_header.name);
		new_file.read = jackos::filesystem::initrd_read; // The read method for ramdisk files, but this won't generally get used because we can just look at the memory address
		new_file.extent_location = modules[i].mod_start; // The memory address of the file
		new_file.inode = i;
		jackos::filesystem::root_nodes[i] = new_file;

		printf("Added command [ "); printf(new_file.name); printf(" ]\n");
	}
}

void Terminal::initialize() {
    active = true;
    buffer[0] = '\0';
    buffer_len = 0;
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

	// Search the files in the ramdisk to see if one should be run
	for(int i = 0; i < jackos::filesystem::initrd_header -> nfiles; i++) {
		if(jackos::libc::weakstrcmp(argv[0], jackos::filesystem::root_nodes[i].name) == 0) {
			// Construct an ELF executable and run it
			jackos::filesystem::elf::Elf_File elf_command((Elf_Ehdr*)jackos::filesystem::root_nodes[i].extent_location, gdt);
			if(!elf_command.check_file()) {
				return;
			}
			elf_command.run();
		}
	}

	buffer[0] = '\0';
	buffer_len = 0;
}

bool Terminal::check_key(char key) {
    return keyboard_statemap[(int)key];
}
