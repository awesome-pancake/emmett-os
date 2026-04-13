#include <efi.h>
#include <efilib.h>

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
{
  EFI_LOADED_IMAGE *loaded_image = NULL;
  EFI_GUID lip_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  EFI_FILE_IO_INTERFACE *io_volume;
  EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  EFI_FILE_HANDLE volume;

  // Get loaded image protocol interface
  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lip_guid, (VOID**)&loaded_image);
  
  // Get the volume handle
  uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fs_guid, (VOID**)&io_volume);
  uefi_call_wrapper(io_volume->OpenVolume, 2, io_volume, &volume);

  return volume;
}

UINT64 FileSize(EFI_FILE_HANDLE file_handle)
{
  UINT64 size;
  EFI_FILE_INFO *file_info;

  file_info = LibFileInfo(file_handle);
  size = file_info->FileSize;
  FreePool(file_info);
  return size;
}

EFI_STATUS GetMapKey(UINT64 *map_key)
{
  // Get memory map size and initialize some variables
  EFI_STATUS status;
  UINTN map_size = 0;
  EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
  UINTN descriptor_size = 0;
  UINT32 descriptor_version = 0;

  status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, &memory_map, NULL, &descriptor_size, NULL);
  if(status != EFI_BUFFER_TOO_SMALL && status != EFI_SUCCESS){
    Print(L"Could not get memory map size: %d\r\n", status);
    return status;
  }

  // Accounts for the increase in size of the memory map after allocation
  map_size += 2 * descriptor_size;

  // Allocate space for memory map
  status = uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, map_size, (void **)&memory_map);
  if(EFI_ERROR(status)){
    Print(L"Could not allocate memory map: %d\r\n", status);
    return status;
  }

  // Get memory map
  status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, memory_map, map_key, &descriptor_size, &descriptor_version);
  if(EFI_ERROR(status)){
    Print(L"Could not get memory map: %d\r\n", status);
    return status;
  }

  return EFI_SUCCESS;
}

EFI_PHYSICAL_ADDRESS GetFrameBuffer()
{
  EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

  // Locate graphics output protocol
  uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop);

  return gop->Mode->FrameBufferBase;
}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  InitializeLib(ImageHandle, SystemTable);

  EFI_STATUS status;
  EFI_PHYSICAL_ADDRESS kernel_buffer = 0;
  EFI_FILE_HANDLE volume = GetVolume(ImageHandle);
  EFI_FILE_HANDLE file_handle;
  CHAR16 *file_name = L"test.bin";

  // Open file
  status = uefi_call_wrapper(volume->Open, 5, volume, &file_handle, file_name, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
  if(EFI_ERROR(status)){
    Print(L"(ERROR) Could not open file, or find volume. Exit code: %d\r\n", status);
    return status;
  }

  Print(L"Successfully opened kernel file.\r\n");

  // Size of the file
  UINT64 byte_size;
  UINT64 page_size;
  byte_size = FileSize(file_handle);
  page_size = (UINT64)(1.0 + byte_size/0x1000);

  // Add additional pages for kernel stack
  // page_size += 2;

  Print(L"Kernel size: %d bytes, %d pages.\r\n", byte_size, page_size);
  
  // Allocate pages
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

  EFI_PHYSICAL_ADDRESS frame_buffer = GetFrameBuffer();

  UINT64 map_key = 0;
  status = GetMapKey(&map_key);
  if(EFI_ERROR(status)){
    Print(L"(ERROR) Could not get memory map. Exit code: %d\r\n", status);
    return status;
  }

  // Exit boot services
  status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, map_key);
  if(EFI_ERROR(status)){
    Print(L"(ERROR) Could not exit boot services. Exit code: %d\r\n", status);
    return status;
  }
  
  // TODO: Pass frame buffer and memory map to the kernel
  // Set ebp, esp
  // Put frame buffer and memory map as arguments
  // Jmp to kernel code

  asm( // Jump to kernel
    "jmp *%0"
    :
    :"r"(kernel_buffer)
  );
  
  // Just in case
  for(;;);
}