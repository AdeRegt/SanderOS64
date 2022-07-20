#include "../include/multitasking.h"
#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/memory.h"
#include "../include/paging.h"
#include "../include/idt.h"
#include "../include/timer.h"

static int vl = 0xF;
static int cmt = 1;
static Task tasks[MAX_TASKS];

extern void _KernelStart();
extern void _KernelEnd();

Task* getTasks(){
    return (Task*) &tasks;
}

int getPid(){
    return vl;
}

Task* getCurrentTaskInfo(){
    return (Task*)&tasks[vl];
}

void new_program_starter(){
    k_printf("About to start program with pid %d \n",getPid());
    void (*pstart)() = ((__attribute__((sysv_abi)) void (*)() ) EXTERNAL_PROGRAM_ADDRESS);
    pstart();
    for(;;);
}

void multitaskinghandler(stack_registers *ix){
    // vl = 0;
    // goto finishup;
    uint64_t cr0, cr2, cr3;
    __asm__ __volatile__ (
        "mov %%cr0, %%rax\n\t"
        "mov %%eax, %0\n\t"
        "mov %%cr2, %%rax\n\t"
        "mov %%eax, %1\n\t"
        "mov %%cr3, %%rax\n\t"
        "mov %%eax, %2\n\t"
    : "=m" (cr0), "=m" (cr2), "=m" (cr3)
    : /* no input */
    : "%rax"
    );
    if(vl==0xf){
        vl = 0;
        for(int i = 0 ; i < MAX_TASKS ; i++){
            memcpy(&tasks[i].sessionregs,ix,sizeof(stack_registers));
        }
        k_printf("Finished first multitasking instance!\n");
    }else{
        memcpy(&tasks[vl].sessionregs,ix,sizeof(stack_registers));
        vl++;
        if(vl==cmt){
            vl=0;
        }
    }
    // k_printf("Switch to pid %d %d | ",vl,cmt);
    memcpy(ix,&tasks[vl].sessionregs,sizeof(stack_registers));
    if(tasks[vl].cr3){
        map_memory((void*) cr3,(void*) EXTERNAL_PROGRAM_ADDRESS ,(void*) tasks[vl].cr3 );
    }
    // k_printf("Switch to pid %d !\n",vl);
    timerfunc();
    outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

int addTask(void *task,void *cr3,uint64_t size){
    // fill the registry
    tasks[cmt].sessionregs.rip = (uint64_t)new_program_starter;
    tasks[cmt].sessionregs.rsp = (uint64_t)requestPage();
    // tasks[cmt].sessionregs.ss = (uint64_t)requestPage();
    tasks[cmt].cr3 = (uint64_t)cr3;
    tasks[cmt].size = (uint64_t)size;
    tasks[cmt].files[0].available = 1;
    tasks[cmt].files[1].available = 1;
    tasks[cmt].files[2].available = 1;
    tasks[cmt].files[3].available = 1;
    tasks[cmt].files[4].available = 1;
    cmt++;
    return cmt - 1;
}

void initialise_multitasking_driver(){
    setInterrupt(0,multitaskingint);
    while(vl==0xF);
}