#include "apis.h"
#include "types.h"

#include "../drivers/vga/vga.h"
void kernel_print_char(char character, u16 color, int x, int y) { vga_print_char(character, color, x, y); }
void kernel_render() { vga_render(); }

#include "../arch/i386/i386.h"
u8 inb(u16 port) { i386_inb(port); }
void outb(u16 port, u8 value) { i386_outb(port, value); }