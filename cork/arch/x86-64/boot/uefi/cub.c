#include <efi.h>
#include <efilib.h>

#include "types.h"
#include "cub.h"

// CUB v2.0

static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **kernel_buffer, u64 *kernel_size);

#define KERNEL_FILE L"kernel.elf"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {

    EFI_STATUS status;

    // Initialize library
    InitializeLib(image_handle, system_table);
    Print(L"\n[CUB v2] Booting...\n");

    // Load kernel into RAM
    void *kernel;
    u64 kernel_size;
    status = load_kernel(image_handle, &kernel, &kernel_size);

    if (EFI_ERROR(status)) {
        Print(L"[CUB v2] Kernel loading failed...\n");
        while (1);
    }

    Print(L"[CUB v2] Loaded kernel size: %lu bytes...\n", kernel_size);
    
    // Get ELF executable header
    Elf64_Executable_Header *e_header = (Elf64_Executable_Header *)kernel;
    Print(L"[CUB v2] ELF magic number: 0x%x %c %c %c, ", e_header->e_ident[0], e_header->e_ident[1], e_header->e_ident[2], e_header->e_ident[3]);

    // Check magic number
    if (e_header->e_ident[0] == 0x7F && e_header->e_ident[1] == 'E' && e_header->e_ident[2] == 'L' && e_header->e_ident[3] == 'F') {
        Print(L"Valid ELF file detected...\n");
    } else { Print(L"Not a valid ELF file...\n"); }

    Print(L"[CUB v2] Finished, halting.\n");

    // Halt so text can be read
    while (1);
    //return EFI_SUCCESS;
}

static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **kernel, u64 *kernel_size) {
    
    EFI_STATUS status;

    // Fill loaded_image with metadata
    EFI_LOADED_IMAGE *loaded_image;
    status = uefi_call_wrapper(BS->HandleProtocol, 3, image_handle, &gEfiLoadedImageProtocolGuid, (void**)&loaded_image);
    if (EFI_ERROR(status)) return status;

    // Open root directory
    EFI_FILE_HANDLE root_directory;
    root_directory = LibOpenRoot(loaded_image->DeviceHandle);
    if (!root_directory) {return EFI_DEVICE_ERROR;}

    // Open kernel file (gotten from KERNEL_FILE) from root directory
    EFI_FILE_HANDLE kernel_file;
    status = uefi_call_wrapper(root_directory->Open, 5, root_directory, &kernel_file, KERNEL_FILE, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) return status;

    // Find size of kernel info by failing successfully
    u64 kernel_info_size = 0;
    status = uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &gEfiFileInfoGuid, &kernel_info_size, NULL);
    if (EFI_ERROR(status) && status != EFI_BUFFER_TOO_SMALL) return status;

    // Allocate RAM for kernel info
    EFI_FILE_INFO *kernel_info;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, kernel_info_size, (void**)&kernel_info);
    if (EFI_ERROR(status)) return status;

    // Get kernel info
    status = uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &gEfiFileInfoGuid, &kernel_info_size, kernel_info);
    if (EFI_ERROR(status)) return status;

    // Allocate RAM for kernel
    *kernel_size = kernel_info->FileSize;
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, *kernel_size, (void**)kernel);
    if (EFI_ERROR(status)) return status;

    // Read kernel into allocated RAM
    status = uefi_call_wrapper(kernel_file->Read, 3, kernel_file, kernel_size, *kernel);
    if (EFI_ERROR(status)) return status;

    // Close kernel file and free allocated RAM from info
    uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
    uefi_call_wrapper(BS->FreePool, 1, kernel_info);

    return status;
}



/*static EFI_STATUS setup_framebuffer(BootInfo *boot_info) {

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
}*/