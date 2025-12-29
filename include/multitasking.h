#ifndef __jackos_multitasking_H
#define __jackos_multitasking_H

#include <common/types.h>
#include <gdt.h>

namespace jackos {
    struct CPUState {
        jackos::common::uint32_t edi;
        jackos::common::uint32_t esi;
        jackos::common::uint32_t ebp;
        jackos::common::uint32_t esp;
        jackos::common::uint32_t ebx;
        jackos::common::uint32_t edx;
        jackos::common::uint32_t ecx;
        jackos::common::uint32_t eax;

        jackos::common::uint32_t interrupt;
        jackos::common::uint32_t error;

        // jackos::common::uint32_t gs;
        // jackos::common::uint32_t fs;
        // jackos::common::uint32_t es;
        // jackos::common::uint32_t ds;


        jackos::common::uint32_t eip;
        jackos::common::uint32_t cs;
        jackos::common::uint32_t eflags;
        // jackos::common::uint32_t esp;
        // jackos::common::uint32_t ss;
    } __attribute__((packed));

    class Task {
        friend class TaskManager;
        private:
            jackos::common::uint8_t stack[4096]; // 4 KiB
            CPUState* cpustate;
        public:
            Task(GlobalDescriptorTable *gdt, void entrypoint());
            ~Task();
    };
    class TaskManager {
        private:
            Task* tasks[256];
            int numTasks;
            int currentTask;
        public:
            TaskManager();
            ~TaskManager();
            bool AddTask(Task* task);
            CPUState* Schedule(CPUState* cpustate);
    };
}

#endif