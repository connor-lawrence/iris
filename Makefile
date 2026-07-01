.PHONY: all clean run

# Directory placeholders
KERNEL_DIR := cork
BUILD_DIR := build
BOOT_DIR := $(BUILD_DIR)/EFI/BOOT

# Outputs
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
BOOT_EFI := $(BOOT_DIR)/BOOTX64.EFI

# Sources
KERNEL_FILES := $(shell find $(KERNEL_DIR) -name "*.c" ! -path "$(KERNEL_DIR)/arch/*")
KERNEL_OBJ := $(patsubst $(KERNEL_DIR)/%.c,$(BUILD_DIR)/%.o,$(KERNEL_FILES))
BOOTLOADER := $(KERNEL_DIR)/arch/x86-64/boot/uefi/cub.c

# Flags
KERNEL_GCC_FLAGS := -c -ffreestanding -fno-stack-protector -mno-red-zone -m64 \
	-I $(KERNEL_DIR)/include
BOOTLOADER_GCC_FLAGS := -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER -I /usr/include/efi -I /usr/include/efi/x86_64 \
	-I $(KERNEL_DIR)/include
BOOTLOADER_LD_FLAGS := -nostdlib -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib -lgnuefi -lefi
BOOTLOADER_OBJCOPY_FLAGS := -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64

all: $(BOOT_EFI) $(KERNEL_ELF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BOOT_DIR):
	mkdir -p $(BOOT_DIR)

# Compile kernel files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	gcc $(KERNEL_GCC_FLAGS) $< -o $@

# Link kernel files
$(KERNEL_ELF): $(KERNEL_OBJ)
	ld -T linker.ld -o $@ $^

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

run: clean all
	rm -f $(BUILD_DIR)/OVMF_VARS.fd
	cp /usr/share/OVMF/OVMF_VARS_4M.fd $(BUILD_DIR)/OVMF_VARS.fd
	qemu-system-x86_64 $(QEMU_FLAGS) -debugcon stdio

run-c: clean all
	rm -f $(BUILD_DIR)/OVMF_VARS.fd
	cp /usr/share/OVMF/OVMF_VARS_4M.fd $(BUILD_DIR)/OVMF_VARS.fd
	qemu-system-x86_64 $(QEMU_FLAGS) -nographic