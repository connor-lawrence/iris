KERNEL_DIR := cork
BOOTLOADER := $(KERNEL_DIR)/arch/x86-64/boot/uefi/cub.c
BOOTLOADER_GCC_FLAGS := -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -DEFI_FUNCTION_WRAPPER -I /usr/include/efi -I /usr/include/efi/x86_64 
BOOTLOADER_GCC_FLAGS += -I $(KERNEL_DIR)/include
BOOTLOADER_LD_FLAGS := -nostdlib -T /usr/lib/elf_x86_64_efi.lds -shared -Bsymbolic -L /usr/lib -lgnuefi -lefi
BOOTLOADER_OBJCOPY_FLAGS := -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-x86_64

KERNEL_FILES := $(shell find $(KERNEL_DIR) -name "*.c" ! -path "$(KERNEL_DIR)/arch/*")
KERNEL_OBJ := $(patsubst $(KERNEL_DIR)/%.c,build/%.o,$(KERNEL_FILES))
KERNEL_GCC_FLAGS := -c -ffreestanding -fno-stack-protector -mno-red-zone -m64 
KERNEL_GCC_FLAGS += -I $(KERNEL_DIR)/include

QEMU_FLAGS := -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd \
	-drive if=pflash,format=raw,file=build/OVMF_VARS.fd \
	-drive format=raw,file=fat:rw:build -m 512 -debugcon stdio

all: build/EFI/BOOT/BOOTX64.EFI build/kernel.bin

build:
	mkdir -p build

# Build bootloader (BOOTX64.EFI)

build/bootloader.o: $(BOOTLOADER) | build
	gcc -c $(BOOTLOADER_GCC_FLAGS) $< -o $@

build/bootloader.so: build/bootloader.o
	ld $< /usr/lib/crt0-efi-x86_64.o $(BOOTLOADER_LD_FLAGS) -o $@

build/EFI/BOOT/BOOTX64.EFI: build/bootloader.so
	mkdir -p build/EFI/BOOT
	objcopy $(BOOTLOADER_OBJCOPY_FLAGS) $< $@

# Build kernel (kernel.bin)

build/%.o: cork/%.c | build
	mkdir -p $(dir $@)
	gcc $(KERNEL_GCC_FLAGS) $< -o $@

build/kernel.bin: $(KERNEL_OBJ)
	ld -T linker.ld -o build/kernel.elf $^
	objcopy -O binary build/kernel.elf $@

# Clean

clean:
	rm -rf build

# Run QEMU VM

build/OVMF_VARS.fd: /usr/share/OVMF/OVMF_VARS_4M.fd | build
	cp $< $@

run: clean all build/OVMF_VARS.fd
	qemu-system-x86_64 $(QEMU_FLAGS)