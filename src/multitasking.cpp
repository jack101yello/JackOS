#include <multitasking.h>

using namespace jackos;
using namespace jackos::common;

Task::Task(GlobalDescriptorTable *gdt, void entrypoint()) {
    cpustate = (CPUState*)(stack + 4096 - sizeof(CPUState));
    cpustate -> eax = 0;
    cpustate -> ebx = 0;
    cpustate -> ecx = 0;
    cpustate -> edx = 0;
    cpustate -> esi = 0;
    cpustate -> edi = 0;
    cpustate -> ebp = 0;
    cpustate -> es = gdt -> UserDataSegmentSelector();
    cpustate -> fs = gdt -> UserDataSegmentSelector();
    cpustate -> es = gdt -> UserDataSegmentSelector();
    cpustate -> ds = gdt -> UserDataSegmentSelector();
    // cpustate -> esp = ;
    cpustate -> eip = (uint32_t)entrypoint;
    cpustate -> cs = gdt -> UserCodeSegmentSelector();
    // cpustate -> ss = ;
    cpustate -> eflags = 0x202;
}

Task::~Task() {
    
}

TaskManager::TaskManager() {
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager() {

}

bool TaskManager::AddTask(Task* task) {
    if(numTasks >= 256) {
        return false;
    }
    tasks[numTasks++] = task;
    return true;
}

CPUState* TaskManager::Schedule(CPUState* cpustate) {
    if(numTasks <= 0) {
        return cpustate;
    }
    if(currentTask >= 0) {
        tasks[currentTask] -> cpustate = cpustate;
    }
    if(++currentTask >= numTasks) {
        currentTask %= numTasks;
    }
    return tasks[currentTask] -> cpustate;
}