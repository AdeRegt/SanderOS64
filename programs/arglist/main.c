#include <stdio.h>
#include <stdlib.h>

int main(int argc,char** argv){
    printf("Hello world!\n");
    for(int i = 0 ; i < argc ; i++){
        printf("arg%d: %s \n",i,argv[i]);
    }
    printf("For more information, see the nice docs!\n");
    exit(0);
}