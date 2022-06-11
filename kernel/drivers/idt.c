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
    k_printf("Page fault detected");
    asm volatile("hlt");
}

void interrupt_set_offset(IDTDescEntry* int_PageFault,uint64_t offset){
    int_PageFault->offset0 = (uint16_t)(offset & 0x000000000000ffff);
    int_PageFault->offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    int_PageFault->offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
}

void setInterrupt(int offset,void *fun){
    IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + offset * sizeof(IDTDescEntry));
    interrupt_set_offset(int_PageFault,(uint64_t)fun);
    int_PageFault->type_attr = IDT_TA_InterruptGate;
    int_PageFault->selector = 0x08;
}

void initialise_idt_driver(){
    k_printf("get some info from the old idt...\n");
    asm volatile ("sidt %0" : "=m"(idtr));
    k_printf("sidt: Limit:%x Offset:%x \n",idtr.Limit,idtr.Offset);
    IDTDescEntry *idtentries = (IDTDescEntry*) idtr.Offset;
    IDTDescEntry pfe = idtentries[0x0e];
    k_printf("ist=%x offset0=%x offset1=%x offset2=%x selector=%x type_attr=%x \n",pfe.ist,pfe.offset0,pfe.offset1,pfe.offset2,pfe.selector,pfe.type_attr);
    interrupt_set_offset(&pfe,(uint64_t)PageFault_Handler);
    k_printf("ist=%x offset0=%x offset1=%x offset2=%x selector=%x type_attr=%x \n",pfe.ist,pfe.offset0,pfe.offset1,pfe.offset2,pfe.selector,pfe.type_attr);
    
}