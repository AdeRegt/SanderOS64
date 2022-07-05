#pragma once
#include <stdint.h>

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_REGISTER 0x64
#define PS2_COMMAND_REGISTER 0x64

#define VK_UP 0xCB
#define VK_LEFT 0xCC
#define VK_RIGHT 0xCD
#define VK_DOWN 0xCE
#define VK_SHIFT 0xCF

void initialise_ps2_driver();
uint8_t getch(uint8_t wait);