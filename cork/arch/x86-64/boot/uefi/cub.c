#include <efi.h>
#include <efilib.h>

#include "types.h"
#include "cub.h"

// CUB v2.0
#define KERNEL_FILE L"kernel.elf"

// Function declarations
static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **kernel, u64 *kernel_size);

EFI_STATUS EFIAPI efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {

    EFI_STATUS status;
    
    // Initialize library
    InitializeLib(image_handle, system_table);
    Print(L"\n[CUB v2] Initialized...\n");

    // Load kernel into memory
    Print(L"[CUB v2] Loading kernel into memory...\n");
    void *kernel;
    u64 kernel_size;
    status = load_kernel(image_handle, &kernel, &kernel_size);

    if (EFI_ERROR(status)) {
        Print(L"[CUB v2] Kernel loading failed...\n");
        while (1);
    }

    Print(L"[CUB v2] Loaded kernel size: %lu bytes.\n", kernel_size);

    ////////////////////////////////////////////////////// TEMPORARY

    // Get and print ELF executable header
    Elf64_Executable_Header *e_header = (Elf64_Executable_Header *)kernel;
    Print(L"[CUB v2] ELF magic number: 0x%x %c %c %c, ", e_header->e_ident[0], e_header->e_ident[1], e_header->e_ident[2], e_header->e_ident[3]);

    // Check magic number
    if (e_header->e_ident[0] == 0x7F && e_header->e_ident[1] == 'E' && e_header->e_ident[2] == 'L' && e_header->e_ident[3] == 'F') {
        Print(L"Valid ELF file detected.\n");
    } else { Print(L"Not a valid ELF file.\n"); }

    ////////////////////////////////////////////////////// END OF TEMPORARY

    // Halt
    Print(L"[CUB v2] Finished, halting.\n");
    while (1);
    //return EFI_SUCCESS;
}

static EFI_STATUS load_kernel(EFI_HANDLE image_handle, void **kernel, u64 *kernel_file_size) {

    EFI_STATUS status;
    *kernel = NULL;

    EFI_LOADED_IMAGE *loaded_image = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *filesystem = NULL;
    EFI_FILE_PROTOCOL *root_directory = NULL;
    EFI_FILE_PROTOCOL *kernel_file = NULL;
    u64 kernel_file_info_size = 0;
    EFI_FILE_INFO *kernel_file_info = NULL;

    // Get loaded image (and device handle)
    status = uefi_call_wrapper(BS->HandleProtocol, 3, image_handle, &gEfiLoadedImageProtocolGuid, (void **)&loaded_image);
    if (EFI_ERROR(status)) goto ocp;

    // Get filesystem protocol
    status = uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **)&filesystem);
    if (EFI_ERROR(status)) goto ocp;

    // Open root_directory
    status = uefi_call_wrapper(filesystem->OpenVolume, 2, filesystem, &root_directory);
    if (EFI_ERROR(status)) goto ocp;

    // Open kernel file
    status = uefi_call_wrapper(root_directory->Open, 5, root_directory, &kernel_file, KERNEL_FILE, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) goto ocp;

    // Get size of kernel file info by failing successfully
    status = uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &gEfiFileInfoGuid, &kernel_file_info_size, NULL);
    if (EFI_ERROR(status) && status != EFI_BUFFER_TOO_SMALL) goto ocp;

    // Allocate memory for kernel file info
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, kernel_file_info_size, (void **)&kernel_file_info);
    if (EFI_ERROR(status)) goto ocp;
    
    // Get kernel file info
    status = uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &gEfiFileInfoGuid, &kernel_file_info_size, kernel_file_info);
    if (EFI_ERROR(status)) goto ocp;

    // Get (and return) kernel file size from kernel file info
    *kernel_file_size = kernel_file_info->FileSize;

    // Allocate memory for kernel file
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, *kernel_file_size, kernel);
    if (EFI_ERROR(status)) goto ocp;

    // Read kernel file into memory
    status = uefi_call_wrapper(kernel_file->Read, 3, kernel_file, kernel_file_size, *kernel);
    if (EFI_ERROR(status)) goto ocp;

    // Close kernel file and free allocated RAM from info
    uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
    uefi_call_wrapper(root_directory->Close, 1, root_directory);
    uefi_call_wrapper(BS->FreePool, 1, kernel_file_info);

    return EFI_SUCCESS;

    // OCP
    ocp:
    if (*kernel) uefi_call_wrapper(BS->FreePool, 1, *kernel);
    if (kernel_file_info) uefi_call_wrapper(BS->FreePool, 1, kernel_file_info);
    if (kernel_file) uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
    if (root_directory) uefi_call_wrapper(root_directory->Close, 1, root_directory);
    return status;
}