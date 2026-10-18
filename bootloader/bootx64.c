// Bootloader
// Emmett Hoffman
// June 12, 2026
// 
// Description:
// - The bootloader is a UEFI application that loads the kernel and jumps to it.
// - Task order:
//     - Open kernel file
//     - Allocate space for the kernel
//     - Read the kernel to memory
//     - Read the .elf header to determine the kernel entry point
//     - Jump to the kernel

#include <efi.h>
#include <efilib.h>

// Kernel display structure
typedef struct{
        uint8_t     *frame_buffer;
        uint32_t    horizontal_resolution;
        uint32_t    vertical_resolution;
} DISPLAY;

// Kernel memory map
typedef struct{
        EFI_MEMORY_DESCRIPTOR           *descriptor_table;
        uint64_t                        map_size;
} KERNEL_MEMMAP;

EFI_FILE_HANDLE getVolume(EFI_HANDLE image)
{
        // Initialize loaded image protocol and file system protocol GUIDs
        EFI_GUID lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
        EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

        // Initialize variables
        EFI_LOADED_IMAGE *loaded_image = NULL;
        EFI_FILE_IO_INTERFACE *io_volume;
        EFI_FILE_HANDLE volume;

        // Get loaded image protocol interface
        uefi_call_wrapper(BS->HandleProtocol, 3, image, &lip_guid, (VOID**)&loaded_image);
        
        // Get the volume handle
        uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fs_guid, (VOID**)&io_volume);
        uefi_call_wrapper(io_volume->OpenVolume, 2, io_volume, &volume);

        return volume;
}

UINT64 fileSize(EFI_FILE_HANDLE file_handle)
{
        // Initialize size and file_info
        UINT64 size;
        EFI_FILE_INFO *file_info;

        // Find size of a given file handle
        file_info = LibFileInfo(file_handle);
        size = file_info->FileSize;
        FreePool(file_info);

        return size;
}

EFI_STATUS getMapKey(UINT64 *map_key, UINTN *size, EFI_MEMORY_DESCRIPTOR **output_map)
{
        // Get memory map size and initialize some variables
        EFI_STATUS status;
        UINTN map_size = 0;
        EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
        UINTN descriptor_size = 0;
        UINT32 descriptor_version = 0;

        // The first call to GetMemoryMap is expected to return the memory map size as well as descriptor size
        status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, *output_map, NULL, &descriptor_size, NULL);
        if(status != EFI_BUFFER_TOO_SMALL && status != EFI_SUCCESS){
                Print(L"Could not get memory map size: %d\r\n", status);
                return status;
        }

        // Accounts for the increase in size of the memory map after allocation
        map_size += 2 * descriptor_size;

        // Allocate space for memory map
        status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, map_size, (void **)output_map);
        if(EFI_ERROR(status)){
                Print(L"Could not allocate memory map: %d\r\n", status);
                return status;
        }

        // Get memory map
        status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, *output_map, map_key, &descriptor_size, &descriptor_version);
        if(EFI_ERROR(status)){
                Print(L"Could not get memory map: %d\r\n", status);
                return status;
        }

        *size = map_size;

        return EFI_SUCCESS;
}

DISPLAY *getDisplay()
{
        DISPLAY *display = AllocatePool(sizeof(DISPLAY));
        EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
        EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

        // Locate graphics output protocol
        uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop);

        display->frame_buffer = (uint8_t*)gop->Mode->FrameBufferBase;
        display->vertical_resolution = gop->Mode->Info->VerticalResolution;
        display->horizontal_resolution = gop->Mode->Info->PixelsPerScanLine;
        return display;
}

EFI_STATUS getEntryPoint(UINT64 header, UINT64 *start)
{
        // Reads the entry point for the kernel off of the elf header
        UINT64 entry_point = header + 0x18;
        *start = *(UINT64*)entry_point;

        return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
        InitializeLib(ImageHandle, SystemTable);

        // Status of each UEFI call
        EFI_STATUS status;
        
        // Initialize necessary information for kernel
        EFI_PHYSICAL_ADDRESS kernel_buffer = 0;
        EFI_FILE_HANDLE volume = getVolume(ImageHandle);
        EFI_FILE_HANDLE file_handle;
        CHAR16 *file_name = L"kernel.elf";

        // Kernel entrypoint
        void (*kernelMain)(DISPLAY *disp, KERNEL_MEMMAP *mem_map);

        // Open kernel executable
        status = uefi_call_wrapper(volume->Open, 5, volume, &file_handle, file_name, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
        if(EFI_ERROR(status)){
                Print(L"(ERROR) Could not open file, or find volume. Exit code: %d\r\n", status);
                return status;
        }

        Print(L"Successfully opened kernel file.\r\n");

        // Find the size of the kernel for allocation
        UINT64 byte_size;
        UINT64 page_size;
        byte_size = fileSize(file_handle);
        page_size = (UINT64)(1.0 + byte_size/0x1000);

        Print(L"Kernel size: %d bytes, %d pages.\r\n", byte_size, page_size);
        
        // Allocate pages for kernel
        status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, page_size, &kernel_buffer);
        if(EFI_ERROR(status)){
                Print(L"(ERROR) Could not allocate pages. Exit code: %d\r\n", status);
                return status;
        }

        Print(L"Successfully allocated space for the kernel: %X\r\n", kernel_buffer);

        // Read kernel
        status = uefi_call_wrapper(file_handle->Read, 3, file_handle, &byte_size, (VOID*)kernel_buffer);
        if(EFI_ERROR(status)){
                Print(L"(ERROR) Could not read file. Exit code: %d\r\n", status);
                return status;
        }

        Print(L"Successfully read kernel.\r\n");

        // Close file
        status = uefi_call_wrapper(file_handle->Close, 1, file_handle);
        if(EFI_ERROR(status)){
                Print(L"(ERROR) Could not close file. Exit code: %d\r\n", status);
                return status;
        }

        Print(L"Successfully closed file.\r\n");

        // Determine entry offset within kernel file
        UINT64 entry_offset = 0;
        status = getEntryPoint(kernel_buffer, &entry_offset);
        if(EFI_ERROR(status)){
                Print(L"(ERROR) Could not get kernel entry point. Exit code: %d\r\n", status);
                return status;
        }

        Print(L"Successfully obtained kernel entry point: %X", entry_offset);

        DISPLAY *display = getDisplay();
        EFI_MEMORY_DESCRIPTOR *descriptor_table = NULL;
        UINTN map_size = 0;
        UINT64 map_key = 0;
        KERNEL_MEMMAP memory_map;

        // Get memory map
        status = getMapKey(&map_key, &map_size, &descriptor_table);
        if(EFI_ERROR(status)){
                Print(L"(ERROR) Could not get memory map. Exit code: %d\r\n", status);
                return status;
        }

        memory_map.map_size = map_size;
        memory_map.descriptor_table = descriptor_table;

        // Exit boot services
        status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, map_key);
        if(EFI_ERROR(status)){
                Print(L"(ERROR) Could not exit boot services. Exit code: %d\r\n", status);
                return status;
        }

        // Jump to kernel
        kernelMain = (void*)(kernel_buffer + entry_offset);
        kernelMain(display, &memory_map);
        
        // Halt execution in a loop in case the jump fails
        for(;;);
}