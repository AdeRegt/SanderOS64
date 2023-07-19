#include "../include/idt.h" 
#include "../include/graphics.h" 
#include "../include/memory.h" 
#include "../include/ports.h" 
#include "../include/multitasking.h"
#include "../include/device.h"
#include "../include/paging.h"
#include "../include/timer.h"
#include "../include/ethernet.h"
#include "../include/tty.h"

static IDTR idtr;
uint8_t idtoffsetcode = 0;
__attribute__ ((aligned(0x10))) static IDTDescEntry idt[256];
extern void* isr_stub_table[];



#ifndef __x86_64
    uint32_t _inti_eip;
    uint32_t _inti_eax;
    uint32_t _inti_cs;
    uint32_t _inti_eflags;
    uint32_t _inti_edx;
    uint32_t _inti_ecx;
    uint32_t _inti_ebx;
    uint32_t _inti_ebp;
    uint32_t _inti_edi;
    uint32_t _inti_esi;
    uint32_t _inti_gs;
    uint32_t _inti_fs;
    uint32_t _inti_es;
    uint32_t _inti_ds;

    stack_registers uv;
#endif 

void interrupt_eoi(){
    outportb(0xA0,0x20);
	outportb(0x20,0x20);
}

#ifndef __x86_64
extern void isrpanic();
void GeneralFault_Handler(){
    clear_screen(0xFF000D);
    k_printf("===================================================\n");
    k_printf("             K E R N E L   P A N I C \n");
    k_printf("===================================================\n");
    k_printf("EIP        : %x \n",_inti_eip);
    k_printf("EAX        : %x \n",_inti_eax);
    k_printf("CS         : %x \n",_inti_cs);
    k_printf("EFLAGS     : %x \n",_inti_eflags);
    k_printf("EDX        : %x \n",_inti_edx);
    k_printf("ECX        : %x \n",_inti_ecx);
    k_printf("EBX        : %x \n",_inti_ebx);
    k_printf("EBP        : %x \n",_inti_ebp);
    k_printf("EDI        : %x \n",_inti_edi);
    k_printf("ESI        : %x \n",_inti_esi);
    k_printf("GS         : %x \n",_inti_gs);
    k_printf("FS         : %x \n",_inti_fs);
    k_printf("ES         : %x \n",_inti_es);
    k_printf("DS         : %x \n",_inti_ds);
    struct stackframe *stk = (struct stackframe *)_inti_ebp;
    k_printf("\n\nStack trace:\n");
    for(unsigned int frame = 0; stk && frame < 10; ++frame){
        // Unwind to previous stack frame
        k_printf("%d          :%x\n", frame, stk->eip);
        stk = stk->ebp;
    }
#else 
__attribute__((interrupt)) void GeneralFault_Handler(interrupt_frame* frame){
    k_printf("\nInterrupt: error cs:%x flags:%x ip:%x sp:%x ss:%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
#endif
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void NakedInterruptHandler(interrupt_frame* frame){
	interrupt_eoi();
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void DefaultInterruptHandler(interrupt_frame* frame){
	interrupt_eoi();
}

#ifdef __x86_64
    void interrupt_set_offset(IDTDescEntry* int_PageFault,upointer_t offset){
        int_PageFault->offset0 = (uint16_t)(offset & 0x000000000000ffff);
        int_PageFault->offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
        int_PageFault->offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
    }
#else 
    void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags){
        IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + ((num) * sizeof(IDTDescEntry)));
        /* The interrupt routine's base address */
        int_PageFault->base_lo = (base & 0xFFFF);
        int_PageFault->base_hi = (base >> 16) & 0xFFFF;

        /* The segment or 'selector' that this IDT entry will use
        *  is set here, along with any access flags */
        int_PageFault->sel = sel;
        int_PageFault->always0 = 0;
        int_PageFault->flags = flags;
    }
#endif 

void IRQ_set_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inportb(port) | (1 << IRQline);
    outportb(port, value);        
}
 
void IRQ_clear_mask(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inportb(port) & ~(1 << IRQline);
    outportb(port, value);        
}

void setInterrupt(int offset,void *fun){
    #ifdef __x86_64
        IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + ((offset+idtoffsetcode) * sizeof(IDTDescEntry)));
        interrupt_set_offset(int_PageFault,(upointer_t)fun);
        int_PageFault->type_attr = IDT_TA_InterruptGate;
        int_PageFault->selector = 0x08;
    #else 
        IRQ_clear_mask(32 + offset);
        idt_set_gate(32 + (unsigned char)offset, (unsigned long)fun, 0x08, 0x8E);
    #endif 
}

void setRawInterrupt(int offset,void *fun){
    #ifdef __x86_64
        IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + ((offset) * sizeof(IDTDescEntry)));
        interrupt_set_offset(int_PageFault,(upointer_t)fun);
        int_PageFault->type_attr = IDT_TA_TrapGate;
        int_PageFault->selector = 0x08;
    #else
        idt_set_gate((unsigned char)offset, (unsigned long)fun, 0x08, 0x8E);
    #endif
}

extern void isrint();
extern void isr2int();

void end_of_program(){
    k_printf("Program with pid %d ended!\n\n",getPid());
    tty_inner_loop();
    // for(;;);
}

typedef struct{
  long tv_sec;		/* Seconds.  */
  long tv_usec;	/* Microseconds.  */
}__attribute__((packed)) timeval;

// http://faculty.nps.edu/cseagle/assembly/sys_call.html
#ifndef __x86_64
void isrhandler(){
    stack_registers *ix = (stack_registers*)&uv;
    ix->cs      = _inti_cs;
    ix->rax     = _inti_eax;
    ix->rbp     = _inti_ebp;
    ix->rbx     = _inti_ebx;
    ix->rcx     = _inti_ecx;
    ix->rdi     = _inti_edi;
    ix->rdx     = _inti_edx;
    ix->rflags  = _inti_eflags;
    ix->rip     = _inti_eip;
    ix->rsi     = _inti_esi;
#else 
void isrhandler(stack_registers *ix){
#endif 
    // k_printf("isr: yield eax=%x ebx=%x ecx=%x edx=%x \n",ix->rax,ix->rbx,ix->rcx,ix->rdx);
    outportb(0xA0,0x20);
	outportb(0x20,0x20);
    if(ix->rax==1){
        k_printf("program returned with %d from %x \n",ix->rbx,ix->rip);
        ix->rip = (upointer_t)end_of_program;
    }else if(ix->rax==2){
        k_printf("isr: fork not implemented!\n");
    }else if(ix->rax==3){
        if(ix->rbx){
            char* u = (char*) ix->rcx;
            u[0] = getch(1);
        }
    }else if(ix->rax==4){
        // k_printf("rbx: %d | rcx: %d | rdx: %d \n",ix->rbx,ix->rcx,ix->rdx);
        char* z = ((char*) (ix->rcx));
        for(upointer_t i = 0 ; i < ix->rdx ; i++){
            k_printf("%c",z[i]);
        }
    }else{
        k_printf("isr: Unknown isr code: %d !\n",ix->rax);for(;;);
    }
    #ifndef __x86_64
    _inti_cs = ix->cs;
    _inti_eax = ix->rax;
    _inti_ebp = ix->rbp;
    _inti_ebx = ix->rbx;
    _inti_ecx = ix->rcx;
    _inti_edi = ix->rdi;
    _inti_edx = ix->rdx;
    _inti_eflags = ix->rflags;
    _inti_eip = ix->rip;
    _inti_esi = ix->rsi;
    #endif
}

#ifndef __x86_64
void isr2handler(){
    stack_registers *ix = (stack_registers*)&uv;
    ix->cs      = _inti_cs;
    ix->rax     = _inti_eax;
    ix->rbp     = _inti_ebp;
    ix->rbx     = _inti_ebx;
    ix->rcx     = _inti_ecx;
    ix->rdi     = _inti_edi;
    ix->rdx     = _inti_edx;
    ix->rflags  = _inti_eflags;
    ix->rip     = _inti_eip;
    ix->rsi     = _inti_esi;
#else 
void isr2handler(stack_registers *ix){
#endif 
    // k_printf("isr2: yield eax=%x ebx=%x ecx=%x edx=%x \n",ix->rax,ix->rbx,ix->rcx,ix->rdx);
    if(ix->rax==0){
        // k_printf("isr2:request read\n");
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rdi]);
        memcpy((void*)ix->rsi,(void*)(fl->buffer + fl->pointer),ix->rdx);
        fl->pointer = fl->pointer + ix->rdx;
    }else if(ix->rax==1){
        char* z = ((char*) (ix->rsi));
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rdi]);
        if(ix->rdi<5){
            for(upointer_t i = 0 ; i < ix->rdx ; i++){
                k_printf("%c",z[i]);
            }
        }else{
            // k_printf("isr2:request write\n");
            upointer_t oldsize = getFileSize(fl->filename);
            upointer_t newsize = oldsize + ix->rdx;
            void *tmpbuf = requestPage();
            readFile(fl->filename,tmpbuf);
            memcpy((void*)(tmpbuf + oldsize),z,ix->rdx);
            upointer_t q = writeFile(fl->filename,tmpbuf,newsize);
            if(q==0){
                ix->rax = -1;
            }else{
                ix->rax = q;
            }
            freePage(tmpbuf);
        }
    }else if(ix->rax==2){
        // k_printf("isr2:request open rsi=%x \n",ix->rsi);
        // fileopen option!
        char* path = (char*) ix->rdi;
        upointer_t filesize = 0;
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
        // k_printf("isr2:request close\n");
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rdi]);
        fl->available = 0;
        ix->rax = 0;
    }else if(ix->rax==8){
        // k_printf("isr2:request seek rdi:%x rdx:%x \n",ix->rdi,ix->rdx);
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
        // k_printf("isr2:request space\n");
        // we always accept extending memory whatoever
        ix->rax = ix->rdi;
    }else if(ix->rax==60){
        // k_printf("isr2:and\n");
        Task* ts = (Task*) (getTasks() + (sizeof(Task)*getPid()));
        ts->task_running = 0;
        ix->rip = (upointer_t)end_of_program;
    }else if(ix->rax==96){
        // k_printf("isr2:request time\n");
        timeval* tv = (timeval*) ix->rdi;
        tv->tv_sec = 10000;
        tv->tv_usec = 10000;
        ix->rax = 0;
    }else if(ix->rax==400){
        ix->rax = (upointer_t) requestPage();
    }else if(ix->rax==401){
        if(ix->rcx>0&&ix->rcx<10){
            char* us = (char*) getCurrentTaskInfo()->arguments[ix->rcx-1];
            ((uint32_t*)ix->rdi)[0] = (uint32_t)((upointer_t)us);
        }else{
            ((uint32_t*)ix->rdi)[0] = 0;
        }
    }else if(ix->rax==402){
        // k_printf("isr2: rbx:%x rcx:%x rdx:%x \n",ix->rbx,ix->rcx,ix->rdx);
        draw_pixel_at(ix->rbx,ix->rcx,ix->rdx);
    }else if(ix->rax==403){
        char* u = (char*) ix->rbx;
        u[0] = 0;
        u[0] = getch(0);
    }else if(ix->rax==404){
        char* u = (char*) ix->rbx;
        u[0] = 0;
        u[0] = getch(1);
    }else if(ix->rax==405){
        uint8_t *name = (uint8_t*) ix->rbx;
        ix->rax = (upointer_t)getIPFromName(name);
    }else if(ix->rax==406){
        uint8_t *name = (uint8_t*) ix->rbx;
        ix->rax = (upointer_t)getMACFromIp(name);
    }else if(ix->rax==407){
        create_tcp_session(getOurIpAsLong(),((unsigned long*)ix->rbx)[0],ix->rcx,ix->rcx,ix->rdx);
    }else if(ix->rax==408){
        sleep(ix->rbx);
    }else{
        k_printf("\n\n------------------------\n"); 
        k_printf("interrupt: isr2: RAX=%x RIP=%x \n",ix->rax,ix->rip);
        k_printf("isr2: Unknown isr code!\n");
        k_printf("------------------------\n\n"); 
        for(;;);
    }
    eot:
    interrupt_eoi();
    #ifndef __x86_64
    _inti_cs = ix->cs;
    _inti_eax = ix->rax;
    _inti_ebp = ix->rbp;
    _inti_ebx = ix->rbx;
    _inti_ecx = ix->rcx;
    _inti_edi = ix->rdi;
    _inti_edx = ix->rdx;
    _inti_eflags = ix->rflags;
    _inti_eip = ix->rip;
    _inti_esi = ix->rsi;
    #endif
}

void initialise_idt_driver(){
    idtoffsetcode = 0x20;

    uint8_t oldpic1 = inportb(PIC1_DATA);
    uint8_t oldpic2 = inportb(PIC2_DATA);
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, idtoffsetcode);
    outportb(0xA1, idtoffsetcode + 8);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
    outportb(PIC1_DATA,oldpic1);
    outportb(PIC2_DATA,oldpic2);

    #ifndef __x86_64
    idtr.Offset = (uintptr_t)&idt[0];
    idtr.Limit = (uint16_t)sizeof(IDTDescEntry) * IDT_MAX_DESCRIPTORS - 1;
    for(uint8_t i = 0 ; i < IDT_MAX_DESCRIPTORS ; i++){
        IRQ_clear_mask(i);
        idt_set_gate(i,(unsigned long)DefaultInterruptHandler,0x08,0x8E);
        // setRawInterrupt(i,(unsigned long)GeneralFault_Handler);
    }
    for(uint16_t i = 0 ; i < idtoffsetcode ; i++){
        // IRQ_clear_mask(i);
        #ifndef __x86_64
        idt_set_gate(i,(unsigned long)isrpanic,0x08,0x8F);
        #else 
        idt_set_gate(i,(unsigned long)GeneralFault_Handler,0x08,0x8F);
        #endif
    }
    setRawInterrupt(0x80,isrint);
    setRawInterrupt(0x81,isr2int);
    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");
    return;
    #else
    // k_printf("get some info from the old idt...\n");
    asm volatile ("sidt %0" : "=m"(idtr));
    
    k_printf("sidt: Limit:%x Offset:%x \n",idtr.Limit,idtr.Offset);
    IDTDescEntry *idtentries = (IDTDescEntry*) idtr.Offset;
    for(uint16_t i = 0 ; i < idtr.Limit ; i++){
        setRawInterrupt(i,NakedInterruptHandler);
    }
    for(uint16_t i = 0 ; i < idtoffsetcode ; i++){
        IRQ_clear_mask(i);
        setRawInterrupt(i,GeneralFault_Handler);
    }
    // setRawInterrupt(0xCD,PageFault_Handler);
    setRawInterrupt(0x80,isrint);
    setRawInterrupt(0x81,isr2int);
    asm volatile ("sti");
    #endif 
}