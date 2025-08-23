#include <drivers/pit.h>

using namespace jackos;
using namespace jackos::common;
using namespace jackos::drivers;
using namespace jackos::hardware;

extern void printf(const char* msg);

PITEventHandler::PITEventHandler() {
    increment = 0;
}

void PITEventHandler::tick() {
    ++increment;
}

uint32_t PITEventHandler::get_increment() {
    return increment;
}

uint32_t PITEventHandler::internal_wait(uint32_t start_time, uint32_t duration) {
    while(increment < start_time + duration) {
        asm __volatile__("hlt");
    }
    return 0;
}

// Waits for duration, in seconds
uint32_t PITEventHandler::wait(uint32_t duration) {
    return internal_wait(increment, duration);
}

PITDriver::PITDriver(InterruptManager* manager, PITEventHandler* handler) : InterruptHandler(0x20, manager) {
    this -> handler = handler;
}

PITDriver::~PITDriver() {

}

void PITDriver::Activate() {

}

uint32_t PITDriver::HandleInterrupt(uint32_t esp) {
    handler->tick();
    return esp;
}