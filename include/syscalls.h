#ifndef __jackos_syscalls_H
#define __jackos_syscalls_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <multitasking.h>
#include <filesystem/vfs.h>

#define S_IFCHR 0020000
#define ENOENT 2
#define ECHILD 10
#define ENOMEM 12
#define EAGAIN 11
#define EINVAL 22
#define EMLINK 31

namespace jackos {
    class SyscallHandler : public jackos::hardware::InterruptHandler {
        public:
            SyscallHandler(jackos::hardware::InterruptManager* interruptManager, jackos::common::uint8_t InterruptNumber);
            ~SyscallHandler();
            virtual jackos::common::uint32_t HandleInterrupt(jackos::common::uint32_t esp);

            void _printf(const char* str);
            void _exit();
            int _close(int file);
            char** _environ();
            int _execve(char* name, char** argv, char** env);
            int _fork();
            int _fstat(int file, struct stat* st);
            int _getpid();
            int _isatty(int file);
            int _kill(int pid, int sig);
            int _link(char* o, char* n);
            int _lseek(int file, int ptr, int dir);
            int _open(const char* name, int flags, int mode);
            int _read(int file, char* ptr, int len);
            void* _sbrk(int incr);
            int _stat(char* file, struct stat *st);
            int _times(struct tms *buf);
            int _unlink(char* name);
            int _wait(int* status);
            int _write(int file, char* ptr, int len);

        private:
            void _printf_background(const char* str); // Option 4
            void _exit_background();
            int _close_background(int file);
            char** _environ_background();
            int _execve_background(char* name, char** argv, char** env);
            int _open_background(const char* name, int flags, int mode);
            void* _sbrk_background(int incr);
    };

    struct stat {
        int st_mode;
    };

    struct tms {

    };
}

#endif