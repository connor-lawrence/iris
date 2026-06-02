#ifndef VGA_H
#define VGA_H

#include "types.h"

void vga_print_char(char character, u16 color, int x, int y);
void vga_write_char(char character, u16 color, int x, int y);
void vga_render();
void kernel_print_string(const char* str, u16 color, int x, int y);

#endif