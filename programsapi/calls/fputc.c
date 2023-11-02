#include <stdio.h>

int fputc(int chart, FILE *stream){
    char t[2];
    t[0] = 0;
    t[1] = 0;
    t[0] = chart;
    write((int)((upointer_t)stream),t,1);
}