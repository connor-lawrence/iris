#include "keyboard.h"
#include "types.h"

/*
static u8 last_key = 0;

void keyboard_poll() {
    u8 scancode = inb(0x60);
    if (scancode == last_key) return;
    last_key = scancode;
    if (scancode & 0x80) {
        last_key = 0;
        return;
    }
    char key = scancode_to_ascii[scancode];
    if (key) {
        keyboard_handler(key);
    }
}
*/