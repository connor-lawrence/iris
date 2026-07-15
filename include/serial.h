#pragma once
#include "types.h"

void serial_putc(char c);
void serial_print(const char *str);
void serial_print_int(const char *before, const u64 x, const char *after);
void serial_print_hex(const char *before, const u64 x, const char *after);
