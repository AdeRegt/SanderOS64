#include "../include/idt.h" 
#include "../include/graphics.h" 
#include "../include/memory.h" 
#include "../include/ports.h" 

IDTR idtr;

__attribute__((interrupt)) void PageFault_Handler(interrupt_frame* frame){
    k_printf("Interrupt: Page fault detected\n");
	outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

__attribute__((interrupt)) void NakedInterruptHandler(interrupt_frame* frame){
	outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

void interrupt_set_offset(IDTDescEntry* int_PageFault,uint64_t offset){
    int_PageFault->offset0 = (uint16_t)(offset & 0x000000000000ffff);
    int_PageFault->offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    int_PageFault->offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
}

void setInterrupt(int offset,void *fun){
    IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + ((offset+32) * sizeof(IDTDescEntry)));
    interrupt_set_offset(int_PageFault,(uint64_t)fun);
    int_PageFault->type_attr = IDT_TA_TrapGate;
    int_PageFault->selector = 0x08;
}

void setRawInterrupt(int offset,void *fun){
    IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + ((offset) * sizeof(IDTDescEntry)));
    interrupt_set_offset(int_PageFault,(uint64_t)fun);
    int_PageFault->type_attr = IDT_TA_TrapGate;
    int_PageFault->selector = 0x08;
}

void initialise_idt_driver(){
    k_printf("get some info from the old idt...\n");
    asm volatile ("sidt %0" : "=m"(idtr));
    
    outportb(0x20, 0x11);
	outportb(0xA0, 0x11);
	outportb(0x21, 0x20);
	outportb(0xA1, 0x28);
	outportb(0x21, 0x04);
	outportb(0xA1, 0x02);
	outportb(0x21, 0x01);
	outportb(0xA1, 0x01);
	outportb(0x21, 0x0);
	outportb(0xA1, 0x0);

    k_printf("sidt: Limit:%x Offset:%x \n",idtr.Limit,idtr.Offset);
    IDTDescEntry *idtentries = (IDTDescEntry*) idtr.Offset;
    IDTDescEntry pfe = idtentries[0xcd];
    k_printf("ist=%x offset0=%x offset1=%x offset2=%x selector=%x type_attr=%x \n",pfe.ist,pfe.offset0,pfe.offset1,pfe.offset2,pfe.selector,pfe.type_attr);
    for(uint16_t i = 0 ; i < idtr.Limit ; i++){
        setRawInterrupt(i,NakedInterruptHandler);
    }
    setRawInterrupt(0xCD,PageFault_Handler);
    k_printf("ist=%x offset0=%x offset1=%x offset2=%x selector=%x type_attr=%x \n",pfe.ist,pfe.offset0,pfe.offset1,pfe.offset2,pfe.selector,pfe.type_attr);
	asm volatile("sti");
}