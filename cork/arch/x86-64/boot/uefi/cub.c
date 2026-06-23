#include <efi.h>
#include <efilib.h>

#include "types.h"
#include "cub.h"

// CUB v2.0

static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **file_buffer, UINTN *file_size);
//static void start_kernel(void *kernel, BootInfo *boot_info);

#define KERNEL_FILE L"kernel.elf"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *uefi_services) {

    // Initialize library
    InitializeLib(image_handle, uefi_services);
    Print(L"[CUB v2] Booting...\n");
 
    void *kernel;
    UINTN size;

    load_kernel(image_handle, &kernel, &size);

    Print(L"[CUB v2] Loaded kernel size: %d\n", size);
    
    UINT8 *bytes = (UINT8 *)kernel;

    if (bytes[0] != 0x7F ||
        bytes[1] != 'E' ||
        bytes[2] != 'L' ||
        bytes[3] != 'F') {

        Print(L"[CUB v2] Not a valid ELF file\n");
        while (1);
    }

    Print(L"[CUB v2] ELF OK!\n");

    Print(L"[CUB v2] Halted.\n");

    while (1);
    return EFI_SUCCESS;
}

static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **file_buffer, UINTN *file_size) {
    
    EFI_STATUS outcome;
    EFI_LOADED_IMAGE *loaded_image;
    EFI_FILE_HANDLE root_directory;
    EFI_FILE_HANDLE file;
    EFI_FILE_INFO *file_info;
    UINTN info_size;
    //UINTN file_size;

    // Get boot info
    outcome = uefi_call_wrapper(BS->HandleProtocol, 3, image_handle, &gEfiLoadedImageProtocolGuid, (void**)&loaded_image);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Open root directory
    root_directory = LibOpenRoot(loaded_image->DeviceHandle);
    if (!root_directory) {return EFI_DEVICE_ERROR;}
    
    // Open kernel file
    outcome = uefi_call_wrapper(root_directory->Open, 5, root_directory, &file, KERNEL_FILE, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Allocate RAM for kernel info
    info_size = sizeof(EFI_FILE_INFO) + 200;
    outcome = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, info_size, (void**)&file_info);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Get kernel info
    outcome = uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &info_size, file_info);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Allocate RAM for kernel
    *file_size = file_info->FileSize;
    outcome = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderCode, *file_size, (void**)file_buffer);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Copy kernel to RAM
    outcome = uefi_call_wrapper(file->Read, 3, file, file_size, *file_buffer);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Close kernel file and free allocated RAM from info
    uefi_call_wrapper(file->Close, 1, file);
    uefi_call_wrapper(BS->FreePool, 1, file_info);
    
    return outcome;
}
/*
static EFI_STATUS setup_framebuffer(BootInfo *boot_info) {

    EFI_STATUS outcome;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

    // Initialize GOP
    outcome = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiGraphicsOutputProtocolGuid, NULL, (void**)&gop);
    if (EFI_ERROR(outcome)) return outcome;

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