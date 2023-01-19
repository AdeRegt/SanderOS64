#include "../include/timer.h"
#include "../include/idt.h"
#include "../include/ports.h"
#include "../include/graphics.h"
#include "../include/memory.h"
#include "../include/multitasking.h"

static int counter = 0;

void sleep(uint64_t time){
    for(uint64_t i = 0 ; i < time ; i++){
        counter = 0;
        while(counter==0);
    }
}

void timerfunc(){
    counter = 1;
}