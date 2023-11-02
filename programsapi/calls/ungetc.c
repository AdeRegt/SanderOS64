#include <stdio.h>

int ungetc(int chart, FILE *stream){
    fputc(chart,stream);
}
