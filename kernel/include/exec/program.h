#pragma once 
#include <stdint.h>
#include "../../include/outint.h"

int exec(uint8_t *path,char *argv);
int exec_memory(void* buffer,char *argv);
int exec_is_using_paging();