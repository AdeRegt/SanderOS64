#include "../include/ports.h"

void outportb(uint16_t port, uint8_t value){
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inportb(uint16_t port){
    uint8_t returnVal;
    asm volatile ("inb %1, %0"
    : "=a"(returnVal)
    : "Nd"(port));
    return returnVal;
}

void outportw(uint16_t port, uint16_t value){
    asm volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

uint16_t inportw(uint16_t port){
    uint16_t returnVal;
    asm volatile ("inw %1, %0"
    : "=a"(returnVal)
    : "Nd"(port));
    return returnVal;
}

void outportl(uint16_t port, uint32_t value){
    asm volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

uint32_t inportl(uint16_t port){
    uint32_t returnVal;
    asm volatile ("inl %1, %0"
    : "=a"(returnVal)
    : "Nd"(port));
    return returnVal;
}