#include "physical.h"
#include "types.h"
#include "boot.h"

#include "serial.h"

#define MAX_REGIONS 128
#define EFI_CONVENTIONAL_MEMORY 7

static MemoryRegion memory_regions[MAX_REGIONS];
static u64 region_count = 0;

static void parse_memory_map(Memory *memory);

void memory_init(Memory *memory) {

    parse_memory_map(memory);

}

static void parse_memory_map(Memory *memory) {

    u64 region_base = 0;
    u64 usable_pages = 0;

    serial_print("   Memory:\n\n");

    while (region_base < memory->map_size && region_count < MAX_REGIONS) {

        MemoryMapDescriptor *descriptor = (MemoryMapDescriptor *)((u8 *)memory->map + region_base);

        if (descriptor->type == EFI_CONVENTIONAL_MEMORY) {

            memory_regions[region_count].base = descriptor->physical_start;
            memory_regions[region_count].size = descriptor->number_of_pages * 4096;

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
    serial_print_int("", usable_pages / 256, " MiB\n");

}
