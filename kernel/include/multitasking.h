#pragma once
#include <stdint.h>
#include "outint.h"

typedef struct{
    // Pushed by isr_common_stub
    #ifdef __x86_64
    upointer_t r15;
    upointer_t r14;
    upointer_t r13;
    upointer_t r12;
    upointer_t r11;
    upointer_t r10;
    upointer_t r9;
    upointer_t r8;
    #else 
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    #endif
    upointer_t rdi;
    upointer_t rsi;
    upointer_t rbp;
    upointer_t rdx;
    upointer_t rcx;
    upointer_t rbx;
    upointer_t rax;

    // Registers pushed by IRETQ
    upointer_t rip;
    upointer_t cs;
    upointer_t rflags;
    upointer_t rsp;
    upointer_t ss;
}__attribute__((packed)) stack_registers;

typedef struct{
    uint8_t available;
    upointer_t filesize;
    void* buffer;
    upointer_t pointer;
    uint8_t* filename;
    upointer_t flags;
}__attribute__((packed)) File;

typedef struct{
    stack_registers sessionregs;
    volatile int innercounter;
    upointer_t cr3;
    upointer_t size;
    File files[10];
    uint8_t task_running;
    char** arguments;
    int window_id;
}__attribute__((packed)) Task;

#define MAX_TASKS 10

void initialise_multitasking_driver();
extern void multitaskingint();
Task* getTasks();
int addTask(void *task,void *cr3,upointer_t size,char** args);
int getPid();
Task* getCurrentTaskInfo();
void waitForPid(int pid);