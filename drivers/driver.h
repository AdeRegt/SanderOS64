#pragma once 
#include <stdint.h>

typedef struct{
    uint64_t ip;
    uint64_t cs;
    uint64_t flags;
    uint64_t sp;
    uint64_t ss;
}interrupt_frame;

typedef struct{
    int bus;
    int slot;
    int function;
}__attribute__((packed)) PCIInfo;

void outportb(uint16_t port, uint8_t value);
uint8_t inportb(uint16_t port);

void k_printf(char* format,...);

void setInterrupt(int offset,void *fun);
void *requestPage();
void memset(void *start, unsigned char value, unsigned long long num);

unsigned long getBARaddress(int bus,int slot,int function,int barNO);
void sleep(uint64_t time);
void registerHIDDevice(void *getcpointer);