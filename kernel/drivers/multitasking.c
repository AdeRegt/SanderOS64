#include "../include/multitasking.h"
#include "../include/graphics.h"
#include "../include/ports.h"
#include "../include/memory.h"
#include "../include/paging.h"
#include "../include/idt.h"
#include "../include/timer.h"

static int vl = 0;
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
    // tasks[cmt].sessionregs.rip = (upointer_t)task;
    // tasks[cmt].sessionregs.rsp = (upointer_t)requestPage();
    // #ifndef __x86_64
    // tasks[cmt].sessionregs.rax = 0;
    // tasks[cmt].sessionregs.rax = 0x10;
    // tasks[cmt].sessionregs.ds = tasks[cmt].sessionregs.rsp;
    // tasks[cmt].sessionregs.es = tasks[cmt].sessionregs.rsp;
    // tasks[cmt].sessionregs.fs = tasks[cmt].sessionregs.rsp;
    // tasks[cmt].sessionregs.gs = tasks[cmt].sessionregs.rsp;
    // tasks[cmt].sessionregs.ss = tasks[cmt].sessionregs.rsp;
    // #endif
    // tasks[cmt].cr3 = (upointer_t)cr3;
    // tasks[cmt].size = (upointer_t)size;
    // tasks[cmt].files[0].available = 1;
    // tasks[cmt].files[1].available = 1;
    // tasks[cmt].files[2].available = 1;
    // tasks[cmt].files[3].available = 1;
    // tasks[cmt].files[4].available = 1;
    // tasks[cmt].task_running = 1;
    // tasks[cmt].arguments = args;
    memcpy(&tasks[cmt],&tasks[0],sizeof(Task));
    tasks[cmt].sessionregs.rip = (upointer_t)task;
    tasks[cmt].sessionregs.rsp = (upointer_t)requestPage();
    #ifndef __x86_64
    tasks[cmt].sessionregs.ss = tasks[cmt].sessionregs.rsp;
    #endif
    cmt++;
    return cmt - 1;
}

__attribute__((interrupt)) void legacy_timer_handler(interrupt_frame* frame){
    timerfunc();
    interrupt_eoi();
}

void _test_handler(){
    #ifndef __x86_64
    if(tasks[vl].task_running){
        tasks[vl].sessionregs.rip       = _context_eip;
        tasks[vl].sessionregs.rax       =  _context_eax;
        tasks[vl].sessionregs.cs        =  _context_cs;
        tasks[vl].sessionregs.rflags    =  _context_eflags;
        tasks[vl].sessionregs.rdx       =  _context_edx;
        tasks[vl].sessionregs.rcx       =  _context_ecx;
        tasks[vl].sessionregs.rbx       =  _context_ebx;
        tasks[vl].sessionregs.rbp       =  _context_ebp;
        tasks[vl].sessionregs.rdi       =  _context_edi;
        tasks[vl].sessionregs.rsi       =  _context_esi;
        tasks[vl].sessionregs.gs        =  _context_gs;
        tasks[vl].sessionregs.fs        =  _context_fs;
        tasks[vl].sessionregs.es        =  _context_es;
        tasks[vl].sessionregs.ds        =  _context_ds;
    }
    int tf = 0;
    for(int i = vl ; i < MAX_TASKS ; i++){
        if(tasks[i].task_running){
            vl = i;
            tf = 1;
            break;
        }
    }
    if(tf==0){
        vl = 0;
    }
    if(tasks[vl].task_running){
        _context_eip                    = tasks[vl].sessionregs.rip;
        _context_eax                    = tasks[vl].sessionregs.rax;
        _context_cs                     = tasks[vl].sessionregs.cs;
        _context_eflags                 = tasks[vl].sessionregs.rflags;
        _context_edx                    = tasks[vl].sessionregs.rdx;
        _context_ecx                    = tasks[vl].sessionregs.rcx;
        _context_ebx                    = tasks[vl].sessionregs.rbx;
        _context_ebp                    = tasks[vl].sessionregs.rbp;
        _context_edi                    = tasks[vl].sessionregs.rdi;
        _context_esi                    = tasks[vl].sessionregs.rsi;
        _context_gs                     = tasks[vl].sessionregs.gs;
        _context_fs                     = tasks[vl].sessionregs.fs;
        _context_es                     = tasks[vl].sessionregs.es;
        _context_ds                     = tasks[vl].sessionregs.ds;
    }
    #endif 
    timerfunc();
    return;
}

void initialise_multitasking_driver(){
    k_printf("multitasking: enabling multitasking...\n");
    int divisor = 1193180 / 100;       /* Calculate our divisor */
	outportb(0x43, 0x36);             /* Set our command byte 0x36 */
	outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
    memset(tasks,0,sizeof(Task)*MAX_TASKS);
    tasks[0].task_running = 1;
    setInterrupt(0,multitaskingint);
    sleep(5);
}