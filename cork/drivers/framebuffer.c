/*#include "framebuffer.h"
#include "types.h"
#include "cub.h"

static u32* framebuffer_address = 0;
static int framebuffer_width = 0;
static int framebuffer_height = 0;
static int framebuffer_pitch = 0;

//#define BUFFER_SIZE (1024 * 768)
//static u32 buffer[BUFFER_SIZE];

void init_framebuffer(BootInfo* boot_info) {

    framebuffer_address = (u32*)boot_info->framebuffer.address;
    framebuffer_width = boot_info->framebuffer.width;
    framebuffer_height = boot_info->framebuffer.height;
    framebuffer_pitch = boot_info->framebuffer.pitch;

}

void framebuffer_put_pixel(int x, int y, u32 color) {
    //buffer[y * framebuffer_width + x] = color;
}

void framebuffer_write_pixel(int x, int y, u32 color) {
    framebuffer_address[y * framebuffer_pitch + x] = color;
}

int clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void framebuffer_fill_rectangle(int start_x, int end_x, int start_y, int end_y, u32 color) {
    int s_x = clamp(start_x, 0, framebuffer_width);
    int e_x = clamp(end_x, 0, framebuffer_width);
    int s_y = clamp(start_y, 0, framebuffer_height);
    int e_y = clamp(end_y, 0, framebuffer_height);
    for (int x = s_x; x < e_x; x++) {
        for (int y = s_y; y < e_y; y++) {
            framebuffer_write_pixel(x, y, color);
        }
    }
}

void framebuffer_render() {
    for (int x = 0; x < framebuffer_width; x++) {
        for (int y = 0; y < framebuffer_height; y++) {
            //framebuffer_write_pixel(x, y, buffer[y * framebuffer_width + x]);
        }
    }
}

int get_framebuffer_width() {
    return framebuffer_width;
}

int get_framebuffer_height() {
    return framebuffer_height;
}*/