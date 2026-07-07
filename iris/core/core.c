#include "core.h"
#include "types.h"
#include "cub.h"

void kernel_main(BootInfo* boot_info) {
    
    // Rainbow
    /*
    int i = 24;
    for (int x = framebuffer_width / 100; x < framebuffer_width * 99 / 100; x++) {
        for (int y = framebuffer_width / 100; y < framebuffer_height - (framebuffer_width / 100); y++) {
            if ((x + y) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0xFF0000;
            } else if ((x + y + i) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0xFFFF00;
            } else if ((x + y + (i * 2)) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0x00FF00;
            } else if ((x + y + (i * 3)) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0x00FFFF;
            } else if ((x + y + (i * 4)) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0x8080FF;
            } else if ((x + y + (i * 5)) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0xFF00FF;
            } else if ((x - y) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0xFF0000;
            } else if ((x - y + i) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0xFFFF00;
            } else if ((x - y + (i * 2)) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0x00FF00;
            } else if ((x - y + (i * 3)) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0x00FFFF;
            } else if ((x - y + (i * 4)) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0x8080FF;
            } else if ((x - y + (i * 5)) % (i * 6) == 0) {
                framebuffer_address[y * framebuffer_pitch + x] = 0xFF00FF;
            } else {
                framebuffer_address[y * framebuffer_pitch + x] = 0x000000;
            }
        }
    }
    */

    while(1) { __asm__ volatile("hlt"); } 
}