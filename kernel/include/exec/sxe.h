#pragma once 
#include <stdint.h>

#define SXE_SIGNATURE_1 'S'
#define SXE_SIGNATURE_2 'X'
#define SXE_SIGNATURE_3 'E'
#define SXE_SIGNATURE_4 0xCD
#define SXE_SIGNATURE_SIZE 4
#define SXE_MEMORY_SIZE 0x0FFF
#define SXE_VERSION 1
#define SXE_OS_TYPE_SANDEROS64 1
#define SXE_OS_VER_SANDEROS64 1
#define SXE_CALL_STACK_SIZE 10

// EXIT [EXITCODE]
#define SXE_OPCODE_EXIT 0xF

// DEBUG nothing
#define SXE_OPCODE_V2RB 0xE

// SYSCALL [ADDR] 
#define SXE_OPCODE_SYSCALL 0xD

// V2RA nothing
#define SXE_OPCODE_V2RA 0xC

// CALL [ADDR]
#define SXE_OPCODE_CALL 0xB

// ADDR to REGA [ADDR]
#define SXE_OPCODE_A2RA 0xA

// ADDR to REGB [ADDR]
#define SXE_OPCODE_A2RB 0x9

// REGA to ADDR [ADDR]
#define SXE_OPCODE_RA2A 0x8

// REGB to ADDR [ADDR]
#define SXE_OPCODE_RB2A 0x7

// JUMP [ADDR]
#define SXE_OPCODE_JUMP 0x6

// MATH [FLAGS]
#define SXE_OPCODE_FLAGS 0x5

// JE [ADDR]
#define SXE_OPCODE_JE 0x4

// JM [ADDR]
#define SXE_OPCODE_JM 0x3

// JL [ADDR]
#define SXE_OPCODE_JL 0x2

// JNE [ADDR]
#define SXE_OPCODE_JNE 0x1

// NOOPCODE
#define SXE_OPCODE_NOP 0x0  

typedef struct __attribute ((packed)) {
    uint8_t signature[SXE_SIGNATURE_SIZE];
    uint8_t version;
    uint8_t os_type;
    uint16_t os_version;
    uint16_t buffer[SXE_MEMORY_SIZE];
} SXE_HEADER;

typedef struct __attribute ((packed)) {
    uint16_t memorypointer;
    uint16_t callstack[SXE_CALL_STACK_SIZE];
    uint8_t callstackpointer;
    uint16_t stack[SXE_CALL_STACK_SIZE];
    uint8_t stackpointer;
    uint16_t A;
    uint16_t B;
    SXE_HEADER *memory;
} SXE_CPU;

uint8_t is_sxe(void *programmem);
int sxe_run(void *programmem);