#pragma once
#include "types.h"

typedef struct {
    u8 e_ident[16];
    u16 e_type;
    u16 e_machine;
    u16 e_version;
    u16 e_entry;
    u16 e_phoff;
    u16 e_shoff;
    u16 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
} Elf64_Executable_Header;

typedef struct {

    struct {
        u64 address;
        u32 width;
        u32 height;
        u32 pitch;
    } framebuffer;

} BootInfo;