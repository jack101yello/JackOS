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
    runtime_loop = i_runtime_loop;
}

SyscallHandler::~SyscallHandler() {

}

void printf(const char* str);
void printaddr(int n);

uint32_t SyscallHandler::HandleInterrupt(uint32_t esp) {
    CPUState* cpu = (CPUState*)esp;
    switch(cpu -> eax) {
        case 0: // Print
            printf((const char*)(cpu -> ecx));
            break;
        case 1: // Enter graphics mode
            if(graphicsMode || graphics == nullptr) break;
            graphicsMode = true;
            graphics -> SetMode(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DEPTH);
            desktop -> Draw(graphics);
            graphics -> DrawFrame(SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case 2: // Exit graphics mode
            if(!graphicsMode) break;
            graphicsMode = false;
            graphics -> SetTextMode();
            break;
        case 3: // Get key
            cpu -> ecx = terminal -> getLastKey();
            break;
        case 4: // printaddr
            printaddr(cpu -> ecx);
            break;
        case 5: // quit
            runtime_loop(desktop, graphics, terminal);
            break;
        default:
            break;
    }
    return esp;
}