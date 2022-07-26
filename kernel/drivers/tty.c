#include "../include/tty.h"
#include "../include/graphics.h"
#include "../include/ps2.h"
#include "../include/memory.h"
#include "../include/device.h"
#include "../include/exec/program.h"
#include "../include/multitasking.h"

char wd[25];
char pd[25];

void initialise_tty(){
    clear_screen(0xFFFFFFFF);

    memset((void*)&wd,0,25);
    memcpy((void*)&wd,"A:PROGRAMS",strlen("A:PROGRAMS"));

    k_printf("SanderOS64 Buildin Command Interpeter\n");
    k_printf("Known programs: \n");
    k_printf("%s\n",dir(wd));
    k_printf("Commands: \n");
    k_printf("exit               - Exit the shell\n");
    k_printf("dir                - Print contents of the working directory\n");
    k_printf("cd        [path]   - Change working directory\n");
    k_printf("\n");
    while(1){
        k_printf("%s > ",wd);
        uint8_t *tw = scanLine(25);
        k_printf("\n");
        if(strcmp(tw,"exit",4)){
            break;
        }else if(strcmp(tw,"dir",3)){
            k_printf("%s\n",dir(wd));
        }else if(strcmp(tw,"cd",2)){
            char *strpath = (char*) (tw+3);
            if(strpath[1]==':'){
                memset((void*)&wd,0,25);
                memcpy((void*)&wd,strpath,strlen(strpath));
            }else{
                if(strlen((char*)&wd)>2){
                    memcpy((void*)(((uint64_t)&wd)+strlen((char*)&wd) + -1 ),"/",1);
                }
                memcpy((void*)(((uint64_t)&wd)+strlen((char*)&wd) + -1 ),strpath,strlen(strpath));
            }
        }else{
            memset((void*)&pd,0,25);
            if(tw[1]==':'){
                memcpy((void*)&pd,tw,strlen(tw));
            }else{
                int m1 = -1;
                memcpy((void*)&pd,(char*)&wd,strlen((char*)&wd));
                if(strlen((char*)&pd)>2){
                    memcpy((void*)(((uint64_t)&pd) + strlen((char*)&wd) -1  ),"/",1);
                    m1 = 0;
                }
                memcpy((void*)(((uint64_t)&pd) + strlen((char*)&wd) + m1  ),tw,strlen(tw));
            }
            int rt = exec(pd,"TEST");
            if(rt<1000){
                k_printf("Program is running in the background\n");
                waitForPid(rt);
            }else if(rt==-1){
                k_printf("Unable to run program!\n");
            }else{
                k_printf("Program exited with %d \n",rt);
            }
        }
        free(tw);
    }
}