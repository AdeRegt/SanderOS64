#include <efi.h>
#include <efilib.h>
#include "../../include/graphics.h"
#include "../../include/kernel.h"

GraphicsInfo framebuffer;
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

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;

    ST = SystemTable;

    GraphicsInfo* newBuffer = InitializeGOP();
    kernel_main(newBuffer);
 
    return Status;
}