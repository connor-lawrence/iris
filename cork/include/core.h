#ifndef CORE_H
#define CORE_H

#include "types.h"
#include "cub.h"

extern BootInfo* boot_info;

void kernel_main(BootInfo* boot_info);

#endif