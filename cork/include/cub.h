#ifndef CUB_H
#define CUB_H

#include "types.h"

typedef struct {

    struct {
        u64 address;
        u32 width;
        u32 height;
        u32 pitch;
    } framebuffer;

} BootInfo;

typedef void (*kernel_entry_t)(BootInfo*);

#endif