#include "../include/tty.h"
#include "../include/graphics.h"
#include "../include/ps2.h"
#include "../include/memory.h"
#include "../include/device.h"
#include "../include/exec/program.h"

void initialise_tty(){
    clear_screen(0xFFFFFFFF);
    k_printf("SanderOS64 Buildin Command Interpeter\n");
    k_printf("Known programs: \n");
    k_printf("%s \n\n",dir("A:PROGRAMS"));
    while(1){
        k_printf("> ");
        uint8_t *tw = scanLine(25);
        k_printf("\n");
        if(strcmp(tw,"exit",4)){
            break;
        }else{
            int rt = exec(tw,"TEST");
            if(rt==0xCDCDCD){
                k_printf("Program is running in the background\n");
            }else if(rt==-1){
                k_printf("Unable to run program!\n");
            }else{
                k_printf("Program exited with %d \n",rt);
            }
        }
        free(tw);
    }
}