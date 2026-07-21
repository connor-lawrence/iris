#pragma once
#include "types.h"
#include "boot.h"

typedef struct {
    u32 type;
    u32 pad;
    u64 physical_start;
    u64 virtual_start;
    u64 number_of_pages;
    u64 attribute;
} MemoryMapDescriptor;

typedef struct {
    u64 base;
    u64 size;
} MemoryRegion;

void memory_init(Memory *memory);
void* physical_allocate_page(void);
