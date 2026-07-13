.PHONY: all clean run run-c

# Directory placeholders
KERNEL_DIR := iris
BUILD_DIR := build
BOOT_DIR := $(BUILD_DIR)/EFI/BOOT

# Outputs
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
BOOT_EFI := $(BOOT_DIR)/BOOTX64.EFI

# Sources
KERNEL_C_FILES := $(shell find $(KERNEL_DIR) -name "*.c" ! -path "$(KERNEL_DIR)/arch/*/boot/uefi/*")
KERNEL_S_FILES := $(shell find $(KERNEL_DIR) -name "*.s" ! -path "$(KERNEL_DIR)/arch/*/boot/uefi/*")

KERNEL_C_OBJ := $(patsubst $(KERNEL_DIR)/%.c,$(BUILD_DIR)/%.o,$(KERNEL_C_FILES))
KERNEL_S_OBJ := $(patsubst $(KERNEL_DIR)/%.s,$(BUILD_DIR)/%.o,$(KERNEL_S_FILES))

KERNEL_OBJ := $(KERNEL_C_OBJ) $(KERNEL_S_OBJ)

BOOTLOADER := $(KERNEL_DIR)/arch/x86_64/boot/uefi/cub.c

# Flags
KERNEL_GCC_FLAGS := -c -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -m64 \
	-I $(KERNEL_DIR)/include
BOOTLOADER_GCC_FLAGS := -fno-pie -fpic -fno-stack-protector -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER -Wall -Wextra -I /usr/include/efi -I /usr/include/efi/x86_64 \
	-I $(KERNEL_DIR)/include
BOOTLOADER_LD_FLAGS := -nostdlib -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib -lgnuefi -lefi
BOOTLOADER_OBJCOPY_FLAGS := -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64

all: $(KERNEL_ELF) $(BOOT_EFI) $(BUILD_DIR)/OVMF_VARS.fd

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BOOT_DIR):
	mkdir -p $(BOOT_DIR)

# Compile kernel C files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	gcc $(KERNEL_GCC_FLAGS) $< -o $@

# Assemble kernel assembly files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.s | $(BUILD_DIR)
	mkdir -p $(dir $@)
	gcc -c -m64 $< -o $@

# Link kernel files
$(KERNEL_ELF): $(KERNEL_OBJ)
	ld -T iris/arch/x86_64/linker.ld -o $@ $^

# Compile bootloader into an object file
$(BUILD_DIR)/bootloader.o: $(BOOTLOADER) | $(BUILD_DIR)
	gcc -c $(BOOTLOADER_GCC_FLAGS) $< -o $@

# Link bootloader into an ELF
$(BUILD_DIR)/bootloader.so: $(BUILD_DIR)/bootloader.o
	ld $< /usr/lib/crt0-efi-x86_64.o $(BOOTLOADER_LD_FLAGS) -o $@

# Convert bootloader into a firmware bootable format
$(BOOT_EFI): $(BUILD_DIR)/bootloader.so | $(BOOT_DIR)
	objcopy $(BOOTLOADER_OBJCOPY_FLAGS) $< $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Run QEMU VM
QEMU_FLAGS := -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
	-drive if=pflash,format=raw,file=$(BUILD_DIR)/OVMF_VARS.fd \
	-drive format=raw,file=fat:rw:$(BUILD_DIR) -m 512

$(BUILD_DIR)/OVMF_VARS.fd:
	rm -f $(BUILD_DIR)/OVMF_VARS.fd
	cp /usr/share/OVMF/OVMF_VARS_4M.fd $@

run:
	sudo chown -R $(USER):$(USER) $(BUILD_DIR)
	qemu-system-x86_64 $(QEMU_FLAGS) -debugcon stdio

run-c:
	qemu-system-x86_64 $(QEMU_FLAGS) -nographic
