#ifndef IO_H
#define IO_H

#include "types.h"

u8 i386_inb(u16 port);
void i386_outb(u16 port, u8 value);

#endif