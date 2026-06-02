#include "vialmain.h"
#include "types.h"
#include "../engines/engines.h"

//OSMode os_mode = OS_MODE_INIT;

void os_init() {
    print_banner();
    //os_mode = OS_MODE_TEST;
}

void os_main_loop() {
    render();
}

void print_banner() {
    print_string("___      ___  ___  __  ___", VGA_LIGHT_BLUE, 2, 1);
    print_string("\\  \\    /  / /  / /  \\ \\  \\", VGA_LIGHT_BLUE, 2, 2);
    print_string("\\  \\  /  / /  / /    \\ \\  \\", VGA_LIGHT_BLUE, 3, 3);
    print_string("\\  \\/  / /  / /  /\\  \\ \\  \\", VGA_LIGHT_BLUE, 4, 4);
    print_string("\\    / /  / /  /__\\  \\ \\  \\__", VGA_LIGHT_BLUE, 5, 5);
    print_string("\\__/ /__/ /__/    \\__\\ \\_____\\", VGA_LIGHT_BLUE, 6, 6);

    print_string("VialOS - v0.2.1", VGA_LIGHT_BLUE, 32, 2);
    print_string("CorK - v0.3.4", VGA_RED, 34, 3);
    print_string("vsh - v0.0.1", VGA_GREEN, 35, 4);
    print_string("* - v0.4.1", VGA_LIGHT_GRAY, 37, 5);
    print_string("- (i386)", VGA_RED, 39, 6);
}

//  ___      ___  ___  __  ___
//  \  \    /  / /  / /  \ \  \   VialOS - v0.2.1
//   \  \  /  / /  / /    \ \  \    CorK - v0.3.4
//    \  \/  / /  / /  /\  \ \  \    vsh - v0.0.1
//     \    / /  / /  /__\  \ \  \__   * - v0.4.1
//      \__/ /__/ /__/    \__\ \_____\   - (i386)

//  ___      ___  ___  __  ___
//  \\  \\    /  / /  / /  \\ \\  \\   VialOS - v0.2.1
//   \\  \\  /  / /  / /    \\ \\  \\    CorK - v0.3.4
//    \\  \\/  / /  / /  /\\  \\ \\  \\    vsh - v0.0.1
//     \\    / /  / /  /__\\  \\ \\  \\__   * - v0.4.1
//      \\__/ /__/ /__/    \\__\\ \\_____\\   - (i386)


// (Push CorK to 0.4.1 if needed, last was 0.3.3)
// (Note - i386, x86-32 aka x86 32-bit protected)