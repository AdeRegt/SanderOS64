#include "../include/multitasking.h"
#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/memory.h"
#include "../include/idt.h"
#include "../include/timer.h"

static int vl = 0xF;
static Task tasks[2];

Task* getTasks(){
    return (Task*) &tasks;
}

void multitaskinghandler(stack_registers *ix){
    if(vl==0xf){
        memcpy(&tasks[0].sessionregs,ix,sizeof(stack_registers));
        memcpy(&tasks[1].sessionregs,ix,sizeof(stack_registers));
        vl = 1;
    }else{
        memcpy(&tasks[vl].sessionregs,ix,sizeof(stack_registers));
        if(vl==0){
            vl=1;
        }else if(vl==1){
            vl=0;
        }
    }
    memcpy(ix,&tasks[vl].sessionregs,sizeof(stack_registers));
    timerfunc();
    outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

void setTask(void *task){
    tasks[1].sessionregs.rip = (uint64_t)task;
}

void ttt(){
    k_printf("AAA");
    for(int i = 0 ; i<5; i++){
        sleep(1);
        k_printf("%x ",tasks[1].innercounter);
    }
    for(;;);
}

void initialise_multitasking_driver(){
    setInterrupt(0,multitaskingint);
    while(vl==0xF);
    setTask(ttt);
}