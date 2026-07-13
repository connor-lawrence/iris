#include <efi.h>
#include <efilib.h>

#include "types.h"
#include "cub.h"

// CUB v2.1
#define KERNEL_FILE L"kernel.elf"

// Function declarations
static EFI_STATUS read_file(EFI_HANDLE image_handle, void **file_image, u64 *file_size);
static EFI_STATUS validate_elf(void *file_image, u64 file_size);
static EFI_STATUS load_elf(void *file_image, void (**kernel_entry)(BootInfo *));
static EFI_STATUS get_framebuffer(BootInfo *boot_info);
static EFI_STATUS get_memory_map(BootInfo *boot_info);
static EFI_STATUS exit_boot_services(EFI_HANDLE image_handle, BootInfo *boot_info);

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {

    EFI_STATUS status = EFI_SUCCESS;
    void *kernel_image = NULL;
    u64 kernel_size = 0;
    void (*kernel_entry)(BootInfo *) = NULL;
    BootInfo *boot_info = NULL;

    InitializeLib(image_handle, system_table);

    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(BootInfo), (void **)&boot_info);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    SetMem(boot_info, sizeof(BootInfo), 0);

    status = read_file(image_handle, &kernel_image, &kernel_size);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    status = validate_elf(kernel_image, kernel_size);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    status = load_elf(kernel_image, &kernel_entry);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    uefi_call_wrapper(BS->FreePool, 1, kernel_image);
    kernel_image = NULL;

    status = get_framebuffer(boot_info);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    status = get_memory_map(boot_info);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    status = exit_boot_services(image_handle, boot_info);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    kernel_entry(boot_info);

    return EFI_SUCCESS;

    // Optional cleanup
    oc:

    if (boot_info) {
        uefi_call_wrapper(BS->FreePool, 1, boot_info);
    }

    if (kernel_image) {
        uefi_call_wrapper(BS->FreePool, 1, kernel_image);
    }

    return status;

}

static EFI_STATUS read_file(EFI_HANDLE image_handle, void **file_image, u64 *file_size) {

    EFI_STATUS status;
    *file_image = NULL;
    *file_size = 0;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *filesystem = NULL;
    EFI_FILE_PROTOCOL *root_directory = NULL;
    EFI_FILE_PROTOCOL *file = NULL;
    u64 file_metadata_size = 0;
    EFI_FILE_INFO *file_metadata = NULL;

    // Get loaded image information
    status = uefi_call_wrapper(BS->HandleProtocol, 3, image_handle, &gEfiLoadedImageProtocolGuid, (void **)&loaded_image);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    // Get filesystem protocol
    status = uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void **)&filesystem);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    // Open root directory
    status = uefi_call_wrapper(filesystem->OpenVolume, 2, filesystem, &root_directory);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    // Open file
    status = uefi_call_wrapper(root_directory->Open, 5, root_directory, &file, KERNEL_FILE, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    // Get the size of the file metadata by failing successfully
    status = uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &file_metadata_size, NULL);
    if (status != EFI_BUFFER_TOO_SMALL) {
        goto oc;
    }

    // Allocate memory for file metadata
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, file_metadata_size, (void **)&file_metadata);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    // Get file metadata
    status = uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &file_metadata_size, file_metadata);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    *file_size = file_metadata->FileSize;

    // Allocate memory for file
    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, *file_size, file_image);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    // Read file into memory
    status = uefi_call_wrapper(file->Read, 3, file, file_size, *file_image);
    if (EFI_ERROR(status)) {
        goto oc;
    }

    // Check if the read returned the expected size
    if (*file_size != file_metadata->FileSize) {
        status = EFI_LOAD_ERROR;
        goto oc;
    }

    // Close file, close root directory, and free memory for file metadata
    uefi_call_wrapper(file->Close, 1, file);
    uefi_call_wrapper(root_directory->Close, 1, root_directory);
    uefi_call_wrapper(BS->FreePool, 1, file_metadata);
    file = NULL;
    root_directory = NULL;
    file_metadata = NULL;

    return EFI_SUCCESS;

    // Optional cleanup
    oc:

    if (*file_image) {
        uefi_call_wrapper(BS->FreePool, 1, *file_image);
    }

    if (file_metadata) {
        uefi_call_wrapper(BS->FreePool, 1, file_metadata);
    }

    if (file) {
        uefi_call_wrapper(file->Close, 1, file);
    }

    if (root_directory) {
        uefi_call_wrapper(root_directory->Close, 1, root_directory);
    }

    return status;

}

static EFI_STATUS validate_elf(void *file_image, u64 file_size) {

    // Check if the file is large enough to contain the ELF header
    if (file_size < sizeof(Elf64_Executable_Header)) {
        return EFI_LOAD_ERROR;
    }

    Elf64_Executable_Header *e_header = (Elf64_Executable_Header *)file_image;

    // Check ELF magic number
    if (e_header->e_ident[0] != 0x7F || e_header->e_ident[1] != 'E' || e_header->e_ident[2] != 'L' || e_header->e_ident[3] != 'F') {
        return EFI_LOAD_ERROR;
    }

    // Check if the ELF is 64-bit
    if (e_header->e_ident[4] != 2) {
        return EFI_LOAD_ERROR;
    }

    // Check if the ELF is for x86_64 architecture
    if (e_header->e_machine != 0x3E) {
        return EFI_LOAD_ERROR;
    }

    // Check if the program header entry size matches the ELF header
    if (e_header->e_phentsize != sizeof(Elf64_Program_Header)) {
        return EFI_LOAD_ERROR;
    }

    // Check if the file is large enough to contain the program headers
    if (e_header->e_phoff > file_size || e_header->e_phnum > (file_size - e_header->e_phoff) / e_header->e_phentsize) {
        return EFI_LOAD_ERROR;
    }

    Elf64_Program_Header *p_headers = (Elf64_Program_Header *)((u8 *)file_image + e_header->e_phoff);

    // Check that at least one segment will be loaded (PT_LOAD)
    for (u64 i = 0; i < e_header->e_phnum; i++) {

        if (p_headers[i].p_type == 1) {
            break;
        } else if (i == (u64)e_header->e_phnum - 1) {
            return EFI_LOAD_ERROR;
        }

    }

    // Check each segment:
    for (u64 i = 0; i < e_header->e_phnum; i++) {

        if (p_headers[i].p_type != 1) {
            continue;
        }

        // Check if the segment will fit in the file
        if (p_headers[i].p_offset > file_size || p_headers[i].p_filesz > file_size - p_headers[i].p_offset) {
            return EFI_LOAD_ERROR;
        }

        // Check if BSS in the segment is valid (memory size >= file size)
        if (p_headers[i].p_memsz < p_headers[i].p_filesz) {
            return EFI_LOAD_ERROR;
        }

    }

    return EFI_SUCCESS;

}

static EFI_STATUS load_elf(void *file_image, void (**kernel_entry)(BootInfo *)) {

    EFI_STATUS status;

    Elf64_Executable_Header *e_header = (Elf64_Executable_Header *)file_image;
    Elf64_Program_Header *p_headers = (Elf64_Program_Header *)((u8 *)file_image + e_header->e_phoff);

    for (u64 i = 0; i < e_header->e_phnum; i++) {

        // Skip segments that aren't PT_LOAD
        if (p_headers[i].p_type != 1) {
            continue;
        }

        // Find the number of pages needed for the segment and allocate them
        u64 pages = (p_headers[i].p_memsz + 4095) / 4096;

        EFI_PHYSICAL_ADDRESS segment_address = p_headers[i].p_vaddr;

        status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderCode, pages, &segment_address);
        if (EFI_ERROR(status)) {
            return status;
        }

        u8 *source = (u8 *)file_image + p_headers[i].p_offset;
        u8 *destination = (u8 *)segment_address;

        // Copy segment
        CopyMem(destination, source, p_headers[i].p_filesz);

        // Fill BSS
        SetMem(destination + p_headers[i].p_filesz, p_headers[i].p_memsz - p_headers[i].p_filesz, 0);

    }

    // Set entry point
    *kernel_entry = (void (*)(BootInfo *))e_header->e_entry;

    return EFI_SUCCESS;

}

static EFI_STATUS get_framebuffer(BootInfo *boot_info) {

    EFI_STATUS status;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

    // Get graphics output protocol
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gEfiGraphicsOutputProtocolGuid, NULL, (void **)&gop);
    if (EFI_ERROR(status)) return status;

    // Fill boot info with framebuffer information
    boot_info->framebuffer.address = (void *)gop->Mode->FrameBufferBase;
    boot_info->framebuffer.width = gop->Mode->Info->HorizontalResolution;
    boot_info->framebuffer.height = gop->Mode->Info->VerticalResolution;
    boot_info->framebuffer.pitch = gop->Mode->Info->PixelsPerScanLine;

    return EFI_SUCCESS;

}

static EFI_STATUS get_memory_map(BootInfo *boot_info) {

    EFI_STATUS status;
    u64 memory_map_key = 0;
    u64 memory_map_size = 0;
    u64 memory_map_descriptor_size = 0;
    u64 memory_map_descriptor_version = 0;

    // Get size of memory map by failing successfully
    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &memory_map_size, NULL, &memory_map_key, &memory_map_descriptor_size, &memory_map_descriptor_version);
    if (status != EFI_BUFFER_TOO_SMALL) {
        return status;
    }

    // Add extra space and allocate memory for memory map
    memory_map_size += 4 * memory_map_descriptor_size;

    status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, memory_map_size, &boot_info->memory.map);
    if (EFI_ERROR(status)) {
        return status;
    }

    // Get memory map
    status = uefi_call_wrapper(BS->GetMemoryMap, 5, &memory_map_size, boot_info->memory.map, &memory_map_key, &memory_map_descriptor_size, &memory_map_descriptor_version);
    if (EFI_ERROR(status)) {
        uefi_call_wrapper(BS->FreePool, 1, boot_info->memory.map);
        return status;
    }

    // Fill boot info with memory map and metadata
    boot_info->memory.map_size = memory_map_size;
    boot_info->memory.map_descriptor_size = memory_map_descriptor_size;
    boot_info->memory.map_descriptor_version = memory_map_descriptor_version;
    boot_info->memory.map_key = memory_map_key;

    return EFI_SUCCESS;

}

static EFI_STATUS exit_boot_services(EFI_HANDLE image_handle, BootInfo *boot_info) {

    EFI_STATUS status;

    // Exit boot services assuming failure
    status = uefi_call_wrapper(BS->ExitBootServices, 2, image_handle, boot_info->memory.map_key);

    if (status == EFI_INVALID_PARAMETER) {

        // Get new memory map
        status = uefi_call_wrapper(BS->GetMemoryMap, 5, &boot_info->memory.map_size, boot_info->memory.map, &boot_info->memory.map_key, 
            &boot_info->memory.map_descriptor_size, &boot_info->memory.map_descriptor_version);
        if (EFI_ERROR(status)) {
            return status;
        }

        // Retry exiting boot services
        status = uefi_call_wrapper(BS->ExitBootServices, 2, image_handle, boot_info->memory.map_key);

    }

    return status;

}
