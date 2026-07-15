#pragma once
#include "types.h"
#include "boot.h"

void framebuffer_init(Framebuffer framebuffer_info);
void framebuffer_put_pixel(u32 x, u32 y, u32 color);
