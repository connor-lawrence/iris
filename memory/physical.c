#include "physical.h"
#include "types.h"
#include "boot.h"

#include "serial.h"

#define PAGE_SIZE 4096
#define MAX_REGIONS 128
#define EFI_CONVENTIONAL_MEMORY 7

static MemoryRegion memory_regions[MAX_REGIONS];
static u64 region_count = 0;

// For bump allocator
static u64 current_region = 0;
static u64 current_offset = 0;

static void parse_memory_map(Memory *memory);

void memory_init(Memory *memory) {

    current_region = 0;
    current_offset = 0;

    parse_memory_map(memory);

}

static void parse_memory_map(Memory *memory) {

    region_count = 0;
    u64 region_base = 0;
    u64 usable_pages = 0;

    serial_print("   Memory:\n\n");

    while (region_base < memory->map_size && region_count < MAX_REGIONS) {

        MemoryMapDescriptor *descriptor = (MemoryMapDescriptor *)((u8 *)memory->map + region_base);

        if (descriptor->type == EFI_CONVENTIONAL_MEMORY) {

            memory_regions[region_count].base = descriptor->physical_start;
            memory_regions[region_count].size = descriptor->number_of_pages * PAGE_SIZE;

            region_count++;

            serial_print_int("Memory region ", region_count, " ");
            serial_print_hex("@ ", descriptor->physical_start, " - ");
            serial_print_int("", descriptor->number_of_pages, " pages, ");

            u64 kib = descriptor->number_of_pages * 4;

            if (kib >= 1024)
                serial_print_int("", kib / 1024, " MiB (Large)\n");
            else
                serial_print_int("", kib, " KiB\n");

            usable_pages += descriptor->number_of_pages;

        }

        region_base += memory->map_descriptor_size;

    }

    serial_print_int("\nTotal usable memory: ", usable_pages, " pages, ");
    serial_print_int("", usable_pages / 256, " MiB\n\n");

}

void* physical_allocate_page(void) {

    if (current_region >= region_count) {
        return NULL;
    }
    
    if (current_offset + PAGE_SIZE > memory_regions[current_region].size) {
        current_offset = 0;
        current_region++;
    }

    if (current_region >= region_count) {
        return NULL;
    }

    u64 address = memory_regions[current_region].base + current_offset;

    current_offset += PAGE_SIZE;

    return (void*)address;

}
