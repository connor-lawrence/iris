#include "core.h"
#include "types.h"
#include "cub.h"

void kernel_main(BootInfo *boot_info) {

    u32 *framebuffer = (u32 *)boot_info->framebuffer.address;

    // Clear screen
    for (u32 y = 0; y < boot_info->framebuffer.height; y++) {
        for (u32 x = 0; x < boot_info->framebuffer.width; x++) {
            framebuffer[y * boot_info->framebuffer.pitch + x] = 0;
        }
    }

    // Halt
    while (1) {
        __asm__ volatile ("hlt");
    }
}
