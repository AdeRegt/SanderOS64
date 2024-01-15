#pragma once
#include <stdint.h>
#include "sizet.h"
#include "boot.h"

void set_graphics_info(GraphicsInfo *gi);
GraphicsInfo *get_graphics_info();
void clear_screen(unsigned int colour);
void draw_pixel_at(unsigned int x,unsigned int y,unsigned int colour);
unsigned int get_pixel_at(unsigned int x,unsigned int y);
void printString(char* message);
void k_printf(char* format,...);
void initialise_graphics_driver();
void putc(char deze);
unsigned int create_colour_code(unsigned char red,unsigned char green,unsigned char blue,unsigned char alpha);
void repaint();
void itoa(int n, char *buffer, int base);
char *convert(unsigned int num, int base);