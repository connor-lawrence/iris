#ifndef TYPES_H
#define TYPES_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char i8;
typedef signed short i16;
typedef signed int i32;

typedef u32 uintptr_t;
typedef i32 intptr_t;

typedef u32 size_t;

#define bool u8
#define true 1
#define false 0

#define NULL ((void*)0)

typedef u32 phys_addr_t;
typedef u32 virt_addr_t;

typedef u8 byte;
typedef u16 word;
typedef u32 dword;

// TEMPORARY!!!

typedef enum {
    VGA_BLACK = 0x00,
    VGA_BLUE = 0x01,
    VGA_GREEN = 0x02,
    VGA_CYAN = 0x03,
    VGA_RED = 0x04,
    VGA_MAGENTA = 0x05,
    VGA_BROWN = 0x06,
    VGA_LIGHT_GRAY = 0x07,
    VGA_DARK_GRAY = 0x08,
    VGA_LIGHT_BLUE = 0x09,
    VGA_LIGHT_GREEN = 0x0A,
    VGA_LIGHT_CYAN = 0x0B,
    VGA_LIGHT_RED = 0x0C,
    VGA_LIGHT_MAGENTA = 0x0D,
    VGA_YELLOW = 0x0E,
    VGA_WHITE = 0x0F
} vga_color;

#endif