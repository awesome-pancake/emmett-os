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

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  InitializeLib(ImageHandle, SystemTable);

  EFI_STATUS status;
  EFI_PHYSICAL_ADDRESS kernel_buffer;
  EFI_FILE_HANDLE volume = GetVolume(ImageHandle);
  EFI_FILE_HANDLE file_handle;
  CHAR16 *file_name = L"TEST.TXT";

  // Open file
  status = uefi_call_wrapper(volume->Open, 5, volume, &file_handle, file_name, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
  if(EFI_ERROR(status)){
    Print(L"Could not open file, or find volume.\r\n");
    return status;
  }

  // Size of the file
  UINT64 byte_size;
  UINT64 page_size;
  byte_size = FileSize(file_handle);
  page_size = (UINT64)(1.0 + byte_size/0x1000);

  // Allocate pages
  status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, page_size, kernel_buffer);
  if(EFI_ERROR(status)){
    Print(L"Could not allocate pages.\r\n");
    return status;
  }

  // Read kernel
  status = uefi_call_wrapper(file_handle->Read, 3, file_handle, &byte_size, (UINT8*)kernel_buffer);
  if(EFI_ERROR(status)){
    Print(L"Could not read file.\r\n");
    return status;
  }

  // Close file
  status = uefi_call_wrapper(file_handle->Close, 1, file_handle);
  if(EFI_ERROR(status)){
    Print(L"Could not close file.");
    return status;
  }

  // Get memory map size and initialize some variables
  UINTN map_size = 0;
  EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
  UINTN map_key = 0;
  UINTN descriptor_size = 0;
  UINT32 descriptor_version = 0;
  status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, &memory_map, NULL, &descriptor_size, NULL);
  
  map_size += 2 * descriptor_size;

  // Get memory map
  uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, map_size, (void **)&memory_map);
  status = uefi_call_wrapper(BS->GetMemoryMap, 5, &map_size, &memory_map, &map_key, &descriptor_size, &descriptor_version);
  if(EFI_ERROR(status)){
    Print(L"Could not retreive memory map.\r\n");
    return status;
  }

  // Exit boot services
  status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, map_key);
  if(EFI_ERROR(status)){
    Print(L"Could not exit boot services.\r\n");
    return status;
  }

  for(;;);

  // TODO: Pass frame buffer and memory map to the kernel
}