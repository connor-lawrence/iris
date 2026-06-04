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
    int i = 24;
    for (int x = width / 100; x < width * 99 / 100; x++) {
        for (int y = width / 100; y < height - (width / 100); y++) {
            if ((x + y) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0xFF0000;
            } else if ((x + y + i) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0xFFFF00;
            } else if ((x + y + (i * 2)) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x00FF00;
            } else if ((x + y + (i * 3)) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x00FFFF;
            } else if ((x + y + (i * 4)) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x8080FF;
            } else if ((x + y + (i * 5)) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0xFF00FF;
            } else if ((x - y) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0xFF0000;
            } else if ((x - y + i) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0xFFFF00;
            } else if ((x - y + (i * 2)) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x00FF00;
            } else if ((x - y + (i * 3)) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x00FFFF;
            } else if ((x - y + (i * 4)) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x8080FF;
            } else if ((x - y + (i * 5)) % (i * 6) == 0) {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0xFF00FF;
            } else {
                framebuffer[y * boot_info->framebuffer.pitch + x] = 0x000000;
            }
        }
    }

    while(1) { __asm__ volatile("hlt"); } 
}