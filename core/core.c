#include "core.h"
#include "types.h"
#include "boot.h"
#include "framebuffer.h"
#include "physical.h"

#include "serial.h"

static void core_init(BootInfo *boot_info);

void kernel_main(BootInfo *boot_info) {

    // Print boot info
    serial_print("\nHello from Iris!\n\n");
    serial_print("   Boot info from Cub v2.1:\n\n");
    serial_print_hex("Kernel base:         ", (u64)boot_info->memory.kernel_base, "\n");
    serial_print_int("Kernel size:         ", boot_info->memory.kernel_size / 1024, " KiB ");
    serial_print_int("(", boot_info->memory.kernel_size, " bytes)\n\n");
    serial_print_hex("Memory map base:     ", (u64)boot_info->memory.map, "\n");
    serial_print_int("Memory map size:     ", boot_info->memory.map_size, " bytes\n");
    serial_print_int("Memory map dsc size: ", boot_info->memory.map_descriptor_size, " bytes\n");
    serial_print_int("Memory map dsc vers: ", boot_info->memory.map_descriptor_version, "\n");
    serial_print_int("Memory map key:      ", boot_info->memory.map_key, "\n\n");
    serial_print_hex("Framebuffer base:    ", (u64)boot_info->framebuffer.base, "\n");
    serial_print_int("Framebuffer width:   ", boot_info->framebuffer.width, " px\n");
    serial_print_int("Framebuffer height:  ", boot_info->framebuffer.height, " px\n");
    serial_print_int("Framebuffer scan-ln: ", boot_info->framebuffer.pixels_per_scanline, " px\n");
    serial_print_int("Framebuffer px frmt: ", boot_info->framebuffer.pixel_format, "\n\n");

    core_init(boot_info);

    // Allocate a page
    serial_print_hex("Allocated page @ ", (u64)physical_allocate_page(), "\n");

    // Halt
    while (1) {
        __asm__ volatile ("hlt");
    }
}

static void core_init(BootInfo *boot_info) {

    framebuffer_init(boot_info->framebuffer);
    memory_init(&boot_info->memory);

}
