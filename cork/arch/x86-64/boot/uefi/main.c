#include <efi.h>
#include <efilib.h>

#include "types.h"

// CUB v1.0

#define KERNEL_FILE L"kernel.bin"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *uefi_services) {

    InitializeLib(image_handle, uefi_services); // Initializes library
    Print(L"[CUB] CUB initialized, library initalized...\n");

    EFI_STATUS result;

    EFI_LOADED_IMAGE *efi_info = NULL;
    EFI_FILE_HANDLE root_directory;
    EFI_FILE_HANDLE kernel_file;
    UINTN kernel_info_buffer_size;
    EFI_FILE_INFO *kernel_info = NULL;
    void *kernel_buffer = NULL;
    UINTN kernel_size;

    // Get Boot Info
    result = uefi_call_wrapper(BS->HandleProtocol, 3, image_handle, &gEfiLoadedImageProtocolGuid, (void**)&efi_info);
    if (EFI_ERROR(result)) {Print(L"[CUB] ! When getting boot info: %r\n", result);} else {Print(L"[CUB] Boot info pulled...\n");}

    // Open Root Directory
    root_directory = LibOpenRoot(efi_info->DeviceHandle);
    Print(L"[CUB] Root directory opened...\n");
    if (!root_directory) {
        Print(L"[CUB] Failed to open root directory\n");
        return EFI_DEVICE_ERROR;
    }

    // Open Kernel File
    result = uefi_call_wrapper(root_directory->Open, 5, root_directory, &kernel_file, KERNEL_FILE, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(result)) {Print(L"[CUB] ! When opening kernel file: %r\n", result);} else {Print(L"[CUB] Kernel file opened in read-only...\n");}

    // Allocate RAM for Kernel Info
    kernel_info_buffer_size = sizeof(EFI_FILE_INFO) + 200;
    result = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, kernel_info_buffer_size, (void**)&kernel_info);
    if (EFI_ERROR(result)) {Print(L"[CUB] ! When allocating RAM for kernel info: %r\n", result);} else {Print(L"[CUB] RAM allocated for kernel info...\n");}

    // Get Kernel Info
    result = uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &gEfiFileInfoGuid, &kernel_info_buffer_size, kernel_info);
    if (EFI_ERROR(result)) {Print(L"[CUB] ! When getting kernel info: %r\n", result);} else {Print(L"[CUB] Got kernel info...\n");}
    kernel_size = kernel_info->FileSize;

    // Allocate RAM for Kernel
    result = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderCode, kernel_size, &kernel_buffer);
    if (EFI_ERROR(result)) {Print(L"[CUB] ! When allocating RAM for kernel: %r\n", result);} else {Print(L"[CUB] RAM allocated for kernel...\n");}

    // Copy Kernel To RAM
    result = uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &kernel_size, kernel_buffer);
    if (EFI_ERROR(result)) {Print(L"[CUB] ! When copying kernel to RAM: %r\n", result);} else {Print(L"[CUB] Kernel copied to RAM...\n");}

    // Close Kernel File
    result = uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
    if (EFI_ERROR(result)) {Print(L"[CUB] ! When closing kernel file: %r\n", result);} else {Print(L"[CUB] Kernel file closed...\n");}

    // Free Allocated RAM
    result = uefi_call_wrapper(BS->FreePool, 1, kernel_info);
    if (EFI_ERROR(result)) {Print(L"[CUB] ! When freeing allocated RAM for kernel info: %r\n", result);} else {Print(L"[CUB] Allocated RAM freed for kernel info...\n");}

    // Passing Screen Info On To Kernel
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    result = uefi_call_wrapper(uefi_services->BootServices->LocateProtocol, 3, &gEfiGraphicsOutputProtocolGuid, NULL, (void**)&gop);
    if (EFI_ERROR(result) || !gop || !gop->Mode) {Print(L"[CUB] ! When passing screen info to kernel: %r\n", result);} else {Print(L"[CUB] Passed screen info on to kernel...\n");}
    static BootInfo boot_info;
    boot_info.framebuffer = (u64)gop->Mode->FrameBufferBase;
    boot_info.width = gop->Mode->Info->HorizontalResolution;
    boot_info.height = gop->Mode->Info->VerticalResolution;
    boot_info.pixels_per_scanline = gop->Mode->Info->PixelsPerScanLine;
    
    // Jump To Kernel
    typedef void (*kernel_entry_t)(BootInfo*);
    kernel_entry_t kernel_entry = (kernel_entry_t)kernel_buffer;
    Print(L"[CUB] Jumping to kernel... \n");
    kernel_entry(&boot_info);

    return EFI_SUCCESS;
}