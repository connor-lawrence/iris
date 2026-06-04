#include "core.h"
#include "types.h"
#include "cub.h"

void kernel_main(BootInfo* boot_info) {
    
    u32* framebuffer = (u32*)boot_info->framebuffer.address;
    int width = boot_info->framebuffer.width;
    int height = boot_info->framebuffer.height;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            framebuffer[y * boot_info->framebuffer.pitch + x] = 0x000000;
        }
    }
    for (int x = width / 10; x < width * 9 / 10; x++) {
        for (int y = height / 10; y < height * 9 / 10; y++) {
            if ((x + y) % 60 == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0xFF0000;
            } else if ((x + y + 10) % 60 == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0xFFFF00;
            } else if ((x + y + 20) % 60 == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x00FF00;
            } else if ((x + y + 30) % 60 == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x00FFFF;
            } else if ((x + y + 40) % 60 == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x0000FF;
            } else if ((x + y + 50) % 60 == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0xFF00FF;
            } else {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x000000;
            }
        }
    }

    while(1) { __asm__ volatile("hlt"); } 
}