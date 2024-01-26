#include <stdio.h>
#include <SanderOS.h>

void assert(int a){
    if(!a){
        printf("__ASSERT FAILURE__");
        hang("");
    }
}
