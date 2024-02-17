#pragma once
#include <stdint.h>
#include "sizet.h"
#include "boot.h"

void set_graphics_info(GraphicsInfo *gi);
GraphicsInfo *get_graphics_info();
void clear_screen(unsigned int colour);
void clear_screen_at_buffer(GraphicsInfo* buffer,unsigned int colour);
void draw_pixel_at(unsigned int x,unsigned int y,unsigned int colour);
void draw_pixel_at_buffer(GraphicsInfo* buffer,unsigned int x,unsigned int y,unsigned int colour);
unsigned int get_pixel_at(unsigned int x,unsigned int y);
unsigned int get_pixel_at_buffer(GraphicsInfo* buffer,unsigned int x,unsigned int y);
void printStringAt(GraphicsInfo* buffer,char* message);
void* get_graphics_buffer();
void k_printf_at(GraphicsInfo* buffer,char* format,...);
void k_printf(char* format,...);
void initialise_graphics_driver();
void putc(char deze);
void putc_at_buffer(GraphicsInfo* buffer,char deze);
unsigned int create_colour_code(unsigned char red,unsigned char green,unsigned char blue,unsigned char alpha);
void repaint();
void itoa(int n, char *buffer, int base);
char *convert(unsigned int num, int base);
void move_text_pointer(unsigned long x,unsigned long y);
void move_text_pointer_at_buffer(GraphicsInfo* buffer,unsigned long x,unsigned long y);
GraphicsInfo* new_instance_of_graphics_info(int width,int height);