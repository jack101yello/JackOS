#include "keyboard.h"

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager),
dataport(0x60),
commandport(0x64)
{
    
}

KeyboardDriver::~KeyboardDriver() {

}

void KeyboardDriver::Activate() {
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

void printf(const char* str);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp) {
    uint8_t key = dataport.Read();
    
    static bool Shift = false;
    switch(key) {
        case 0x01: break; // esc
        case 0x02: if(Shift) printf("!"); else printf("1"); break;
        case 0x03: if(Shift) printf("@"); else printf("2"); break;
        case 0x04: if(Shift) printf("#"); else printf("3"); break;
        case 0x05: if(Shift) printf("$"); else printf("4"); break;
        case 0x06: if(Shift) printf("%"); else printf("5"); break;
        case 0x07: if(Shift) printf("^"); else printf("6"); break;
        case 0x08: if(Shift) printf("&"); else printf("7"); break;
        case 0x09: if(Shift) printf("*"); else printf("8"); break;
        case 0x0A: if(Shift) printf("("); else printf("9"); break;
        case 0x0B: if(Shift) printf(")"); else printf("0"); break;
        case 0x0C: if(Shift) printf("_"); else printf("-"); break;
        case 0x0D: if(Shift) printf("+"); else printf("="); break;
        case 0x0E: break; // Backspace
        case 0x0F: printf("\t"); break;
        case 0x10: if(Shift) printf("Q"); else printf("q"); break;
        case 0x11: if(Shift) printf("W"); else printf("w"); break;
        case 0x12: if(Shift) printf("E"); else printf("e"); break;
        case 0x13: if(Shift) printf("R"); else printf("r"); break;
        case 0x14: if(Shift) printf("T"); else printf("t"); break;
        case 0x15: if(Shift) printf("Y"); else printf("y"); break;
        case 0x16: if(Shift) printf("U"); else printf("u"); break;
        case 0x17: if(Shift) printf("I"); else printf("i"); break;
        case 0x18: if(Shift) printf("O"); else printf("o"); break;
        case 0x19: if(Shift) printf("P"); else printf("p"); break;
        case 0x1a: if(Shift) printf("{"); else printf("["); break;
        case 0x1b: if(Shift) printf("}"); else printf("]"); break;
        case 0x1c: printf("\n"); break; // Enter
        case 0x1d: break; // Left ctrl
        case 0x1e: if(Shift) printf("A"); else printf("a"); break;
        case 0x1f: if(Shift) printf("S"); else printf("s"); break;
        case 0x20: if(Shift) printf("D"); else printf("d"); break;
        case 0x21: if(Shift) printf("F"); else printf("f"); break;
        case 0x22: if(Shift) printf("G"); else printf("g"); break;
        case 0x23: if(Shift) printf("H"); else printf("h"); break;
        case 0x24: if(Shift) printf("J"); else printf("j"); break;
        case 0x25: if(Shift) printf("K"); else printf("k"); break;
        case 0x26: if(Shift) printf("L"); else printf("l"); break;
        case 0x27: if(Shift) printf(":"); else printf(";"); break;
        case 0x28: if(Shift) printf("\""); else printf("'"); break;
        case 0x29: if(Shift) printf("~"); else printf("`"); break;
        case 0x2b: if(Shift) printf("|"); else printf("\\"); break;
        case 0x2c: if(Shift) printf("Z"); else printf("Z"); break;
        case 0x2d: if(Shift) printf("X"); else printf("x"); break;
        case 0x2e: if(Shift) printf("C"); else printf("c"); break;
        case 0x2f: if(Shift) printf("V"); else printf("v"); break;
        case 0x30: if(Shift) printf("B"); else printf("b"); break;
        case 0x31: if(Shift) printf("N"); else printf("n"); break;
        case 0x32: if(Shift) printf("M"); else printf("m"); break;
        case 0x33: if(Shift) printf("<"); else printf(","); break;
        case 0x34: if(Shift) printf(">"); else printf("."); break;
        case 0x35: if(Shift) printf("?"); else printf("/"); break;
        case 0x39: printf(" "); break;

        case 0x2A: case 0x36: Shift = true; break; // Shift keys
        case 0xAA: case 0xB6: Shift = false; break; // Release shift keys

        case 0x45: break; // Numlock
        case 0xFA: break; // ACK
        default:
            if(key < 0x80) {
                char msg[] = "KEYBOARD 0x00 ";
                char hex[] = "0123456789ABCDEF";
                msg[11] = hex[(key >> 4) & 0x0F];
                msg[12] = hex[key & 0x0F];
                printf(msg);
            }
    }
    return esp;
}