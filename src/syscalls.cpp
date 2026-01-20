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
    switch((syscall_label)cpu -> eax) {
        case PRINT: // Print
            printf((const char*)(cpu -> ecx));
            break;
        case ENTER_GRAPHICS_MODE: // Enter graphics mode
            if(graphicsMode || graphics == nullptr) break;
            graphicsMode = true;
            graphics -> SetMode(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DEPTH);
            desktop -> Draw(graphics);
            graphics -> DrawFrame(SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case EXIT_GRAPHICS_MODE: // Exit graphics mode
            if(!graphicsMode) break;
            graphicsMode = false;
            graphics -> SetTextMode();
            break;
        case DRAW_FRAME:
            graphics -> DrawFrame(SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case GET_KEY: // Get key
            cpu -> ecx = terminal -> getLastKey();
            break;
        case PRINT_ADDR: // printaddr
            printaddr(cpu -> ecx);
            break;
        case QUIT: // quit
            if(graphicsMode) {
                graphicsMode = false;
                graphics -> SetTextMode();
            }
            runtime_loop(desktop, graphics, terminal);
            break;
        case PUT_PIXEL:{
            int32_t putx = (int32_t)cpu -> ebx;
            int32_t puty = (int32_t)cpu -> ecx;
            jackos::drivers::COLOR_CODE color = (jackos::drivers::COLOR_CODE)cpu -> edx;
            graphics -> PutPixel(putx, puty, color);
            break;
        }
        default:
            break;
    }
    return esp;
}