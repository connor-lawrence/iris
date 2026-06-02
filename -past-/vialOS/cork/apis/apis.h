#ifndef APIS_H
#define APIS_H

#include "types.h"

void kernel_print_char(char character, u16 color, int x, int y);
void kernel_render();

u8 inb(u16 port);
void outb(u16 port, u8 value);

#endif