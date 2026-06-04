#include "core.h"
#include "types.h"

void kernel_main(BootInfo* boot_info) {
    u32* framebuffer = (u32*)boot_info->framebuffer.address;
    framebuffer[5] = 0xFFFFFF;
    while(1) { __asm__ volatile("hlt"); } 
}