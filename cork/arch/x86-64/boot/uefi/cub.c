#include <efi.h>
#include <efilib.h>

#include "types.h"
#include "cub.h"

// CUB v1.2

static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **kernel_buffer);
static EFI_STATUS setup_framebuffer(BootInfo *boot_info);
static void start_kernel(void *kernel, BootInfo *boot_info);

#define KERNEL_FILE L"kernel.bin"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *uefi_services) {

    // Initialize library
    InitializeLib(image_handle, uefi_services);

    Print(L"[CUB] Initialized...\n");

    BootInfo boot_info = {0};
    void* kernel = NULL;

    EFI_STATUS outcome;

    outcome = load_kernel(image_handle, &kernel);
    if (EFI_ERROR(outcome)) {
        Print(L"[CUB] Kernel load failed: %r\n", outcome);
        return outcome;
    }

    outcome = setup_framebuffer(&boot_info);
    if (EFI_ERROR(outcome)) {
        Print(L"[CUB] Framebuffer setup failed: %r\n", outcome);
        return outcome;
    }

    start_kernel(kernel, &boot_info);

    return EFI_SUCCESS;
}

static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **kernel_buffer) {
    
    EFI_STATUS outcome;
    EFI_LOADED_IMAGE *efi_info = NULL;
    EFI_FILE_HANDLE kernel_file = NULL;
    UINTN kernel_info_buffer_size;
    EFI_FILE_INFO *kernel_info = NULL;
    UINTN kernel_size;
    
    // Get boot info
    outcome = uefi_call_wrapper(BS->HandleProtocol, 3, image_handle, &gEfiLoadedImageProtocolGuid, (void**)&efi_info);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Open root directory
    EFI_FILE_HANDLE root_directory = LibOpenRoot(efi_info->DeviceHandle);
    if (!root_directory) {return EFI_DEVICE_ERROR;}
    
    // Open kernel file
    outcome = uefi_call_wrapper(root_directory->Open, 5, root_directory, &kernel_file, KERNEL_FILE, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Allocate RAM for kernel info
    kernel_info_buffer_size = sizeof(EFI_FILE_INFO) + 200;
    outcome = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, kernel_info_buffer_size, (void**)&kernel_info);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Get kernel info
    outcome = uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &gEfiFileInfoGuid, &kernel_info_buffer_size, kernel_info);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Allocate RAM for kernel
    kernel_size = kernel_info->FileSize;
    outcome = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderCode, kernel_size, (void**)kernel_buffer);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Copy kernel to RAM
    outcome = uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &kernel_size, *kernel_buffer);
    if (EFI_ERROR(outcome)) return outcome;
    
    // Close kernel file and free allocated RAM from info
    uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
    uefi_call_wrapper(BS->FreePool, 1, kernel_info);
    
    return outcome;
}

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
    Print(L"[CUB] Jumping to kernel... \n");
    kernel_entry(boot_info);
}