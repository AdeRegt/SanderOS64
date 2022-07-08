#include "../../include/exec/program.h"
#include "../../include/exec/elf.h"
#include "../../include/device.h"
#include "../../include/memory.h"

// #include "../../include/graphics.h"

int exec(uint8_t *path,uint8_t **argv){

    //
    // First load the image....
    void* buffer = requestPage();
    char raw_program = readFile((uint8_t*)path,buffer);
    if(!raw_program){
        return 0;
    }

    //
    // Then interpetate the engine....
    uint64_t address = elf_load_image(buffer);
    if(!address){
        return 0;
    }

    //
    // Act like we are something else....


    return 1;
}