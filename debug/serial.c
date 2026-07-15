#include "serial.h"
#include "types.h"

void serial_putc(char c) {
    __asm__ volatile ("outb %%al, $0xE9" : : "a"(c));
}

void serial_print(const char *str) {
    while (*str) {
        serial_putc(*str);
        str++;
    }
}

void serial_print_int(const char *before, const u64 x, const char *after) {
    serial_print(before);
    u64 n = x;
    char buffer[21];
    u32 i = 0;
    if (!x) {
        serial_putc('0');
    }
    while (n) {
        buffer[i] = (char)(n % 10) + 48;
        n /= 10;
        i++;
    }
    while (i) {
        serial_putc(buffer[--i]);
    }
    serial_print(after);
}

void serial_print_hex(const char *before, const u64 x, const char *after) {
    serial_print(before);
    serial_print("0x");
    u32 digit = 0;
    for (int i = 15; i >= 0; i--) {
        digit = (x >> (i * 4)) & 0xF;
        if (digit <= 9) {
            serial_putc((char)digit + 48);
        } else {
            serial_putc((char)digit + 55);
        }
    }
    serial_print(after);
}
