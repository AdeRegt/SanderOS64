#pragma once 
#include <stdint.h>

void initialise_comport();
uint8_t is_com_enabled();
void com_write_debug_serial(char a);