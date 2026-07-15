#pragma once
#include "types.h"

typedef struct {
    u64 kernel_base;
    u64 kernel_size;
    void *map;
    u64 map_size;
    u64 map_descriptor_size;
    u32 map_descriptor_version;
    u64 map_key;
} Memory;

typedef struct {
    void *base;
    u32 width;
    u32 height;
    u32 pixels_per_scanline;
    u32 pixel_format;
} Framebuffer;

enum PixelFormat {
    PIXEL_FORMAT_RGB = 0,
    PIXEL_FORMAT_BGR = 1,
    PIXEL_FORMAT_BITMASK = 2,
    PIXEL_FORMAT_BLT_ONLY = 3
};

typedef struct {
    Memory memory;
    Framebuffer framebuffer;
} BootInfo;
