#include "core.h"
#include "types.h"
#include "cub.h"

__attribute__((ms_abi)) void kernel_main(void) {
    __asm__ volatile (
        "mov $0xe9, %%dx\n\t"  // Load QEMU Debug Port into DX register
        
        "mov $91, %%al\n\t"     // '['
        "out %%al, %%dx\n\t"

        "mov $73, %%al\n\t"     // 'I'
        "out %%al, %%dx\n\t"

        "mov $114, %%al\n\t"    // 'r'
        "out %%al, %%dx\n\t"

        "mov $105, %%al\n\t"    // 'i'
        "out %%al, %%dx\n\t"

        "mov $115, %%al\n\t"    // 's'
        "out %%al, %%dx\n\t"

        "mov $93, %%al\n\t"     // ']'
        "out %%al, %%dx\n\t"
        
        "mov $10, %%al\n\t"    // Newline '\n'
        "out %%al, %%dx\n\t"
        :
        :
        : "rax", "rdx"         // Tell compiler we are modifying these registers
    );
    while (1) {
        __asm__ volatile ("hlt");
    }
}