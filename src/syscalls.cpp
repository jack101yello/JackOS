#include <syscalls.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::hardware;

SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber)
: InterruptHandler(InterruptNumber + interruptManager->getHardwareOffset(), interruptManager)
{

}

SyscallHandler::~SyscallHandler() {

}

extern void kpanic(uint8_t interruptNumber, uint32_t esp);
extern void printf(const char* str);
extern void printfhex(int num);
extern int errno;

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp) {
    CPUState* cpu = (CPUState*)esp;
    switch(cpu -> eax) {
        case 1:
            _exit_background();
            break;
        case 2:
            cpu -> eax = (uint32_t)_environ_background();
        case 4:
            _printf_background((char*)cpu->ebx);
            break;
        case 5:
            cpu -> eax = _open_background((const char*)cpu->ebx, (int)cpu->ecx, (int)cpu->edx);
            break;
        case 6:
            cpu -> eax = _close_background((int)cpu->ebx);
            break;
        case 20:
            cpu -> eax = (uint32_t)_sbrk_background((int)cpu->ebx);
        default:
            kpanic(0x14, esp); // Invalid syscall
            break;
    }
    return esp;
}

// Print something to the screen.
void SyscallHandler::_printf(const char* str) {
    asm("int $0x80" : : "a" (4), "b" (str));
}

// Exit a program without cleaning up files.
void SyscallHandler::_exit() {
    asm("int $0x80" : : "a" (1));
}


// Close a file.
int SyscallHandler::_close(int file) {
    int status;
    asm("int $0x80" : "=a" (status) : "a" (6), "b" (file));
    return status;
}

// get a pointer to a list of environment variables.
char** SyscallHandler::_environ() {
    char** environ;
    asm("int $0x80" : "=a" (environ) : "a"(2));
    return environ;
}

// Transfer control to a new process.
int SyscallHandler::_execve(char* name, char** argv, char** env) {
    // We don't have processes, so this can be ignored until later.
    // asm("int $0x80" : : "a" (11), "b" (name), "c" (argv), "d" (env));
    errno = ENOMEM;
    return -1;
}

// Create a new process.
int SyscallHandler::_fork() {
    errno = EAGAIN;
    return -1;
}

// Get the status of an open file.
int SyscallHandler::_fstat(int file, struct stat *st) {
    st -> st_mode = S_IFCHR;
    return 0;
}

// Get the current process-ID.
int SyscallHandler::_getpid() {
    return 1;
}

// Check whether the output stream is a terminal.
int SyscallHandler::_isatty(int file) {
    return 1;
}

// Send a signal.
int SyscallHandler::_kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

// Rename a file.
int SyscallHandler::_link(char* o, char* n) {
    errno = EMLINK;
    return -1;
}

// Set position in file.
int SyscallHandler::_lseek(int file, int ptr, int dir) {
    return 0;
}

// Open a file.
int SyscallHandler::_open(const char* name, int flags, int mode) {
    int status;
    asm("int 0x80" : "=a" (status) : "a" (5), "b" (name), "c" (flags), "d" (mode));
    return status;
}

// Read from a file.
int SyscallHandler::_read(int file, char* ptr, int len) {
    return 0;
}

// Increase program data space.
void* SyscallHandler::_sbrk(int incr) {
    void* addr;
    asm("int 0x80" : "=a" (addr) : "a" (20), "b" (incr));
    return addr;
}

// Get the status of a file.
int SyscallHandler::_stat(char* file, struct stat *st) {
    st -> st_mode = S_IFCHR;
    return 0;
}

// Get timing information for the current process.
int SyscallHandler::_times(struct tms* buf) {
    return -1;
}

// Remove a file's directory entry.
int SyscallHandler::_unlink(char* name) {
    errno = ENOENT;
    return -1;
}

// Write to a file
int SyscallHandler::_write(int file, char *ptr, int len) {
    return len;
}

int SyscallHandler::_wait(int* status) {
    errno = ECHILD;
    return -1;
}

void SyscallHandler::_printf_background(const char* str) {
    printf(str);
}

void SyscallHandler::_exit_background() {
    /* The details of this functions are a little unclear,
    so their implementation is left for another time. */
    return;
}

int SyscallHandler::_close_background(int file) {
    struct jackos::filesystem::dirent* node = jackos::filesystem::readdir_fs(jackos::filesystem::fs_root, file);
    if(node == 0) {
        return -1; // Directory entry not found
    }
    jackos::filesystem::fs_node_t* fs_node = jackos::filesystem::finddir_fs(jackos::filesystem::fs_root, node -> name);
    if(fs_node == 0) {
        return -2; // File not found
    }
    jackos::filesystem::close_fs(fs_node);
    return 0;
}

char** SyscallHandler::_environ_background() {
    char* _env[1] = { 0 };
    return _env;
}

int SyscallHandler::_open_background(const char* name, int flags, int mode) {
    jackos::filesystem::fs_node_t* fs_node = jackos::filesystem::finddir_fs(jackos::filesystem::fs_root, (char*)name);
    jackos::filesystem::open_fs(fs_node, mode & 0x1, mode & 0x10); // The later arguments are questionable, but they'll work for now.
    return 0; // Our VFS doesn't presently return error codes in its implementation of open.
}

void* SyscallHandler::_sbrk_background(int incr) {
    extern char _end; // Defined by the linker
    static char* heap_end;
    char *prev_heap_end;
    if(heap_end == 0) {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;
    // Should check here for heap/stack collision, but I don't know how.
    heap_end += incr;
    return (void*) prev_heap_end;
}
