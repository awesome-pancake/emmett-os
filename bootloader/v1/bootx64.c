#include <efi.h>
#include <efilib.h>

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
{
  EFI_LOADED_IMAGE *loadedImage = NULL;
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
  EFI_FILE_IO_INTERFACE *IOVolume;
  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
  EFI_FILE_HANDLE volume;

  // Get loaded image protocol interface
  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (VOID**)&loadedImage);
  
  // Get the volume handle
  uefi_call_wrapper(BS->HandleProtocol, 3, loadedImage->DeviceHandle, &fsGuid, (VOID**)&IOVolume);
  uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &volume);

  return volume;
}

UINT64 FileSize(EFI_FILE_HANDLE fileHandle)
{
  UINT64 size;
  EFI_FILE_INFO *fileInfo;

  fileInfo = LibFileInfo(fileHandle);
  size = fileInfo->FileSize;
  FreePool(fileInfo);
  return size;
}

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
  InitializeLib(ImageHandle, SystemTable);

  EFI_STATUS status;
  EFI_PHYSICAL_ADDRESS kernelBuffer;
  EFI_FILE_HANDLE volume = GetVolume(ImageHandle);
  EFI_FILE_HANDLE fileHandle;
  CHAR16 *fileName = L"TEST.TXT";

  // Open file
  status = uefi_call_wrapper(volume->Open, 5, volume, &fileHandle, fileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
  if(EFI_ERROR(status)){
    Print(L"Could not open file, or find volume.\r\n");
    return status;
  }

  // Size of the file
  UINT64 byteSize;
  UINT64 pageSize;
  byteSize = FileSize(fileHandle);
  pageSize = (UINT64)(1.0 + byteSize/0x1000);

  // Allocate pages
  status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAnyPages, EfiLoaderData, pageSize, kernelBuffer);
  if(EFI_ERROR(status)){
    Print(L"Could not allocate pages.\r\n");
    return status;
  }

  // Read kernel
  status = uefi_call_wrapper(fileHandle->Read, 3, fileHandle, &byteSize, (UINT8*)kernelBuffer);
  if(EFI_ERROR(status)){
    Print(L"Could not read file.\r\n");
    return status;
  }

  // Close file
  status = uefi_call_wrapper(fileHandle->Close, 1, fileHandle);
  if(EFI_ERROR(status)){
    Print(L"Could not close file.");
    return status;
  }

  // Get memory map size and initialize some variables
  UINTN mapSize = 0;
  EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;
  UINTN mapKey = 0;
  UINTN descriptorSize = 0;
  UINT32 descriptorVersion = 0;
  status = uefi_call_wrapper(BS->GetMemoryMap, 5, &mapSize, &memoryMap, NULL, &descriptorSize, NULL);
  
  mapSize += 2 * descriptorSize;

  // Get memory map
  uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, mapSize, (void **)&memoryMap);
  status = uefi_call_wrapper(BS->GetMemoryMap, 5, &mapSize, &memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
  if(EFI_ERROR(status)){
    Print(L"Could not retreive memory map.\r\n");
    return status;
  }

  // Exit boot services
  status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, mapKey);
  if(EFI_ERROR(status)){
    Print(L"Could not exit boot services.\r\n");
    return status;
  }

  for(;;);

  // TODO: Pass frame buffer and memory map to the kernel
}