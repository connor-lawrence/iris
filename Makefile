.PHONY: all clean run run-c

# Directories
BUILD_DIR := build

# Sources:
ALL_DIRS := $(patsubst %/,%,$(wildcard */))

KERNEL_SRC_DIRS := $(filter-out bootloader build include,$(ALL_DIRS))
KERNEL_C_SOURCES := $(foreach dir,$(KERNEL_SRC_DIRS),$(shell find $(dir) -name "*.c"))
KERNEL_S_SOURCES := $(foreach dir,$(KERNEL_SRC_DIRS),$(shell find $(dir) -name "*.s"))
KERNEL_C_OBJECTS := $(foreach c_file,$(KERNEL_C_SOURCES),$(patsubst %.c,$(BUILD_DIR)/%.o, $(c_file)))
KERNEL_S_OBJECTS := $(foreach s_file,$(KERNEL_S_SOURCES),$(patsubst %.s,$(BUILD_DIR)/%.o, $(s_file)))
KERNEL_OBJECTS := $(KERNEL_C_OBJECTS) $(KERNEL_S_OBJECTS)
KERNEL_LINKER := arch/x86_64/linker.ld

BOOT_SRC_DIR := bootloader
BOOT_C_SOURCE := $(shell find $(BOOT_SRC_DIR) -name "*.c")
BOOT_CRT := /usr/lib/crt0-efi-x86_64.o

# Flags
KERNEL_C_FLAGS := -std=c2x -ffreestanding -fno-pie -fno-stack-protector -m64 -mno-red-zone -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wmissing-prototypes -I include
KERNEL_S_FLAGS := -std=c2x -m64
BOOT_C_FLAGS := -fno-pie -fpic -fno-stack-protector -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER -I /usr/include/efi -I /usr/include/efi/x86_64 -I include
BOOT_LD_FLAGS := -nostdlib -shared -Bsymbolic -lgnuefi -lefi -L /usr/lib -T /usr/lib/elf_x86_64_efi.lds
BOOT_OBJCOPY_FLAGS := -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64

# Outputs
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
BOOT_EFI := $(BUILD_DIR)/EFI/BOOT/BOOTX64.EFI

all: $(KERNEL_ELF) $(BOOT_EFI)

# Link kernel objects into the kernel ELF
$(KERNEL_ELF): $(KERNEL_OBJECTS)
	ld -T $(KERNEL_LINKER) -o $@ $^

# Compile all kernel C files into object files
$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	gcc $(KERNEL_C_FLAGS) -c $< -o $@

# Compile all kernel Assembly files into object files
$(BUILD_DIR)/%.o: %.s
	mkdir -p $(dir $@)
	gcc $(KERNEL_S_FLAGS) -c $< -o $@

# Compile bootloader C file into an object file
$(BUILD_DIR)/bootloader.o: $(BOOT_C_SOURCE)
	mkdir -p $(dir $@)
	gcc $(BOOT_C_FLAGS) -c $< -o $@

# Link bootloader object into the shared object
$(BUILD_DIR)/bootloader.so: $(BUILD_DIR)/bootloader.o
	ld $< $(BOOT_CRT) $(BOOT_LD_FLAGS) -o $@

# Convert bootloader shared object to EFI
$(BOOT_EFI): $(BUILD_DIR)/bootloader.so
	mkdir -p $(dir $@)
	objcopy $(BOOT_OBJCOPY_FLAGS) $< $@

clean:
	rm -rf $(BUILD_DIR)

# QEMU
QEMU_OVMF_CODE := /usr/share/OVMF/OVMF_CODE_4M.fd
QEMU_VARS := $(BUILD_DIR)/OVMF_VARS.fd
QEMU_FLAGS := -drive if=pflash,format=raw,readonly=on,file=$(QEMU_OVMF_CODE) \
	-drive if=pflash,format=raw,file=$(QEMU_VARS) \
	-drive format=raw,file=fat:rw:$(BUILD_DIR) -m 1G

$(QEMU_VARS):
	mkdir -p $(dir $@)
	cp /usr/share/OVMF/OVMF_VARS_4M.fd $@

run: $(QEMU_VARS)
	qemu-system-x86_64 $(QEMU_FLAGS) -debugcon stdio

run-c: $(QEMU_VARS)
	qemu-system-x86_64 $(QEMU_FLAGS) -display none -debugcon stdio
