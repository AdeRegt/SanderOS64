#pragma once
#include <stdint.h>
#include "outint.h"

#define IDT_TA_InterruptGate    0b10001110
#define IDT_TA_CallGate         0b10001100
#define IDT_TA_TrapGate         0b10001111

#ifdef __x86_64
typedef struct {
    uint16_t offset0; 
    uint16_t selector;  
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset1;
    uint32_t offset2;
    uint32_t ignore;
}IDTDescEntry;
#else 
typedef struct{
    uint16_t base_lo;
    uint16_t sel;        /* Our kernel segment goes here! */
    uint8_t always0;     /* This will ALWAYS be set to 0! */
    uint8_t flags;       /* Set using the above table! */
    uint16_t base_hi;
}__attribute__((packed)) IDTDescEntry;
#endif 

typedef struct __attribute__((packed)) {
    uint16_t Limit;
    upointer_t Offset;
} IDTR;

typedef struct{
    upointer_t ip;
    upointer_t cs;
    upointer_t flags;
    upointer_t sp;
    upointer_t ss;
}interrupt_frame;

void initialise_idt_driver();
void setInterrupt(int offset,void *fun);