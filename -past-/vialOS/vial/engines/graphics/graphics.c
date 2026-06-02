#include "graphics.h"
#include "types.h"
#include "apis.h"

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define BUFFER_HEIGHT 2048

static char screen_text_buffer[BUFFER_HEIGHT][SCREEN_WIDTH];
static u16 screen_color_buffer[BUFFER_HEIGHT][SCREEN_WIDTH];

static int start_row = 0;
static int buffer_scrolled_lines = 0;

void print_string(const char* string, u16 color, int x, int y) { //ADD '\n' HANDLING
    while (*string) {
        if (x >= SCREEN_WIDTH) {
            x = 0;
            y++;
        }
        if (y >= (start_row + SCREEN_HEIGHT)) {
            start_row++;
        }
        if ((start_row + SCREEN_HEIGHT) > BUFFER_HEIGHT) {
            scroll_buffer();
        }
        screen_text_buffer[y - buffer_scrolled_lines][x] = *string;
        screen_color_buffer[y - buffer_scrolled_lines][x] = color;
        x++;
        string++;
    }
}

void scroll_buffer() {
    buffer_scrolled_lines++;
    start_row = (BUFFER_HEIGHT - SCREEN_HEIGHT);
    for (int y = 1; y < BUFFER_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            screen_text_buffer[y - 1][x] = screen_text_buffer[y][x];
            screen_color_buffer[y - 1][x] = screen_color_buffer[y][x];
        }
    }
    for (int ix = 0; ix < SCREEN_WIDTH; ix++) {
        screen_text_buffer[BUFFER_HEIGHT - 1][ix] = ' ';
        screen_color_buffer[BUFFER_HEIGHT - 1][ix] = VGA_LIGHT_GRAY;
    }
}

void render() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        int buffer_y = (start_row + y);
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            kernel_print_char(screen_text_buffer[buffer_y][x], screen_color_buffer[buffer_y][x], x, y);
        }
    }
}