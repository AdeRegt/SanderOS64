#include "../include/idt.h" 
#include "../include/graphics.h" 
#include "../include/memory.h" 

IDTR idtr;

struct interrupt_frame {
    uint16_t ip;
    uint16_t cs;
    uint16_t flags;
    uint16_t sp;
    uint16_t ss;
} __attribute__((packed));

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
    asm ("cli");
    k_printf("ACK");
    asm ("hlt");
    k_printf("Page fault detected");
    while(1);
}

void interrupt_set_offset(IDTDescEntry* int_PageFault,uint64_t offset){
    int_PageFault->offset0 = (uint16_t)(offset & 0x000000000000ffff);
    int_PageFault->offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    int_PageFault->offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
}

void setInterrupt(int offset,void *fun){
    IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + offset * sizeof(IDTDescEntry));
    interrupt_set_offset(int_PageFault,(uint64_t)fun);
    int_PageFault->type_attr = IDT_TA_TrapGate;
    int_PageFault->selector = 0x08;
}

void initialise_idt_driver(){
    idtr.Limit = 0x0FFF;
    idtr.Offset = (uint64_t)requestPage();
    setInterrupt(0x0E,PageFault_Handler);
    // for(unsigned short i = 0 ; i < idtr.Limit ; i++){
    //     setInterrupt(i,PageFault_Handler);
    // }
    asm ("lidt %0" : : "m" (idtr));
}