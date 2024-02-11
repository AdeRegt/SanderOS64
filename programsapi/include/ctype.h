#ifndef CTYPE_H
#define CTYPE_H

int isalnum(int c);  // return 1 if the passed character is alphanumeric
int isalpha(int c);  // return 1 if the passed character is alphabetic
int iscntrl(int c);  // return 1 if the passed character is control character
int isdigit(int c);  // return 1 if the passed character is decimal digit
int islower(int c);  // return 1 if the passed character is lowercase letter
int isprint(int c);  // return 1 if the passed character is printable
int ispunct(int c);  // return 1 if the passed character is a punctuation character
int isspace(int c);  // return 1 if the passed character is white-space
int isupper(int c);  // return 1 if the passed character is an uppercase letter

#endif