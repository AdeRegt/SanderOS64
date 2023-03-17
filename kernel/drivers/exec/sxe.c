#include "../../include/exec/sxe.h"
#include "../../include/graphics.h"
#include "../../include/device.h"
#include "../../include/memory.h"

SXE_CPU *cpu;

uint8_t is_sxe(void *programmem)
{
    SXE_HEADER *mem = (SXE_HEADER*) programmem;
    if(mem->signature[0]==SXE_SIGNATURE_1&&mem->signature[1]==SXE_SIGNATURE_2&&mem->signature[2]==SXE_SIGNATURE_3&&mem->signature[3]==SXE_SIGNATURE_4)
    {
        return 1;
    }
    return 0;
}

int sxe_run(void *programmem)
{
    cpu = (SXE_CPU*) requestPage();
    memset(cpu,0,sizeof(SXE_CPU));
    SXE_HEADER *header = (SXE_HEADER*) programmem;
    cpu->memory = header;
    if(header->version!=SXE_VERSION)
    {
        k_printf("sxe: Unsupported VM version\n");
        return -1;
    }
    if(header->os_type!=SXE_OS_TYPE_SANDEROS64)
    {
        k_printf("sxe: Unsupported OS type!\n");
        return -1;
    }
    if(header->os_version!=SXE_OS_VER_SANDEROS64)
    {
        k_printf("sxe: Unsupported OS version!\n");
        return -1;
    }
    for(int i = 0 ; i < SXE_CALL_STACK_SIZE ; i++)
    {
        cpu->callstack[i] = 0;
    }
    while(cpu->memorypointer<SXE_MEMORY_SIZE)
    {
        uint16_t pointer = header->buffer[cpu->memorypointer];
        uint16_t opcode = ( pointer & 0xF000 ) >> 12;
        uint16_t argument = pointer & 0x0FFF;
        if(opcode==SXE_OPCODE_EXIT)
        {
            return argument;
        }
        else if(opcode==SXE_OPCODE_DEBUG)
        {
            k_printf("sxe: --DEBUG--\n");
            k_printf("sxe: mempointer: %x callstackpointer:%d \n",cpu->memorypointer,cpu->callstackpointer);
            k_printf("sxe: registers: A:%x B:%x \n",cpu->A,cpu->B);
            k_printf("sxe: callstack ");
            for(int i = 0 ; i < SXE_CALL_STACK_SIZE ; i++)
            {
                k_printf("%d:%x ",i,cpu->callstack[i]);
            }
            k_printf("\nsxe: press any key to continue\n");
            getch(1);
            cpu->memorypointer++;
        }
        else if(opcode==SXE_OPCODE_SYSCALL)
        {
            k_printf("sxe: syscall!\n");
            cpu->memorypointer++;
        }
        else if(opcode==SXE_OPCODE_RETURN)
        {
            if(cpu->callstackpointer==0)
            {
                return 0;
            }
            cpu->callstackpointer--;
            cpu->memorypointer = cpu->callstack[cpu->callstackpointer];
        }
        else if(opcode==SXE_OPCODE_CALL)
        {
            cpu->callstack[cpu->callstackpointer] = cpu->memorypointer + 1;
            cpu->callstackpointer++;
            cpu->memorypointer = argument;
        }
        else if(opcode==SXE_OPCODE_A2RA)
        {
            cpu->A = header->buffer[argument];
            cpu->memorypointer++;
        }
        else if(opcode==SXE_OPCODE_A2RB)
        {
            cpu->B = header->buffer[argument];
            cpu->memorypointer++;
        }
        else if(opcode==SXE_OPCODE_RA2A)
        {
            header->buffer[argument] = cpu->A;
            cpu->memorypointer++;
        }
        else if(opcode==SXE_OPCODE_RB2A)
        {
            header->buffer[argument] = cpu->B;
            cpu->memorypointer++;
        }
        else if(opcode==SXE_OPCODE_JUMP)
        {
            cpu->memorypointer = argument;
        }
        else if(opcode==SXE_OPCODE_FLAGS)
        {
            uint16_t bufferlocal = 0;
            if(argument==1){
                bufferlocal = cpu->A+cpu->B;
            }else if(argument==2){
                bufferlocal = cpu->A-cpu->B;
            }else if(argument==3){
                bufferlocal = cpu->A/cpu->B;
            }else if(argument==4){
                bufferlocal = cpu->A*cpu->B;
            }else if(argument==5){
                bufferlocal = cpu->A^cpu->B;
            }else if(argument==6){
                bufferlocal = cpu->A | cpu->B;
            }else if(argument==7){
                bufferlocal = cpu->A & cpu->B;
            }else if(argument==8){
                bufferlocal = ~cpu->A;
            }else if(argument==9){
                bufferlocal = cpu->A >> cpu->B;
            }else if(argument==10){
                bufferlocal = cpu->A << cpu->B;
            }else if(argument==11){
                bufferlocal = cpu->A && cpu->B;
            }else if(argument==12){
                bufferlocal = cpu->A || cpu->B;
            }else if(argument==13){
                bufferlocal = cpu->A++;
            }else if(argument==14){
                bufferlocal = cpu->A--;
            }else if(argument==15){
                bufferlocal = cpu->B;
                cpu->B = cpu->A;
            }
            cpu->A = bufferlocal;
            cpu->memorypointer++;
        }
        else if(opcode==SXE_OPCODE_JE)
        {
            if(cpu->A==cpu->B)
            {
                cpu->memorypointer = argument;
            }
            else
            {
                cpu->memorypointer++;
            }
        }
        else if(opcode==SXE_OPCODE_JM){
            if(cpu->A>cpu->B)
            {
                cpu->memorypointer = argument;
            }
            else
            {
                cpu->memorypointer++;
            }
        }
        else if(opcode==SXE_OPCODE_JL){
            if(cpu->A<cpu->B)
            {
                cpu->memorypointer = argument;
            }
            else
            {
                cpu->memorypointer++;
            }
        }
        else if(opcode==SXE_OPCODE_JNE){
            if(cpu->A!=cpu->B)
            {
                cpu->memorypointer = argument;
            }
            else
            {
                cpu->memorypointer++;
            }
        }
        else
        {
            k_printf("sxe: invalid opcode %x with argument %x at mempointer %x \n",opcode,argument,cpu->memorypointer);
            return -1;
        }
    }
    k_printf("sxe: WARNING: unexpected way of ending program\n");
    return 0;
}