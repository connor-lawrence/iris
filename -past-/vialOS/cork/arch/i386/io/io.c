#include "io.h"
#include "types.h"

u8 i386_inb(u16 port) {
    unsigned char result;

    asm volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );

    return result;
}

void i386_outb(u16 port, u8 value) {
    asm volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}