#include "core.h"
#include "types.h"
#include "cub.h"

void kernel_main() {
   volatile unsigned short *vga = (unsigned short *)0xB8000;

    const char *msg = "Hello from Iris!";

    for (int i = 0; msg[i] != '\0'; i++) {
        vga[i] = (unsigned short)msg[i] | (0x07 << 8);
    }

    while (1) {
        __asm__ volatile ("hlt");
    }
}