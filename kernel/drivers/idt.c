#include "../include/idt.h" 
#include "../include/graphics.h" 
#include "../include/memory.h" 
#include "../include/ports.h" 
#include "../include/multitasking.h"
#include "../include/ps2.h"
#include "../include/device.h"
#include "../include/paging.h"

IDTR idtr;

__attribute__((interrupt)) void PageFault_Handler(interrupt_frame* frame){
    k_printf("Interrupt: Page fault detected\n");for(;;);
	outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

__attribute__((interrupt)) void GeneralFault_Handler(interrupt_frame* frame){
    k_printf("\nInterrupt: error cs:%x flags:%x ip:%x sp:%x ss:%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
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

extern void isrint();
extern void isr2int();

void end_of_program(){
    k_printf("Program with pid %d ended!\n",getPid());
    for(;;);
}

typedef struct{
  long tv_sec;		/* Seconds.  */
  long tv_usec;	/* Microseconds.  */
}__attribute__((packed)) timeval;

// http://faculty.nps.edu/cseagle/assembly/sys_call.html
void isrhandler(stack_registers *ix){
    outportb(0xA0,0x20);
	outportb(0x20,0x20);
    if(ix->rax==1){
        k_printf("program returned with %d from %x \n",ix->rbx,ix->rip);
        ix->rip = (uint64_t)end_of_program;
    }else if(ix->rax==2){
        k_printf("isr: fork not implemented!\n");
    }else if(ix->rax==3){
        if(ix->rbx){
            char* u = (char*) ix->rcx;
            u[0] = '!';
        }
    }else if(ix->rax==4){
        // k_printf("rbx: %d | rcx: %d | rdx: %d \n",ix->rbx,ix->rcx,ix->rdx);
        char* z = ((char*) (ix->rcx));
        for(uint64_t i = 0 ; i < ix->rdx ; i++){
            k_printf("%c",z[i]);
        }
    }else{
        k_printf("isr: Unknown isr code!\n");for(;;);
    }
}

void isr2handler(stack_registers *ix){
    if(ix->rax==0){
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rdi]);
        memcpy((void*)ix->rsi,(void*)(fl->buffer + fl->pointer),ix->rdx);
        fl->pointer = fl->pointer + ix->rdx;
    }else if(ix->rax==1){
        char* z = ((char*) (ix->rsi));
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rdi]);
        if(ix->rdi<2){
            for(uint64_t i = 0 ; i < ix->rdx ; i++){
                k_printf("%c",z[i]);
            }
        }else{
            uint64_t oldsize = getFileSize(fl->filename);
            uint64_t newsize = oldsize + ix->rdx;
            void *tmpbuf = requestPage();
            readFile(fl->filename,tmpbuf);
            memcpy((void*)(tmpbuf + oldsize),z,ix->rdx);
            uint64_t q = writeFile(fl->filename,tmpbuf,newsize);
            if(q==0){
                ix->rax = -1;
            }else{
                ix->rax = q;
            }
            freePage(tmpbuf);
        }
    }else if(ix->rax==2){
        // fileopen option!
        char* path = (char*) ix->rdi;
        uint64_t filesize = 0;
        void* buffer;
        if(ix->rsi==0){
            filesize = getFileSize(path);
            if(filesize==0){
                ix->rax = -1;
                goto eot;
            }
            // filesize / PAGE_SIZE;
            buffer = requestPage();
            readFile(path,buffer);
        }
        int sov = 0;
        for(int i = 0 ; i < 10 ; i++){
            File *fl = (File*) &(getCurrentTaskInfo()->files[i]);
            if(fl->available==0){
                fl->available = 1;
                fl->buffer = buffer;
                fl->filesize = filesize;
                fl->pointer = 0;
                fl->filename = path;
                fl->flags = ix->rsi;
                sov = i;
                break;
            }
        }
        ix->rax = sov;
    }else if(ix->rax==3){
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rdi]);
        fl->available = 0;
        ix->rax = 0;
    }else if(ix->rax==8){
        // seek option!
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rdi]);
        if(ix->rdx==2){
            fl->pointer = fl->filesize;
            ix->rax = fl->pointer;
        }else{
            fl->pointer = ix->rsi;
            ix->rax = fl->pointer;
        }
    }else if(ix->rax==12){
        // we always accept extending memory whatoever
        ix->rax = ix->rdi;
    }else if(ix->rax==60){
        ix->rip = (uint64_t)end_of_program;
    }else if(ix->rax==96){
        timeval* tv = (timeval*) ix->rdi;
        tv->tv_sec = 10000;
        tv->tv_usec = 10000;
        ix->rax = 0;
    }else if(ix->rax==400){
        ix->rax = (uint64_t) malloc(ix->rdx);
    }else{
        k_printf("\n\n------------------------\n"); 
        k_printf("interrupt: isr2: RAX=%x RIP=%x \n",ix->rax,ix->rip);
        k_printf("isr2: Unknown isr code!\n");
        k_printf("------------------------\n\n"); 
        for(;;);
    }
    eot:
    outportb(0xA0,0x20);
	outportb(0x20,0x20);
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
    for(uint16_t i = 0 ; i < 32 ; i++){
        setRawInterrupt(i,GeneralFault_Handler);
    }
    setRawInterrupt(0xCD,PageFault_Handler);
    setRawInterrupt(0x80,isrint);
    setRawInterrupt(0x81,isr2int);
    k_printf("ist=%x offset0=%x offset1=%x offset2=%x selector=%x type_attr=%x \n",pfe.ist,pfe.offset0,pfe.offset1,pfe.offset2,pfe.selector,pfe.type_attr);
	asm volatile("sti");
}