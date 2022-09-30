#include "../../include/exec/module.h"
#include "../../include/pci.h"
#include "../../include/device.h"
#include "../../include/memory.h"

char loadModule(char* path, PCIInfo *pci){
    // request paging
    upointer_t fz = getFileSize(path);
    void *programmem = requestPage();
    for(upointer_t i = 0 ; i < (fz/0x1000) ; i++){
        requestPage();
    }

    // loading file
    if( !readFile(path,programmem) ){
        return 0;
    }

    upointer_t entrypoint = elf_load_image(programmem);
    if(entrypoint==0){
        return 0;
    }

	void (*DriverStart)(PCIInfo* pi) = ((__attribute__((sysv_abi)) void (*)(PCIInfo*)) entrypoint );
    DriverStart(pci);
    return 1;
}