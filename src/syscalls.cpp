#include <syscalls.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::hardware;
using namespace jackos::drivers;
using namespace jackos::terminal;
using namespace jackos::gui;

SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber)
: InterruptHandler(InterruptNumber + interruptManager->getHardwareOffset(), interruptManager)
{
    graphicsMode = false;
    graphics = nullptr;
    terminal = nullptr;
}

SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber, VideoGraphicsArray* i_graphics, Terminal* i_terminal, Desktop* i_desktop, void (*i_runtime_loop)(jackos::gui::Desktop*, jackos::drivers::VideoGraphicsArray*, jackos::terminal::Terminal*))
: InterruptHandler(InterruptNumber + interruptManager->getHardwareOffset(), interruptManager)
{
    graphicsMode = false;
    graphics = i_graphics;
    terminal = i_terminal;
    desktop = i_desktop;
}

SyscallHandler::~SyscallHandler() {

}

void printf(const char* str);

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp) {
    CPUState* cpu = (CPUState*)esp;

    // printf("EAX: ");
    // printaddr(cpu -> eax);
    // printf("\n");
    // printf("EBX: ");
    // printaddr(cpu -> ebx);
    // printf("\n");
    // printf("ECX: ");
    // printaddr(cpu -> ecx);
    // printf("\n");
    // printf("EDX: ");
    // printaddr(cpu -> edx);
    // printf("\n");
    // printf("ESI: ");
    // printaddr(cpu -> esi);
    // printf("\n");
    // printf("EDI: ");
    // printaddr(cpu -> edi);
    // printf("\n");
    // printf("EBP: ");
    // printaddr(cpu -> ebp);
    // printf("\n");
    // printf("error: ");
    // printaddr(cpu -> error);
    // printf("\n");
    // printf("eip: ");
    // printaddr(cpu -> eip);
    // printf("\n");
    // printf("cs: ");
    // printaddr(cpu -> cs);
    // printf("\n");
    // printf("eflags: ");
    // printaddr(cpu -> eflags);
    // printf("\n");
    // printf("esp: ");
    // printaddr(cpu -> esp);
    // printf("\n");
    // printf("ss: ");
    // printaddr(cpu -> ss);
    // printf("\n");

    printf("Message: ");
    printf((const char*)(cpu -> ecx));
    printf("\n");

    const char* ptr = (const char*)(cpu -> ecx);
    printf("Memory at ECX: ");
    for(int i = 0; i < 16; i++) {
        char c = ptr[i];
        if (c >= ' ' && c <= '~') {
            const char foo[2] = {c, '\0'};
            printf(foo);
        }
        else {
            printf("?");
        }
    }

    printf("SYS_PRINT arg test: ");
    printaddr(cpu->ecx);
    printf(" -> ");
    printf((const char*)cpu->ecx);
    printf("\n");

    // switch(cpu -> eax) {
    //     case 0:
    //         printf((const char*)(cpu -> ecx));
    //         break;
    //     default:
    //         break;
    // }
    return esp;
}