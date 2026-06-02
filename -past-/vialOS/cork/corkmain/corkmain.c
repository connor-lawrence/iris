#include "corkmain.h"
#include "types.h"
#include "../drivers/drivers.h"
#include "../../vial/vialmain/vialmain.h"

void kernel_init() {
    kernel_print_string("[      ] Booting CorK...", VGA_LIGHT_GRAY, 2, 1);
    kernel_print_string("CorK", VGA_RED, 4, 1);
    os_init();
    kernel_main_loop();
}

void kernel_main_loop() {
    while (1) {
        os_main_loop();
        vga_render();
    }
}