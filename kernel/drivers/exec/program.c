#include "../../include/exec/program.h"
#include "../../include/exec/elf.h"
#include "../../include/device.h"
#include "../../include/memory.h"
#include "../../include/paging.h"
#include "../../include/multitasking.h"
#include "../../include/graphics.h"

int exec(uint8_t *path,char **argv){

    //
    // First load the image....
    uint64_t fz = getFileSize(path);

    void* buffer = requestPage();
    for(uint64_t i = 0 ; i < ((fz/0x1000)+1) ; i++){
        requestPage();
    }
    char raw_program = readFile((uint8_t*)path,buffer);
    if(raw_program==0){
        return 0;
    }

    //
    // Then interpetate the engine....
    uint64_t address = 0;
    if(is_elf(buffer)){
        address = elf_load_image(buffer);
        if(!address){
            return 0;
        }
    }else{
        memcpy((void*)EXTERNAL_PROGRAM_ADDRESS,buffer,fz);
        address = EXTERNAL_PROGRAM_ADDRESS;
    }

    // call!
    int (*callProgram)(int argc,char** argv) = (void*)address;
    return callProgram(0,0);
}