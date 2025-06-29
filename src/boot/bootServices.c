/* bootServices.c */

#include <boot/bootServices.h>

EFI_STATUS KERNEL_ExitBootService(preboot_info_t* preboot_info,
                                  EFI_HANDLE ImageHandle,
                                  EFI_SYSTEM_TABLE* SystemTable)
{
    /* First call to get required memory map buffer size */
    EFI_STATUS Status = SystemTable->BootServices->GetMemoryMap(
        &preboot_info->MemoryMapSize, preboot_info->MemoryMap, &preboot_info->MapKey,
        &preboot_info->DescriptorSize, &preboot_info->DescriptorVersion);

    if (Status != EFI_BUFFER_TOO_SMALL)
    {
        Print(L"Unexpected error getting memory map size: %r\n", Status);
        return Status;
    }

    /* Allocate memory for the map with extra padding */
    preboot_info->MemoryMapSize += 2 * preboot_info->DescriptorSize;
    Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, preboot_info->MemoryMapSize,
                                                     (void**)&preboot_info->MemoryMap);

    if (EFI_ERROR(Status))
    {
        Print(L"Failed to allocate memory map buffer: %r\n", Status);
        return Status;
    }
    /* Second call to actually get the memory map */
    Status = SystemTable->BootServices->GetMemoryMap(
        &preboot_info->MemoryMapSize, preboot_info->MemoryMap, &preboot_info->MapKey,
        &preboot_info->DescriptorSize, &preboot_info->DescriptorVersion);
    if (EFI_ERROR(Status))
    {
        Print(L"Failed to get memory map: %r\n", Status);
        SystemTable->BootServices->FreePool(preboot_info->MemoryMap);
        return Status;
    }

    /* Try ExitBootServices - it might fail if memory map changed */
    Status = SystemTable->BootServices->ExitBootServices(ImageHandle, preboot_info->MapKey);
    if (Status == EFI_INVALID_PARAMETER)
    {
        /* Memory map changed - try again */
        Status = SystemTable->BootServices->GetMemoryMap(
            &preboot_info->MemoryMapSize, preboot_info->MemoryMap, &preboot_info->MapKey,
            &preboot_info->DescriptorSize, &preboot_info->DescriptorVersion);

        if (EFI_ERROR(Status))
        {
            Print(L"Failed to get updated memory map: %r\n", Status);
            SystemTable->BootServices->FreePool(preboot_info->MemoryMap);
            return Status;
        }

        Status = SystemTable->BootServices->ExitBootServices(ImageHandle, preboot_info->MapKey);
    }

    if (EFI_ERROR(Status))
    {
        Print(L"ExitBootServices failed: %r\n", Status);
        SystemTable->BootServices->FreePool(preboot_info->MemoryMap);
        return Status;
    }
}

EFI_STATUS
KERNEL_FindMemoryRegions(preboot_info_t* preboot_info, MemoryRegion* regions, size_t regions_count)
{
}