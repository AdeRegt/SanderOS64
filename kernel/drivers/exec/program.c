#include "../../include/exec/program.h"
#include "../../include/exec/elf.h"
#include "../../include/device.h"
#include "../../include/memory.h"
#include "../../include/paging.h"
#include "../../include/multitasking.h"
#include "../../include/graphics.h"

int use_paging = 1;

int exec(uint8_t *path,char *argv){

    k_printf("Trying to execute %s with %s \n",path,argv);

    //
    // First load the image....
    uint64_t fz = getFileSize(path);

    void* buffer = requestBigPage();
    
    char raw_program = readFile((uint8_t*)path,buffer);
    if(raw_program==0){
        return -1;
    }

    //
    // Then interpetate the engine....
    uint64_t address = 0;
    if(is_elf(buffer)){
        if(use_paging){
            return -1;
        }
        address = elf_load_image(buffer);
        if(!address){
            return -1;
        }
    }else{
        if(use_paging){
            addTask(buffer,buffer,fz);
            return 0xCDCDCD;
        }else{
            memcpy((void*)EXTERNAL_PROGRAM_ADDRESS,buffer,fz);
            address = EXTERNAL_PROGRAM_ADDRESS;
        }
    }

    // call!
    int (*callProgram)(int argc,char* argv) = (void*)address;
    return callProgram(0,0);
}