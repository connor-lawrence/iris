#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

void print_string(const char* string, u16 color, int x, int y);
void scroll_buffer();
void render();

#endif