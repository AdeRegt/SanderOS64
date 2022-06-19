#include "idt.h"

void initialise_timer_driver();
void sleep(uint64_t time);

typedef struct{
    interrupt_frame frame;
    uint64_t counter;
    uint64_t count_untill;
}__attribute__((packed)) Task;