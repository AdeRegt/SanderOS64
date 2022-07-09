#include "../include/multitasking.h"
#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/memory.h"
#include "../include/idt.h"
#include "../include/timer.h"

static int vl = 0xF;
static int cmt = 1;
static Task tasks[MAX_TASKS];

Task* getTasks(){
    return (Task*) &tasks;
}

void multitaskinghandler(stack_registers *ix){
    vl = 0;
    goto finishup;
    if(vl==0xf){
        for(int i = 0 ; i < MAX_TASKS ; i++){
            memcpy(&tasks[0].sessionregs,ix,sizeof(stack_registers));
        }
        vl = 0;
        k_printf("Finished first multitasking instance!\n");
        goto finishup;
    }else{
        memcpy(&tasks[vl].sessionregs,ix,sizeof(stack_registers));
        vl++;
        if(vl==cmt){
            vl=0;
        }
    }
    // k_printf("Switch to pid %d %d | ",vl,cmt);
    memcpy(ix,&tasks[vl].sessionregs,sizeof(stack_registers));
    finishup:
    timerfunc();
    outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

void addTask(void *task,void *cr3){
    tasks[cmt].sessionregs.rip = (uint64_t)task;
    cmt++;
}

void initialise_multitasking_driver(){
    setInterrupt(0,multitaskingint);
    while(vl==0xF);
}