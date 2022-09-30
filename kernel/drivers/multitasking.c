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

void waitForPid(int pid){
    volatile Task* ts = (volatile Task*) (tasks + (sizeof(volatile Task)*pid));
    ts->task_running=1;
    while(1){
        ts = (volatile Task*) (tasks + (sizeof(volatile Task)*pid));
        if(ts->task_running==0){
            break;
        }
    }
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
    #ifdef __x86_64
    upointer_t cr0, cr2, cr3;
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
    #endif 
    #ifndef __x86_64
    upointer_t cr0, cr2, cr3;
    __asm__ __volatile__ (
        "mov %%cr0, %%eax\n\t"
        "mov %%eax, %0\n\t"
        "mov %%cr2, %%eax\n\t"
        "mov %%eax, %1\n\t"
        "mov %%cr3, %%eax\n\t"
        "mov %%eax, %2\n\t"
    : "=m" (cr0), "=m" (cr2), "=m" (cr3)
    : /* no input */
    : "%eax"
    );
    #endif 
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

int addTask(void *task,void *cr3,upointer_t size,char** args){
    // fill the registry
    tasks[cmt].sessionregs.rip = (upointer_t)new_program_starter;
    tasks[cmt].sessionregs.rsp = (upointer_t)requestPage();
    // tasks[cmt].sessionregs.ss = (upointer_t)requestPage();
    tasks[cmt].cr3 = (upointer_t)cr3;
    tasks[cmt].size = (upointer_t)size;
    tasks[cmt].files[0].available = 1;
    tasks[cmt].files[1].available = 1;
    tasks[cmt].files[2].available = 1;
    tasks[cmt].files[3].available = 1;
    tasks[cmt].files[4].available = 1;
    tasks[cmt].task_running = 1;
    tasks[cmt].arguments = args;
    cmt++;
    return cmt - 1;
}

void initialise_multitasking_driver(){
    #ifdef __x86_64
    if(inportb(0xE9)!=0xE9){
        k_printf("multitasking: enabling multitasking...\n");
        setInterrupt(0,multitaskingint);
        while(vl==0xF);
    }else{
        k_printf("multitasking: unable to accept multitasking on bochs platform!\n");
    }
    #endif
}