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

#ifndef __x86_64
    uint32_t _context_eip;
    uint32_t _context_eax;
    uint32_t _context_cs;
    uint32_t _context_eflags;
    uint32_t _context_edx;
    uint32_t _context_ecx;
    uint32_t _context_ebx;
    uint32_t _context_ebp;
    uint32_t _context_edi;
    uint32_t _context_esi;
    uint32_t _context_gs;
    uint32_t _context_fs;
    uint32_t _context_es;
    uint32_t _context_ds;
#endif 

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

__attribute__((interrupt)) void legacy_timer_handler(interrupt_frame* frame){
    timerfunc();
    interrupt_eoi();
}

void _test_handler(){
    timerfunc();
    return;
}

void initialise_multitasking_driver(){
    k_printf("multitasking: enabling multitasking...\n");
    int divisor = 1193180 / 100;       /* Calculate our divisor */
	outportb(0x43, 0x36);             /* Set our command byte 0x36 */
	outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
    setInterrupt(0,multitaskingint);
}