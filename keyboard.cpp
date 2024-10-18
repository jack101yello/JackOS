#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager),
dataport(0x60),
commandport(0x64)
{
    while(commandport.Read() & 0x1) {
        dataport.Read();
    }
    commandport.Write(0xAE); // Activate interrupts
    commandport.Write(0x20); // Get current state
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    commandport.Write(0x60); // Set state
    dataport.Write(status);

    dataport.Write(0xF4);
}

KeyboardDriver::~KeyboardDriver() {

}

void printf(const char* str);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp) {
    uint8_t key = dataport.Read();
    if(key < 0x80) {
        switch(key) {
            case 0x01: break; // esc
            case 0x02: printf("1"); break;
            case 0x03: printf("2"); break;
            case 0x04: printf("3"); break;
            case 0x05: printf("4"); break;
            case 0x06: printf("5"); break;
            case 0x07: printf("6"); break;
            case 0x08: printf("7"); break;
            case 0x09: printf("8"); break;
            case 0x0A: printf("9"); break;
            case 0x0B: printf("0"); break;
            case 0x0C: printf("-"); break;
            case 0x0D: printf("="); break;
            case 0x0E: break; // Backspace
            case 0x0F: printf("\t"); break;
            case 0x10: printf("Q"); break;
            case 0x11: printf("W"); break;
            case 0x12: printf("E"); break;
            case 0x13: printf("R"); break;
            case 0x14: printf("T"); break;
            case 0x15: printf("Y"); break;
            case 0x16: printf("U"); break;
            case 0x17: printf("I"); break;
            case 0x18: printf("O"); break;
            case 0x19: printf("P"); break;
            case 0x1a: printf("["); break;
            case 0x1b: printf("]"); break;
            case 0x1c: printf("\n"); break;
            case 0x1d: break; // Left ctrl
            case 0x1e: printf("A"); break;
            case 0x1f: printf("S"); break;
            case 0x20: printf("D"); break;
            case 0x21: printf("F"); break;
            case 0x22: printf("G"); break;
            case 0x23: printf("H"); break;
            case 0x24: printf("J"); break;
            case 0x25: printf("K"); break;
            case 0x26: printf("L"); break;
            case 0x27: printf(";"); break;
            case 0x28: printf("'"); break;
            case 0x29: printf("`"); break;
            case 0x2a: break; // Left shift pressed
            case 0x2b: printf("\\"); break;
            case 0x2c: printf("Z"); break;
            case 0x2d: printf("X"); break;
            case 0x2e: printf("C"); break;
            case 0x2f: printf("V"); break;
            case 0x30: printf("B"); break;
            case 0x31: printf("N"); break;
            case 0x32: printf("M"); break;
            case 0x33: printf(","); break;
            case 0x34: printf("."); break;
            case 0x35: printf("/"); break;
            case 0x39: printf(" "); break;

            case 0xFA: break; // ACK
            default:
                char msg[] = "KEYBOARD 0x00 ";
                char hex[] = "0123456789ABCDEF";
                msg[11] = hex[(key >> 4) & 0x0F];
                msg[12] = hex[key & 0x0F];
                printf(msg);
        }
    }
    return esp;
}