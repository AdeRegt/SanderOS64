#pragma once
#include <stdint.h>
#include "outint.h"

#define IDT_TA_InterruptGate    0b10001110
#define IDT_TA_CallGate         0b10001100
#define IDT_TA_TrapGate         0b10001111

typedef struct {
    uint16_t offset0; 
    uint16_t selector;  
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t ignore;
}IDTDescEntry;

typedef struct __attribute__((packed)) {
    uint16_t Limit;
    upointer_t Offset;
} IDTR ;

typedef struct{
    upointer_t ip;
    upointer_t cs;
    upointer_t flags;
    upointer_t sp;
    upointer_t ss;
}interrupt_frame;

void initialise_idt_driver();
void setInterrupt(int offset,void *fun);