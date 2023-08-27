#pragma once
#include <stdint.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71
#define CMOS_STATUS_A 0x0A

uint8_t getCentury();
uint8_t getSecond();
uint8_t getMinute();
uint8_t getHour();
uint8_t getDay();
uint8_t getMonth();
char* getMonthString();
uint8_t getYear();
