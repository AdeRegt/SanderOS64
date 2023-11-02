#include <stdint.h>
#include <SanderOS.h>
#include <stdio.h>

void scanline(char* where,char echo){
    int i = 0;
    while(1){
        char deze = scan_for_character();
        putsc(deze);
        if(deze=='\n'){
            break;
        }
        where[i++] = deze;
        where[i] = 0;
    }
}
