#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    Print(L"[CorK] UEFI Bootloader...\n");

    EFI_FILE_HANDLE root;
    EFI_FILE_HANDLE file;
    EFI_STATUS status;

    Print(L"[CorK] Debug 1\n");
    EFI_LOADED_IMAGE *loaded_image = NULL;
    
    status = uefi_call_wrapper(
        BS->HandleProtocol, 3, 
        ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&loaded_image
    );
    if (EFI_ERROR(status) || !loaded_image) {
        Print(L"Loaded Image error: %r\n", status);
        return status;
    }

    Print(L"[CorK] Debug 2\n");
    root = LibOpenRoot(loaded_image->DeviceHandle);
    if (!root) {
        Print(L"Failed to open root volume\n");
        return EFI_DEVICE_ERROR;
    }

    Print(L"[CorK] Debug 3\n");
    status = uefi_call_wrapper(
        root->Open, 5, 
        root, &file, L"kernel.bin", EFI_FILE_MODE_READ, 0
    );
    if (EFI_ERROR(status)) {
        Print(L"[CorK] Error opening kernel.bin: %r\n", status);
        return status;
    }

    Print(L"[CorK] Debug 4\n");
    EFI_FILE_INFO *info;

    Print(L"[CorK] Debug 5\n");
    UINTN info_size = sizeof(EFI_FILE_INFO) + 200;

    Print(L"[CorK] Debug 6\n");
    uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, info_size, (void**)&info);

    Print(L"[CorK] Debug 7\n");
    status = uefi_call_wrapper(
        file->GetInfo, 4, 
        file, &gEfiFileInfoGuid, &info_size, info
    );
    if (EFI_ERROR(status)) {
        Print(L"[CorK] Error getting file info: %r\n", status);
        return status;
    }

    Print(L"[CorK] Debug 8\n");
    UINTN size = info->FileSize;

    Print(L"[CorK] Debug 9\n");
    void *kernel_buffer;

    Print(L"[CorK] Debug 10\n");
    uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderCode, size, &kernel_buffer);

    Print(L"[CorK] Debug 11\n");
    // FIX: Wrapped Read call properly
    status = uefi_call_wrapper(file->Read, 3, file, &size, kernel_buffer);
    if (EFI_ERROR(status)) {
        Print(L"[CorK] Error reading file: %r\n", status);
        return status;
    }

    Print(L"[CorK] Debug 12\n");
    // FIX: Wrapped Close call properly
    uefi_call_wrapper(file->Close, 1, file);
    uefi_call_wrapper(BS->FreePool, 1, info);

    Print(L"[CorK] Debug 13\n");
    
    // FIX: Typedef the function pointer explicitly to prevent ABI/execution bugs
    typedef void (*kernel_entry_t)(void);
    kernel_entry_t kernel_entry = (kernel_entry_t)kernel_buffer;
    
    Print(L"Jumping to kernel...\n");
    kernel_entry();

    return EFI_SUCCESS;
}
