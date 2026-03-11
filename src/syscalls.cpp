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

SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber, VideoGraphicsArray* i_graphics, Terminal* i_terminal, Desktop* i_desktop, void (*i_runtime_loop)(jackos::gui::Desktop*, jackos::drivers::VideoGraphicsArray*, jackos::terminal::Terminal*), CDROMDriver* i_cdrom_driver)
: InterruptHandler(InterruptNumber + interruptManager->getHardwareOffset(), interruptManager)
{
    graphicsMode = false;
    graphics = i_graphics;
    terminal = i_terminal;
    desktop = i_desktop;
    runtime_loop = i_runtime_loop;
	cdrom_driver = i_cdrom_driver;
}

SyscallHandler::~SyscallHandler() {

}

void printf(const char* str);
void printaddr(int n);

extern MemoryManager kmm;

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
        case CHECK_KEY: {// Get key
            char key = (char)cpu -> ebx;
            cpu -> ecx = terminal -> check_key(key);
            break;
        }
        case PRINT_ADDR: // printaddr
            printaddr(cpu -> ecx);
            break;
        case QUIT: // quit
            printf("Exit syscall received!\n");
            if(graphicsMode) {
                graphicsMode = false;
                graphics -> SetTextMode();
            }
            runtime_loop(desktop, graphics, terminal);
            while(1) asm volatile("hlt");
            break;
        case PUT_PIXEL:{
            int32_t putx = (int32_t)cpu -> ebx;
            int32_t puty = (int32_t)cpu -> ecx;
            jackos::drivers::COLOR_CODE color = (jackos::drivers::COLOR_CODE)cpu -> edx;
            graphics -> PutPixel(putx, puty, color);
            break;
        }
        case MALLOC:{
            uint32_t size = cpu -> ebx;
            cpu -> ecx = (uint32_t)kmm.malloc(size);
            break;
        }
        case FREE:{
            void* ptr = (void*)cpu -> ebx;
            kmm.free(ptr);
            break;
        }
        case CALLOC: // To be implemented in the memory manager
            break;
        case REALLOC: // To be implemented in the memory manager
            break;
        case SWAP_FRAMEBUFFER:
            graphics -> SwapFramebuffer((uint32_t*)cpu -> ebx);
            break;
        case FOPEN:{
            break;
        }
        case FCLOSE:{
            break;
        }
        default:
            break;
    }
    return esp;
}
