#include <efi.h>
#include <efilib.h>

#include "types.h"
#include "cub.h"

// CUB v2.0

static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **kernel_buffer, UINTN *kernel_size);
//static void start_kernel(void *kernel, BootInfo *boot_info);

#define KERNEL_FILE L"kernel.elf"

    static void debug(const char *s){while(*s)__asm__ volatile("outb %0,$0xE9"::"a"(*s++));}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
    
    debug("\n[CUB v2] Initializing library...\n");

    EFI_STATUS status;

    // Initialize library
    InitializeLib(image_handle, system_table);
    Print(L"[CUB v2] Booting...\n");
        debug("[CUB v2] Booting...\n");
 
    void *kernel;
    UINTN size;

    status = load_kernel(image_handle, &kernel, &size);

    if (EFI_ERROR(status)) {
        Print(L"[CUB v2] Kernel loading failed...\n");
            debug("[CUB v2] Kernel loading failed...\n");
        while (1);
    }





    //TEMPORARY
    Print(L"[CUB v2] Loaded kernel size: %lu bytes...\n", size);
        debug("[CUB v2] Found loaded kernel size...\n");

    UINT8 *bytes = (UINT8 *)kernel;

    if (bytes[0] != 0x7F ||
        bytes[1] != 'E' ||
        bytes[2] != 'L' ||
        bytes[3] != 'F') {
        Print(L"[CUB v2] Not a valid ELF file...\n");
            debug("[CUB v2] Not a valid ELF file...\n");
    } else {
        Print(L"[CUB v2] Valid ELF file detected...\n");
            debug("[CUB v2] Valid ELF file detected...\n");
    }
    //END TEMPORARY





    Print(L"[CUB v2] Halted.\n");
        debug("[CUB v2] Halted.\n\n");

    while (1);
    //return EFI_SUCCESS;
}

static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **kernel_buffer, UINTN *kernel_size) {
    
    EFI_STATUS status;
    EFI_LOADED_IMAGE *loaded_image;
    EFI_FILE_HANDLE root_directory;
    EFI_FILE_HANDLE kernel_file;
    EFI_FILE_INFO *kernel_info;
    UINTN kernel_info_size;

    // Get loaded image info
    status = uefi_call_wrapper(BS->HandleProtocol, 3, image_handle, &gEfiLoadedImageProtocolGuid, (void**)&loaded_image);
    if (EFI_ERROR(status)) return status;
    
    // Open root directory
    root_directory = LibOpenRoot(loaded_image->DeviceHandle);
    if (!root_directory) {return EFI_DEVICE_ERROR;}
    
    // Open kernel file
    status = uefi_call_wrapper(root_directory->Open, 5, root_directory, &kernel_file, KERNEL_FILE, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) return status;
    
    // Allocate RAM for kernel info
    kernel_info_size = sizeof(EFI_FILE_INFO) + 200;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, kernel_info_size, (void**)&kernel_info);
    if (EFI_ERROR(status)) return status;
    
    // Get kernel info
    status = uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &gEfiFileInfoGuid, &kernel_info_size, kernel_info);
    if (EFI_ERROR(status)) return status;
    
    // Allocate RAM for kernel
    *kernel_size = kernel_info->FileSize;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderCode, *kernel_size, (void**)kernel_buffer);
    if (EFI_ERROR(status)) return status;
    
    // Read kernel into allocated RAM
    status = uefi_call_wrapper(kernel_file->Read, 3, kernel_file, kernel_size, *kernel_buffer);
    if (EFI_ERROR(status)) return status;
    
    // Close kernel file and free allocated RAM from info
    uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
    uefi_call_wrapper(BS->FreePool, 1, kernel_info);
    
    return status;
}
/*
static EFI_STATUS setup_framebuffer(BootInfo *boot_info) {

    EFI_STATUS status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

    // Initialize GOP
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiGraphicsOutputProtocolGuid, NULL, (void**)&gop);
    if (EFI_ERROR(status)) return status;

    // Fill boot info with framebuffer info
    boot_info->framebuffer.address = (u64)gop->Mode->FrameBufferBase;
    boot_info->framebuffer.width = gop->Mode->Info->HorizontalResolution;
    boot_info->framebuffer.height = gop->Mode->Info->VerticalResolution;
    boot_info->framebuffer.pitch = gop->Mode->Info->PixelsPerScanLine;
    
    return EFI_SUCCESS;
}

static void start_kernel(void *kernel, BootInfo *boot_info) {
    
    // Jump to kernel
    typedef void (*kernel_entry_t)(BootInfo*);
    kernel_entry_t kernel_entry = (kernel_entry_t)kernel;
    Print(L"[CUB v2] Jumping to kernel... \n");
    kernel_entry(boot_info);
}*/