#pragma once
#include <stdint.h>

typedef struct{
    // Pushed by isr_common_stub
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    // Registers pushed by IRETQ
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
}__attribute__((packed)) stack_registers;

typedef struct{
    uint8_t available;
    uint64_t filesize;
    void* buffer;
    uint64_t pointer;
}__attribute__((packed)) File;

typedef struct{
    stack_registers sessionregs;
    volatile int innercounter;
    uint64_t cr3;
    uint64_t size;
    File files[10];
}__attribute__((packed)) Task;

#define MAX_TASKS 10

void initialise_multitasking_driver();
extern void multitaskingint();
Task* getTasks();
int addTask(void *task,void *cr3,uint64_t size);
int getPid();
Task* getCurrentTaskInfo();