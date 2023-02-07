#include "../../include/exec/program.h"
#include "../../include/exec/elf.h"
#include "../../include/device.h"
#include "../../include/memory.h"
#include "../../include/paging.h"
#include "../../include/multitasking.h"
#include "../../include/graphics.h"
#include "../../include/exec/debugger.h"

#ifdef __x86_64
int use_paging = 1;
#else 
int use_paging = 0;
#endif

int exec(uint8_t *path,char *argv){

    k_printf("Trying to execute %s with %s \n",path,argv==0?"<no args>":argv);
    char** argstock = (char**) requestPage();
    for(int i = 0 ; i < 10 ; i++){
        argstock[i] = 0;
    }
    if(argv!=0){
        char* buffer2 = 0;
        int z = 0;
        int asc = 0;
        while(1){
            char deze = argv[z];
            if(deze==0){
                break;
            }
            if(deze==' '){
                argv[z] = 0;
                z++;
                buffer2 = 0;
                continue;
            }
            if(buffer2==0){
                buffer2 = (char*)(argv + z);
                argstock[asc++] = buffer2;
            }
            z++;
        }
    }

    //
    // First load the image....
    uint64_t fz = getFileSize(path);
    if(fz==0){
        return -1;
    }

    void* buffer = requestBigPage();
    
    char raw_program = readFile((uint8_t*)path,buffer);
    if(raw_program==0){
        return -1;
    }

    //
    // Then interpetate the engine....
    upointer_t address = 0;
    if(is_elf(buffer)){
        address = elf_load_image(buffer);
        if(!address){
            return -1;
        }
    }else{
        if(use_paging){
            return addTask(buffer,buffer,fz,argstock);
        }else{
            memcpy((void*)EXTERNAL_PROGRAM_ADDRESS,buffer,fz);
            address = EXTERNAL_PROGRAM_ADDRESS;
        }
    }

    // call!
    int (*callProgram)(int argc,char* argv) = (void*)address;
    return callProgram(0,0);
}