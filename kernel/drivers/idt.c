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
        int_PageFault->selector = GDT_CODE_SEGMENT;
    #else 
        IRQ_clear_mask(32 + offset);
        idt_set_gate(32 + (unsigned char)offset, (unsigned long)fun, GDT_CODE_SEGMENT, IDT_TA_InterruptGate);
    #endif 
}

void setRawInterrupt(int offset,void *fun){
    #ifdef __x86_64
        IDTDescEntry* int_PageFault = (IDTDescEntry*)(idtr.Offset + ((offset) * sizeof(IDTDescEntry)));
        interrupt_set_offset(int_PageFault,(upointer_t)fun);
        int_PageFault->type_attr = IDT_TA_TrapGate;
        int_PageFault->selector = GDT_CODE_SEGMENT;
    #else
        idt_set_gate((unsigned char)offset, (unsigned long)fun, GDT_CODE_SEGMENT, IDT_TA_InterruptGate);
    #endif
}

extern void isrint();
extern void isr2int();

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
        ix->rip = (upointer_t)tty_inner_loop;
    }else if(ix->rax==2){
        k_printf("isr: fork not implemented!\n");
    }else if(ix->rax==3){
        if(ix->rbx<5){
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
        k_printf("isr: Unknown isr code: %d !\n",ix->rax);
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
        #ifdef IDT_DEBUG
        k_printf("isr2:request read\n");
        #endif
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rbx]);
        memcpy((void*)ix->rdx,(void*)(fl->buffer + fl->pointer),ix->rcx);
        fl->pointer = fl->pointer + ix->rcx;
    }else if(ix->rax==1){
        char* z = ((char*) (ix->rsi));
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rbx]);
        if(ix->rdi<5){
            for(upointer_t i = 0 ; i < ix->rdx ; i++){
                k_printf("%c",z[i]);
            }
        }else{
            upointer_t oldsize = 0;
            if(fl->flags==0){
                oldsize = getFileSize(fl->filename);
            }
            upointer_t newsize = oldsize + ix->rcx;
            void *tmpbuf = requestPage();
            if(fl->flags==0){
                readFile(fl->filename,tmpbuf);
            }
            #ifdef IDT_DEBUG
            k_printf("isr2:request write to [%s] newsize:%d oldsize:%d \n",fl->filename,newsize,oldsize);
            #endif
            memcpy((void*)(tmpbuf + oldsize),z,ix->rcx);
            upointer_t q = writeFile(fl->filename,tmpbuf,newsize);
            if(q==0){
                ix->rax = -1;
            }else{
                ix->rax = q;
            }
            freePage(tmpbuf);
        }
    }else if(ix->rax==2){
        // k_printf("isr2:request open rsi=%x \n",ix->rsi);for(;;);
        // fileopen option!
        char* path = (char*) ix->rdx;
        #ifdef IDT_DEBUG
        k_printf("isr2:request to open %s \n",path);
        #endif
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
        for(int i = 6 ; i < 10 ; i++){
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
        // k_printf("Opening file with ID %d \n",sov);
        ix->rax = sov;
    }else if(ix->rax==3){
        #ifdef IDT_DEBUG
        k_printf("isr2:request close\n");
        #endif
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rdi]);
        fl->available = 0;
        ix->rax = 0;
    }else if(ix->rax==8){
        // "D" (stream) , "d" (origin) , "S" (offset)
        #ifdef IDT_DEBUG
        k_printf("isr2:request seek fileid(rbx):%d , args(rcx):%d , method(rdx):%d \n",ix->rbx,ix->rcx,ix->rdx);
        #endif
        // seek option!
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rbx]);
        if(ix->rcx==2){
            fl->pointer = fl->filesize;
        }else{
            fl->pointer = ix->rdx;
        }
        ix->rax = fl->pointer;
        // ix->rax = 0;
    }else if(ix->rax==12){
        #ifdef IDT_DEBUG
        k_printf("isr2:request space\n");
        #endif
        // we always accept extending memory whatoever
        ix->rax = ix->rdi;
    }else if(ix->rax==60){
        #ifdef IDT_DEBUG
        k_printf("isr2: end\n");
        #endif
        Task* ts = (Task*) (getTasks() + (sizeof(Task)*getPid()));
        ts->task_running = 0;
        ix->rip = (upointer_t)tty_inner_loop;
    }else if(ix->rax==96){
        #ifdef IDT_DEBUG
        k_printf("isr2:request time\n");
        #endif
        timeval* tv = (timeval*) ix->rdi;
        tv->tv_sec = 10000;
        tv->tv_usec = 10100;
        ix->rax = 0;
    }else if(ix->rax==400){
        #ifdef IDT_DEBUG
        k_printf("isr2:request page\n");
        #endif
        ix->rax = (upointer_t) requestPage();
    }else if(ix->rax==401){
        #ifdef IDT_DEBUG
        k_printf("isr2:request argument\n");
        #endif
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
        #ifdef IDT_DEBUG
        k_printf("isr2:request ip from name\n");
        #endif
        ix->rax = (upointer_t)getIPFromName(name);
    }else if(ix->rax==406){
        #ifdef IDT_DEBUG
        k_printf("isr2:request mac from ip\n");
        #endif
        uint8_t *name = (uint8_t*) ix->rbx;
        ix->rax = (upointer_t)getMACFromIp(name);
    }else if(ix->rax==407){
        #ifdef IDT_DEBUG
        k_printf("isr2:request tcp session\n");
        #endif
        create_tcp_session(getOurIpAsLong(),((unsigned long*)ix->rbx)[0],ix->rcx,ix->rcx,ix->rdx);
    }else if(ix->rax==408){
        #ifdef IDT_DEBUG
        k_printf("isr2:request sleep\n");
        #endif
        sleep(ix->rbx);
    }else if(ix->rax==409){
        #ifdef IDT_DEBUG
        k_printf("isr2:release page\n");
        #endif
        freePage((void*)ix->rbx);
    }else if(ix->rax==410){
        #ifdef IDT_DEBUG
        k_printf("isr2:request env variable [%s] \n",(char*) ix->rdx);
        #endif
        char* p = "A:INCLUDE/";
        ix->rax = (upointer_t)p;
    }else if(ix->rax==411){
        #ifdef IDT_DEBUG
        k_printf("isr2:request tell \n");
        #endif
        File *fl = (File*) &(getCurrentTaskInfo()->files[ix->rbx]);
        ix->rax = fl->pointer;
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

__attribute__((interrupt)) void Error00(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x00 :: Division Error\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error01(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x01 :: Debug\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error02(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x02 :: Non-maskable Interrupt\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error03(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x03 :: Breakpoint\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error04(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x04 :: Overflow\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error05(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x05 :: Bound Range Exceeded\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error06(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x06 :: Invalid Opcode\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error07(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x07 :: Device Not Available\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error08(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x08 :: Double Fault\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error09(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x09 :: Coprocessor Segment Overrun\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error0A(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x0A :: Invalid TSS\ncs=%x flags=%x ip=%x sp=%x ss=%x ec=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error0B(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x0B :: Segment Not Present\ncs=%x flags=%x ip=%x sp=%x ss=%x ec=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error0C(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x0C :: Stack-Segment Fault\ncs=%x flags=%x ip=%x sp=%x ss=%x ec=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error0D(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x0D :: General Protection Fault\ncs=%x flags=%x ip=%x sp=%x ss=%x ec=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error0E(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x0E :: Page Fault\ncs=%x flags=%x ip=%x sp=%x ss=%x ec=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error0F(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x0F :: Reserved\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error10(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x10 :: x87 Floating-Point Exception\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error11(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x11 :: Alignment Check\ncs=%x flags=%x ip=%x sp=%x ss=%x ec=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error12(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x12 :: Machine Check\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error13(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x13 :: SIMD Floating-Point Exception\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error14(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x14 :: Virtualization Exception\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error15(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x15 :: Control Protection Exception\ncs=%x flags=%x ip=%x sp=%x ss=%x ec=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error16(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x16 :: Reserved\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error17(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x17 :: Reserved\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error18(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x18 :: Reserved\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error19(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x19 :: Reserved\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error1A(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x1A :: Reserved\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error1B(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x1B :: Reserved\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error1C(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x1C :: Hypervisor Injection Exception\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error1D(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x1D :: VMM Communication Exception\ncs=%x flags=%x ip=%x sp=%x ss=%x ec=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error1E(interrupt_frame* frame,upointer_t errorcode){
	interrupt_eoi();
    k_printf("int: 0x1E :: Security Exception\ncs=%x flags=%x ip=%x sp=%x ss=%x ec=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss,errorcode);
	asm volatile("cli\nhlt");
}

__attribute__((interrupt)) void Error1F(interrupt_frame* frame){
	interrupt_eoi();
    k_printf("int: 0x1F :: Reserved\ncs=%x flags=%x ip=%x sp=%x ss=%x\n",frame->cs,frame->flags,frame->ip,frame->sp,frame->ss);
	asm volatile("cli\nhlt");
}

void initialise_idt_driver(){
    idtoffsetcode = 0x20;

    uint8_t oldpic1 = inportb(PIC1_DATA);
    uint8_t oldpic2 = inportb(PIC2_DATA);
    outportb(PIC1, 0x11);
    outportb(PIC2, 0x11);
    outportb(PIC1_DATA, idtoffsetcode);
    outportb(PIC2_DATA, idtoffsetcode + 8);
    outportb(PIC1_DATA, ICW1_INTERVAL4);
    outportb(PIC2_DATA, ICW1_SINGLE);
    outportb(PIC1_DATA, ICW1_ICW4);
    outportb(PIC2_DATA, ICW1_ICW4);
    outportb(PIC1_DATA, 0x0);
    outportb(PIC2_DATA, 0x0);
    outportb(PIC1_DATA,oldpic1);
    outportb(PIC2_DATA,oldpic2);

    #ifndef __x86_64
    idtr.Offset = (uintptr_t)&idt[0];
    idtr.Limit = (uint16_t)sizeof(IDTDescEntry) * IDT_MAX_DESCRIPTORS - 1;
    for(uint8_t i = 0 ; i < IDT_MAX_DESCRIPTORS ; i++){
        IRQ_clear_mask(i);
        idt_set_gate(i,(unsigned long)DefaultInterruptHandler,GDT_CODE_SEGMENT,IDT_TA_InterruptGate);
        // setRawInterrupt(i,(unsigned long)GeneralFault_Handler);
    }
    for(uint16_t i = 0 ; i < idtoffsetcode ; i++){
        // IRQ_clear_mask(i);
        #ifndef __x86_64
        idt_set_gate(i,(unsigned long)isrpanic,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
        #else 
        idt_set_gate(i,(unsigned long)GeneralFault_Handler,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
        #endif
    }
    // https://wiki.osdev.org/Exceptions
    idt_set_gate(0x00,(unsigned long)Error00,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x01,(unsigned long)Error01,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x02,(unsigned long)Error02,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x03,(unsigned long)Error03,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x04,(unsigned long)Error04,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x05,(unsigned long)Error05,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x06,(unsigned long)Error06,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x07,(unsigned long)Error07,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x08,(unsigned long)Error08,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x09,(unsigned long)Error09,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x0A,(unsigned long)Error0A,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x0B,(unsigned long)Error0B,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x0C,(unsigned long)Error0C,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x0D,(unsigned long)Error0D,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x0E,(unsigned long)Error0E,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x0F,(unsigned long)Error0F,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x10,(unsigned long)Error10,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x11,(unsigned long)Error11,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x12,(unsigned long)Error12,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x13,(unsigned long)Error13,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x14,(unsigned long)Error14,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x15,(unsigned long)Error15,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x16,(unsigned long)Error16,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x17,(unsigned long)Error17,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x18,(unsigned long)Error18,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x19,(unsigned long)Error19,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x1A,(unsigned long)Error1A,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x1B,(unsigned long)Error1B,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x1C,(unsigned long)Error1C,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x1D,(unsigned long)Error1D,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x1E,(unsigned long)Error1E,GDT_CODE_SEGMENT,IDT_TA_TrapGate);
    idt_set_gate(0x1F,(unsigned long)Error1F,GDT_CODE_SEGMENT,IDT_TA_TrapGate);

    setRawInterrupt(OSLEGACY,isrint);
    setRawInterrupt(OSINTERRUPTS,isr2int);
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
    setRawInterrupt(OSLEGACY,isrint);
    setRawInterrupt(OSINTERRUPTS,isr2int);
    asm volatile ("sti");
    #endif 
}