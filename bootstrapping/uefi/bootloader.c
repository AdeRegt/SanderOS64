#include <efi.h>
#include <efilib.h>

// THANKS TO : 
// https://www.youtube.com/watch?v=FnRKA8JaxYE&list=PLxN4E629pPnJxCQCLy7E0SQY_zuumOVyZ&index=2
// https://github.com/Absurdponcho/PonchoOS/blob/main/gnu-efi/bootloader/main.c

EFI_FILE* LoadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable){
	EFI_FILE* LoadedFile;

	EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
	SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
	SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

	if (Directory == NULL){
		FileSystem->OpenVolume(FileSystem, &Directory);
	}

	EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
	if (s != EFI_SUCCESS){
		return NULL;
	}
	return LoadedFile;

}
 
EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;
    EFI_INPUT_KEY Key;

    ST = SystemTable;

    CHAR16* kernelfilename = L"sanderos\\kernel.bin";
    Status = ST->ConOut->OutputString(ST->ConOut, L"About to load ");
    Status = ST->ConOut->OutputString(ST->ConOut, kernelfilename);
    Status = ST->ConOut->OutputString(ST->ConOut, L"! \r\n");
    EFI_FILE* kernel_file_link = LoadFile(NULL, kernelfilename, ImageHandle, SystemTable);
    if(kernel_file_link==NULL){
        Status = ST->ConOut->OutputString(ST->ConOut, L"Unable to find ");
        Status = ST->ConOut->OutputString(ST->ConOut, kernelfilename);
        Status = ST->ConOut->OutputString(ST->ConOut, L"! \r\n");
    }else{
        Status = ST->ConOut->OutputString(ST->ConOut, L"Kernel is found!\r\n");
    }
 
    Status = ST->ConOut->OutputString(ST->ConOut, L"EOS: End of system reached!\r\n");
    if (EFI_ERROR(Status)){
        return Status;
    }
    Status = ST->ConIn->Reset(ST->ConIn, FALSE);
    if (EFI_ERROR(Status)){
        return Status;
    }
    while ((Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY) ;
 
    return Status;
}