#include <efi.h>
#include <efilib.h>
#include "elf.h"

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
        Elf64_Ehdr header;
        UINTN size = sizeof(header);
        kernel_file_link->Read(kernel_file_link, &size, &header);
        Status = ST->ConOut->OutputString(ST->ConOut, L"Header loaded into memory\r\n");

        Elf64_Phdr* phdrs;
		kernel_file_link->SetPosition(kernel_file_link, header.e_phoff);
		UINTN size2 = header.e_phnum * header.e_phentsize;
		SystemTable->BootServices->AllocatePool(EfiLoaderData, size2, (void**)&phdrs);
		kernel_file_link->Read(kernel_file_link, &size2, phdrs);
        Status = ST->ConOut->OutputString(ST->ConOut, L"Header locations loaded into memory\r\n");

        for (Elf64_Phdr* phdr = phdrs;(char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)){
            if (phdr->p_type==PT_LOAD){
                int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
                Elf64_Addr segment = phdr->p_paddr;
                SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);

                kernel_file_link->SetPosition(kernel_file_link, phdr->p_offset);
                UINTN size = phdr->p_filesz;
                kernel_file_link->Read(kernel_file_link, &size, (void*)segment);
            }
        }
        Status = ST->ConOut->OutputString(ST->ConOut, L"All locations are loaded into memory!\r\n");

        typedef int func(void);
        func* kernel_main_routine = (func*)header.e_entry;
        int i = kernel_main_routine();
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