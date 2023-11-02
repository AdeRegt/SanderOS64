#include <stdio.h>

void hang(const char* func){
    puts("\nHANG: ");
    puts(func);
    puts(" \n");
    for(;;);
}