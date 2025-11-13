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
    switch(cpu -> eax) {
        case 0: // exit
            while(desktop -> getNumChildren() > 0) {
                desktop -> RemoveChild(0);
            }
            runtime_loop(desktop, graphics, terminal);
            break;
        case 1: { // create window
            Window window(desktop, 50, 50, 100, 100, 0xFF, 0xFF, 0xFF);
            desktop -> AddChild(&window);
            break;
        }
        case 2: { // get key
            char* key = (char*)cpu -> ebx;
            *key = terminal -> getLastKey();
            break;
        }
        case 4: // print
            (graphics != nullptr) ? terminal -> print((char*)cpu -> ebx) : printf((char*)cpu -> ebx);
            break;
        case 5: // draw frame
            // desktop -> Draw(graphics);
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