#include "../include/timer.h"
#include "../include/idt.h"
#include "../include/ports.h"
#include "../include/graphics.h"
#include "../include/memory.h"

static int counter = 0;

void sleep(uint64_t time){
    for(uint64_t i = 0 ; i < time ; i++){
        while(counter==0);
    }
}

__attribute__((interrupt)) void timer_interrupt(interrupt_frame* frame){
    counter = 1;
	outportb(0xA0,0x20);
	outportb(0x20,0x20);
    return;
}

void initialise_timer_driver(){
    setInterrupt(0,timer_interrupt);
}