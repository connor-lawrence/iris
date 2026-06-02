#include "vga.h"
#include "types.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile u16*)0xB8000)

static char vga_screen_text_buffer[VGA_HEIGHT][VGA_WIDTH];
static u16 vga_screen_color_buffer[VGA_HEIGHT][VGA_WIDTH];

void vga_print_char(char character, u16 color, int x, int y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    vga_screen_text_buffer[y][x] = character;
    vga_screen_color_buffer[y][x] = color;
}

void vga_write_char(char character, u16 color, int x, int y) {
    VGA_MEMORY[y * VGA_WIDTH + x] = (color << 8) | character;
}

void vga_render() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_write_char(vga_screen_text_buffer[y][x], vga_screen_color_buffer[y][x], x, y);
        }
    }
}

// Kernel only!

void kernel_print_string(const char* str, u16 color, int x, int y) {
    int cursor_x = x;
    while (*str) {
        vga_print_char(*str++, color, cursor_x++, y);
    }
}