#include "ctype.h"

int isalnum(int c)
{
    if (48 <= c && c <= 57) return 1;
    if (65 <= c && c <= 90) return 1;
    if (97 <= c && c <= 122) return 1;

    return 0;
}

int isalpha(int c)
{
    if (65 <= c && c <= 90) return 1;
    if (97 <= c && c <= 122) return 1;

    return 0;
}

int iscntrl(int c)
{
    if (0 <= c && c <= 32) return 1;
    if (127 == c) return 1;

    return 0;
}

int isdigit(int c)
{
    if (48 <= c && c <= 57) return 1;

    return 0;
}

int islower(int c)
{
   if (97 <= c && c <= 122) return 1;
   return 0;
}

int isprint(int c)
{
    if (21 <= c && c <= 126) return 1;
    return 0;
}

int ispunct(int c)
{
    if (33 <= c && c <= 47) return 1;
    if (58 <= c && c <= 64) return 1;
    if (91 <= c && c <= 96) return 1;
    if (123 <= c && c <= 126) return 1;
    return 0;
}

int isspace(int c)
{
    if (9 == c || c == 32) return 1;
}

int isupper(int c)
{
    if (65 <= c && c <= 90) return 1;
    return 0;
}