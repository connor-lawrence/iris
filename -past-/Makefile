UEFI_INC=/usr/include/efi
UEFI_INC_X86=/usr/include/efi/x86_64
UEFI_LIB=/usr/lib

all: build/EFI/BOOT/BOOTX64.EFI build/kernel.bin

build:
	mkdir -p build

# -------------------------
# UEFI BOOTLOADER
# -------------------------

build/main.o: cork/arch/x86-64/boot/uefi/main.c | build
	gcc $< -c \
	-fno-stack-protector \
	-fpic \
	-fshort-wchar \
	-mno-red-zone \
	-DEFI_FUNCTION_WRAPPER \
	-I $(UEFI_INC) \
	-I $(UEFI_INC_X86) \
	-o $@

build/main.so: build/main.o
	ld $< \
	$(UEFI_LIB)/crt0-efi-x86_64.o \
	-nostdlib \
	-T $(UEFI_LIB)/elf_x86_64_efi.lds \
	-shared \
	-Bsymbolic \
	-L $(UEFI_LIB) \
	-lgnuefi -lefi \
	-o $@

build/EFI/BOOT/BOOTX64.EFI: build/main.so
	mkdir -p build/EFI/BOOT
	objcopy \
	-j .text -j .sdata -j .data -j .rodata \
	-j .dynamic -j .dynsym -j .rel -j .rela -j .reloc \
	--target=efi-app-x86_64 \
	$< \
	$@

build/kernel.o: cork/core/crap.c | build
	gcc $< -c \
		-ffreestanding \
		-fno-stack-protector \
		-fpic \
		-mno-red-zone \
		-o $@

# -------------------------
# KERNEL
# -------------------------
build/kernel.bin: cork/core/crap.c | build
	# 1. Compile into standard flat object code
	gcc $< -c \
		-ffreestanding \
		-fno-stack-protector \
		-mno-red-zone \
		-m64 \
		-o build/kernel.o
	# 2. Extract ONLY the raw machine code bytes from the text section
	objcopy -O binary -j .text build/kernel.o $@



# -------------------------
# CLEAN
# -------------------------

clean:
	rm -rf build
	mkdir -p build
	cp /usr/share/OVMF/OVMF_VARS_4M.fd build/OVMF_VARS.fd