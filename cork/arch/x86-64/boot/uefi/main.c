#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    Print(L"[CorK UEFI Bootloader] efilib library initalized...\n");

    EFI_FILE_HANDLE root;
    EFI_FILE_HANDLE file;
    EFI_STATUS status;
    Print(L"[CUB] Variables initialized...\n");

    EFI_LOADED_IMAGE *loaded_image = NULL;
    Print(L"[CUB] loaded_image pointer initialized and set to NULL...\n");

    status = uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&loaded_image);
    Print(L"[CUB] loaded_image set...\n");

    root = LibOpenRoot(loaded_image->DeviceHandle);
    Print(L"[CUB] Root folder opened...\n");

    status = uefi_call_wrapper(root->Open, 5, root, &file, L"kernel.bin", EFI_FILE_MODE_READ, 0);
    Print(L"[CUB] kernel.bin opened in read-only...\n");

    EFI_FILE_INFO *info;
    UINTN info_size = sizeof(EFI_FILE_INFO) + 200;
    Print(L"[CUB] info_size initialized with info pointer and buffer (200)...\n");

    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, info_size, (void**)&info);
    if (EFI_ERROR(status)) return status;
    Print(L"[CUB] Memory allocated...\n");

    status = uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &info_size, info);
    if (EFI_ERROR(status)) return status;
    Print(L"[CUB] Got file info...\n");

    UINTN size = info->FileSize;
    Print(L"[CUB] Stored size of kernel.bin...\n");

    void *kernel_buffer = NULL;
    Print(L"[CUB] Initializes pointer for kernel buffer...\n");

    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderCode, size, &kernel_buffer);
    if (EFI_ERROR(status)) return status;
    Print(L"[CUB] Kernel buffer allocated...\n");

    status = uefi_call_wrapper(file->Read, 3, file, &size, kernel_buffer);
    if (EFI_ERROR(status)) return status;
    Print(L"[CUB] File copied from disk to RAM...\n");

    status = uefi_call_wrapper(file->Close, 1, file);
    if (EFI_ERROR(status)) return status;
    Print(L"[CUB] File closed...\n");

    status = uefi_call_wrapper(BS->FreePool, 1, info);
    if (EFI_ERROR(status)) return status;
    Print(L"[CUB] Allocated RAM freed...\n");

    typedef void (*kernel_entry_t)(void);
    kernel_entry_t kernel_entry = (kernel_entry_t)kernel_buffer;
    Print(L"[CUB] RAM treated as a function...\n");

    Print(L"[CUB] Jumping to kernel_entry()\n");
    kernel_entry();

    return EFI_SUCCESS;
}
