#include <efi.h>
#include <efilib.h>
#include "../../include/graphics.h"
#include "../../include/memory.h"
#include "../../include/kernel.h"

GraphicsInfo framebuffer;
EFI_HANDLE IH;

GraphicsInfo* InitializeGOP(){
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
	EFI_STATUS status;

    status = ST->BootServices->LocateProtocol(&gopGuid, NULL, (void**)&gop);
	if(EFI_ERROR(status)){
        ST->ConOut->OutputString(ST->ConOut, L"Unable to locate GOP\n\r");
		return NULL;
	}
	else
	{
        ST->ConOut->OutputString(ST->ConOut, L"GOP located\n\r");
	}

	framebuffer.BaseAddress = (void*)gop->Mode->FrameBufferBase;
	framebuffer.BufferSize = gop->Mode->FrameBufferSize;
	framebuffer.Width = gop->Mode->Info->HorizontalResolution;
	framebuffer.Height = gop->Mode->Info->VerticalResolution;
	framebuffer.PixelsPerScanLine = gop->Mode->Info->PixelsPerScanLine;

	return &framebuffer;
	
}

void quitUEFI(){
	EFI_MEMORY_DESCRIPTOR* Map = NULL;
	UINTN MapSize, MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
	ST->BootServices->GetMemoryMap(&MapSize, Map, &MapKey, &DescriptorSize, &DescriptorVersion);
	ST->BootServices->ExitBootServices(IH, MapKey);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;

	IH = ImageHandle;
    ST = SystemTable;

    GraphicsInfo* GIB = InitializeGOP();
	quitUEFI();
    kernel_main(GIB);
 
    return Status;
}