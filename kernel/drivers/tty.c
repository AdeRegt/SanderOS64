#include "../include/tty.h"
#include "../include/graphics.h"
#include "../include/memory.h"
#include "../include/device.h"
#include "../include/exec/program.h"
#include "../include/multitasking.h"

char wd[FILENAME_MAX];
char pd[FILENAME_MAX];

void initialise_tty(){
    clear_screen(create_colour_code(0xFF,0xFF,0xFF,0xFF));

    memset((void*)&wd,0,FILENAME_MAX);
    #ifdef __x86_64
        memcpy((void*)&wd,"A:PROGRAMS/64BIT",strlen("A:PROGRAMS/64BIT"));
    #else
        memcpy((void*)&wd,"A:PROGRAMS/32BIT",strlen("A:PROGRAMS/32BIT"));
    #endif

    char *kp = dir(wd);

    k_printf("SanderOS64 Buildin Command Interpeter\n");
    if(kp){
        k_printf("Known programs: \n");
        k_printf("%s\n",kp);
    }
    k_printf("Commands: \n");
    k_printf("exit               - Exit the shell\n");
    k_printf("dir                - Print contents of the working directory\n");
    k_printf("cd        [path]   - Change working directory\n");
    k_printf("\n");
    while(1){
        k_printf("%s > ",wd);
        uint8_t *tw = scanLine(50);
        k_printf("\n");
        if(memcmp(tw,"exit",4)==0){
            break;
        }else if(memcmp(tw,"dir",3)==0){
            char *dirinfo = dir(wd);
            if(dirinfo){
                k_printf("%s\n",dirinfo);
            }else{
                k_printf("Unable to read directory!\n");
            }
        }else if(memcmp(tw,"cd ",3)==0){
            char *strpath = (char*) (tw+3);
            if(strpath[1]==':'){
                memset((void*)&wd,0,50);
                memcpy((void*)&wd,strpath,strlen(strpath));
            }else{
                if(strlen((char*)&wd)>3){
                    memcpy((void*)(((upointer_t)&wd)+strlen((char*)&wd) + -1 ),"/",1);
                }
                memcpy((void*)(((upointer_t)&wd)+strlen((char*)&wd) + -1 ),strpath,strlen(strpath));
            }
        }else{
            memset((void*)&pd,0,50);
            if(tw[1]==':'){
                memcpy((void*)&pd,tw,strlen(tw));
            }else{
                int m1 = -1;
                memcpy((void*)&pd,(char*)&wd,strlen((char*)&wd));
                if(strlen((char*)&pd)>2){
                    memcpy((void*)(((upointer_t)&pd) + strlen((char*)&wd) -1  ),"/",1);
                    m1 = 0;
                }
                memcpy((void*)(((upointer_t)&pd) + strlen((char*)&wd) + m1  ),tw,strlen(tw));
            }
            char* pd2 = 0;
            for(int z = 0 ; z < strlen(pd) ; z++){
                if(pd[z]==' '){
                    pd2 = (char*)(pd+z+1);
                    pd[z] = 0;
                    break;
                }
            }
            int rt = exec(pd,pd2);
            if(rt==-1){
                k_printf("Unable to run program!\n");
            }else if(rt && rt<1000){
                k_printf("Program is running in the background\n");
                waitForPid(rt);
            }else{
                k_printf("Program exited with %d \n",rt);
            }
        }
        free(tw);
    }
}