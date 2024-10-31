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

void printf(const char* str);

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp) {
    CPUState* cpu = (CPUState*)esp;
    switch(cpu -> eax) {
        case 4:
            printf((char*)cpu->ebx);
            break;
        default:
            break;
    }
    return esp;
}