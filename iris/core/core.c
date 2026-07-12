#include "core.h"
#include "types.h"
#include "cub.h"

/////////////////////////////////////////////////////////////////////////////////////

static void debug_putchar(char c) {
    __asm__ volatile (
        "mov %0, %%al\n\t"
        "mov $0xe9, %%dx\n\t"
        "out %%al, %%dx\n\t"
        :
        : "r"(c)
        : "rax", "rdx"
    );
}

static void debug_print_u64(u64 number) {
    char buffer[20];
    int i = 0;

    if (number == 0) {
        debug_putchar('0');
        return;
    }

    while (number > 0) {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }

    while (i > 0) {
        debug_putchar(buffer[--i]);
    }
}

void kernel_main(BootInfo *boot_info) {

    debug_putchar('[');
    debug_putchar('I');
    debug_putchar('r');
    debug_putchar('i');
    debug_putchar('s');
    debug_putchar(']');
    debug_putchar('\n');

    debug_print_u64(boot_info->framebuffer.width);
    debug_putchar('\n');

    debug_print_u64(boot_info->framebuffer.height);
    debug_putchar('\n');

    while (1) {
        __asm__ volatile ("hlt");
    }
}