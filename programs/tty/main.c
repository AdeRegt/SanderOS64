#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SanderOS.h>
#include <string.h>

char wd[FILENAME_MAX];
char pd[FILENAME_MAX];

void tty_inner_loop(){
    while(1){
        printf("%s > ",wd);
        uint8_t *tw = scan_x_characters(50);
        printf("\n");
        if(memcmp(tw,"exit",4)==0){
            break;
        }else if(memcmp(tw,"dir",3)==0){
            char *dirinfo = dir(wd);
            if(dirinfo){
                printf("%s\n",dirinfo);
            }else{
                printf("Unable to read directory!\n");
            }
        }else if(memcmp(tw,"cd ",3)==0){
            char *strpath = (char*) (tw+3);
            if(strpath[1]==':'){
                memset((void*)&wd,0,50);
                memcpy((void*)&wd,strpath,strlen(strpath));
            }else if(strpath[0]=='.'&&strpath[1]=='.'){
                upointer_t imax = strlen(wd);
                for(upointer_t i = 0 ; i < imax ; i++){
                    char thisone = wd[imax-i];
                    if(thisone=='/'){
                        wd[imax-i] = 0;
                        break;
                    }
                    if(thisone==':'){
                        break;
                    }
                    wd[imax-i] = 0;
                }
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
                printf("Unable to run program!\n");
            }else if(rt && rt<1000){
                printf("Program is running in the background\n");
                waitForPid(rt);
            }else{
                printf("Program exited with %d \n",rt);
            }
        }
        free(tw);
    }
}

void tty_help(){
    printf("Commands: \n");
    printf("exit               - Exit the shell\n");
    printf("dir                - Print contents of the working directory\n");
    printf("cd        [path]   - Change working directory\n");
    printf("\n");
}

int main(void){
    cls();

    memset((void*)&wd,0,FILENAME_MAX);
    #ifdef __x86_64
        memcpy((void*)&wd,"A:PROGRAMS/64BIT",strlen("A:PROGRAMS/64BIT"));
    #else
        memcpy((void*)&wd,"A:PROGRAMS/32BIT",strlen("A:PROGRAMS/32BIT"));
    #endif

    char *kp = dir(wd);

    printf("SanderOS64 Buildin Command Interpeter\n");
    if(kp){
        printf("Known programs: \n");
        printf("%s\n",kp);
    }
    tty_help();
    tty_inner_loop();
    exit(0);
}