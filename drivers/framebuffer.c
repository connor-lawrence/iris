#include "framebuffer.h"
#include "types.h"
#include "boot.h"

static Framebuffer framebuffer;

void framebuffer_init(Framebuffer framebuffer_info) {

    framebuffer = framebuffer_info;

    // Clear screen
    for (u32 y = 0; y < framebuffer.height; y++) {
        for (u32 x = 0; x < framebuffer.width; x++) {
            framebuffer_put_pixel(x, y, 0);
        }
    }

}

void framebuffer_put_pixel(u32 x, u32 y, u32 color) {

    if (x >= framebuffer.width || y >= framebuffer.height) {
        return;
    }

    // Separates color into red green and blue
    u32 red = (color >> 16) & 0xFF;
    u32 green = (color >> 8) & 0xFF;
    u32 blue = (color >> 0) & 0xFF;

    switch(framebuffer.pixel_format) {
        case PIXEL_FORMAT_RGB:
            // Rearrange color so framebuffer (little endian) memory is in RGB
            color = (blue << 16) | (green << 8) | (red << 0);
            break;

        case PIXEL_FORMAT_BGR:
            // Rearrange color so framebuffer (little endian) memory is in BGR
            color = (red << 16) | (green << 8) | (blue << 0);
            break;

        default:
            return;
    }

    ((u32 *)framebuffer.base)[y * framebuffer.pixels_per_scanline + x] = color;

}
