#pragma once 
#include <stdint.h>

#define PS2_STATUS 0x64
#define PS2_MASK_NOT_READY_TO_WRITE 0b00000010
#define PS2_MASK_NOT_READY_TO_READ 0b00000001
#define PS2_DATA 0x60
#define PS2_DATA_ECHO 0xEE
#define PS2_DATA_RESULT_ECHO PS2_DATA_ECHO
#define PS2_DATA_RESULT_ACK 0xFA

#define VK_UP 0xCB
#define VK_LEFT 0xCC
#define VK_RIGHT 0xCD
#define VK_DOWN 0xCE
#define VK_SHIFT 0xCF

uint8_t initialise_ps2_mouse();
uint8_t initialise_ps2_keyboard();
void initialise_ps2_driver();