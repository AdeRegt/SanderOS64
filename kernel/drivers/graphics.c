#include "../include/graphics.h"
#include "../include/comport.h"
#include "../include/psf.h"

GraphicsInfo *graphics_info;

unsigned int* getMouseCoordinates();

void repaint(){
    
}

void set_graphics_info(GraphicsInfo *gi){
    graphics_info = gi;
}

GraphicsInfo *get_graphics_info(){
    return graphics_info;
}

void draw_pixel_at(unsigned int x,unsigned int y,unsigned int colour){
    unsigned int BBP = 4;
    *(unsigned int*)((x*BBP) +(y*graphics_info->PixelsPerScanLine*BBP) + graphics_info->BaseAddress) = colour;
}

unsigned long pointerX = 50;
unsigned long pointerY = 50;

unsigned int create_colour_code(unsigned char red,unsigned char green,unsigned char blue,unsigned char alpha){
    return (alpha<<23) | (red<<16) | (green<<8) | blue;
}

void clear_screen(unsigned int colour){
    asm volatile("cli");
    if(graphics_info->strategy==1){
        unsigned int BBP = 4;
        for(unsigned int y = 0 ; y < graphics_info->Height ; y++){
            for(unsigned int x = 0 ; x < graphics_info->Width ; x++){
                draw_pixel_at(x,y,colour);
            }
        }
        pointerX = 50;
        pointerY = 50;
    }else{
        pointerX = 0;
        pointerY = 0;
        for(unsigned int y = 0 ; y < graphics_info->Height ; y++){
            for(unsigned int x = 0 ; x < graphics_info->Width ; x++){
                putc(' ');
            }
        }
        pointerX = 0;
        pointerY = 0;
    }
    asm volatile("sti");
}

void putc(char deze){
    if(is_com_enabled()){
        com_write_debug_serial(deze);
    }
    if(graphics_info->strategy==1){
        if(pointerY>(graphics_info->Height-50)){
            clear_screen(0xFFFFFFFF);
            pointerX = 50;
            pointerY = 50;
        }
        if(deze=='\n'||pointerX>(graphics_info->Width-50)){
            pointerX = 50;
            pointerY += 16;
        }else{
            drawCharacter(getActiveFont(),deze,0x00000000,pointerX,pointerY);
            pointerX += 8;
        }
    }else{
        if(pointerY>=25){
            pointerX = 0;
            pointerY = 0;
        }
        if(deze=='\n'){
            pointerX = 0;
            pointerY++;
            return;
        }
        ((char*)(graphics_info->BaseAddress+(160*pointerY)+(pointerX*2)))[0] = deze;
        pointerX++;
        if(pointerX==80){
            pointerX = 0;
            pointerY++;
        }
    }
}

void printString(char* message){
    int length = 0;
    while(1){
        char deze = message[length];
        if(deze=='\0'){
            break;
        }else{
            putc(deze);
        }
        length++;
    }
}

typedef __builtin_va_list va_list;

#define va_start(a,b)  __builtin_va_start(a,b)
#define va_end(a)      __builtin_va_end(a)
#define va_arg(a,b)    __builtin_va_arg(a,b)
#define __va_copy(d,s) __builtin_va_copy((d),(s))

#define NULL (void*)0

char *convert(unsigned int num, int base) { 
	static char Representation[]= "0123456789ABCDEF";
	static char buffer[50]; 
	char *ptr; 

    for(int i = 0 ; i < 50 ; i++){
        buffer[i] = 0x00;
    }

	if(num==0){
		ptr = &buffer[0];
		buffer[0] = '0';
		buffer[1] = 0;
		return ptr;
	}
	
	ptr = &buffer[49]; 
	*--ptr = '\0'; 
	*--ptr = '\0'; 
	*--ptr = '\0'; 
	
	do 
	{ 
		*--ptr = Representation[num%base]; 
		num /= base; 
	}while(num != 0);
	
	return(ptr); 
}

void k_printf(char* format,...){
    va_list arg; 
	va_start(arg, format);
    int length = 0;
    while(1){
        char deze = format[length];
        if(deze=='\0'){
            break;
        }else if(deze=='%'){
            length++;
            deze = format[length];
            if(deze=='c'){
                char i = va_arg(arg,int);
                putc(i);
            }else if(deze=='%'){
                putc('%');
            }else if(deze=='s'){
                char *s = va_arg(arg,char *);
                printString(s);
            }else if(deze=='x'){
                int t = va_arg(arg,unsigned int);
                putc('0');
                putc('x');
                char *convertednumber = convert(t,16);
                printString(convertednumber);
            }else if(deze=='d'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,10);
                printString(convertednumber);
            }else if(deze=='o'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,8);
                printString(convertednumber);
            }
            length++;
        }else{
            putc(deze);
            length++;
        }
    }
    va_end(arg);
}


void initialise_graphics_driver(){
    clear_screen(0xFFFFFFFF);
    setActiveFont(getDefaultFont());
    k_printf("Graphics driver initialised!\nBase Address: %x\nHeight: %x \nPixelsPerScanline: %x \nWidth: %x \n",graphics_info->BaseAddress,graphics_info->Height,graphics_info->PixelsPerScanLine,graphics_info->Width);
}