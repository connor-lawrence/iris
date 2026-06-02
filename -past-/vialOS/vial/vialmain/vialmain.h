#ifndef VIALMAIN_H
#define VIALMAIN_H

#include "types.h"

/*
typedef enum {
    OS_MODE_INIT,
    OS_MODE_VSH,
    OS_MODE_TEST
} OSMode;

extern OSMode os_mode;
*/

void os_init();
void os_main_loop();
void print_banner();

#endif