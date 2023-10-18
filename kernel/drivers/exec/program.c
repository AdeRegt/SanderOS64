#include "../../include/exec/program.h"
#include "../../include/exec/elf.h"
#include "../../include/device.h"
#include "../../include/memory.h"
#include "../../include/paging.h"
#include "../../include/multitasking.h"
#include "../../include/graphics.h"
#include "../../include/exec/debugger.h"
#include "../../include/exec/sxe.h"

#ifdef __x86_64
int use_paging = 1;
#else 
int use_paging = 0;
#endif

int exec_memory(void* buffer,char *argv){
     k_printf("Trying to execute %s with %s \n","{memory}",argv==0?"<no args>":argv);
    char *data[10];// = {path, argv};
    for(int i = 0 ; i < 10 ; i++){
        data[i] = 0;
    }
    data[0] = "{memory}";
    int asc = 1;
    if(argv!=0){
        char* buffer2 = 0;
        int z = 0;
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
                data[asc++] = buffer2;
            }
            z++;
        }
    }

    //
    // Then interpetate the engine....
    upointer_t address = 0;
    if(is_elf(buffer)){
        address = elf_load_image(buffer);
        if(!address){
            return -1;
        }
    }else if(is_sxe(buffer)){
        return sxe_run(buffer);
    }else if(use_paging){
        return addTask(buffer,buffer,0x1000,data);
    }else{
        memcpy((void*)EXTERNAL_PROGRAM_ADDRESS,buffer,0x1000);
        address = EXTERNAL_PROGRAM_ADDRESS;
    }

    // call!
    k_printf("exec: running BIN program at %x \n",address);
    int (*callProgram)(int,char**) = (void*)address;
    char **dictionary = data;
    return callProgram(1 + asc,dictionary);
}

int exec(uint8_t *path,char *argv){

    k_printf("Trying to execute %s with %s \n",path,argv==0?"<no args>":argv);
    char *data[10];// = {path, argv};
    for(int i = 0 ; i < 10 ; i++){
        data[i] = 0;
    }
    data[0] = path;
    int asc = 1;
    if(argv!=0){
        char* buffer2 = 0;
        int z = 0;
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
                data[asc++] = buffer2;
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
    }else if(is_sxe(buffer)){
        return sxe_run(buffer);
    }else if(use_paging){
        return addTask(buffer,buffer,fz,data);
    }else{
        memcpy((void*)EXTERNAL_PROGRAM_ADDRESS,buffer,fz);
        address = EXTERNAL_PROGRAM_ADDRESS;
    }

    // call!
    k_printf("exec: running BIN program at %x \n",address);
    int (*callProgram)(int,char**) = (void*)address;
    char **dictionary = data;
    return callProgram(1 + asc,dictionary);
}