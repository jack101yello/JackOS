#include <syscalls.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::hardware;
using namespace jackos::drivers;
using namespace jackos::terminal;

SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber)
: InterruptHandler(InterruptNumber + interruptManager->getHardwareOffset(), interruptManager)
{
    graphicsMode = false;
    graphics = nullptr;
    terminal = nullptr;
}

SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber, VideoGraphicsArray* i_graphics, Terminal* i_terminal)
: InterruptHandler(InterruptNumber + interruptManager->getHardwareOffset(), interruptManager)
{
    graphicsMode = false;
    graphics = i_graphics;
    terminal = i_terminal;
}

SyscallHandler::~SyscallHandler() {

}

void printf(const char* str);

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp) {
    CPUState* cpu = (CPUState*)esp;
    switch(cpu -> eax) {
        case 4: // print
            (graphics != nullptr) ? terminal -> print((char*)cpu -> ebx) : printf((char*)cpu -> ebx);
            break;
        case 5: // draw frame
            graphics -> DrawFrame(SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case 6: // draw pixel
            graphics -> PutPixel((int32_t)cpu->ebx, (int32_t)cpu->ecx, (COLOR_CODE)cpu->edx);
            break;
        default:
            break;
    }
    return esp;
}