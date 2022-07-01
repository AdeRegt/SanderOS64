#include "../../include/exec/module.h"
#include "../../include/pci.h"
#include "../../include/device.h"
#include "../../include/memory.h"

char loadModule(char* path, PCIInfo *pci){
    // request paging
    void *programmem = requestPage();

    // loading file
    if( !readFile(path,programmem) ){
        return 0;
    }

    uint64_t entrypoint = elf_load_image(programmem);
    if(entrypoint==0){
        return 0;
    }

	void (*DriverStart)(PCIInfo* pi) = ((__attribute__((sysv_abi)) void (*)(PCIInfo*)) entrypoint );
    DriverStart(pci);
    return 1;
}