#pragma once
#include <stdint.h>
#include "../../include/outint.h"

/**
 * @brief Prints the value of one instruction
 * 
 * @param location where the code is stored
 * @return int the size of the instruction
 */
int debugger_interpetate_instruction(void* location);

int debugger_interpetate_next_x_instructions(void *location,upointer_t times);