#include "../include/graphics.h"
#include "../include/memory.h"
#include "../include/comport.h"
#include "../include/psf.h"

GraphicsInfo *graphics_info;

GraphicsInfo* new_instance_of_graphics_info(int width,int height){
    GraphicsInfo* buff = requestPage();
    memset(buff,0,sizeof(GraphicsInfo));
    memcpy(buff,graphics_info,sizeof(GraphicsInfo));
    buff->BaseAddress = requestPage();
    buff->Height = height;
    buff->Width = width;
    buff->pointerX = 0;
    buff->pointerY = 0;
    return buff;
}

unsigned int* getMouseCoordinates();

void repaint(){
    
}

void set_graphics_info(GraphicsInfo *gi){
    graphics_info = gi;
}

GraphicsInfo *get_graphics_info(){
    return graphics_info;
}

void* get_graphics_buffer(){
    return graphics_info->BaseAddress;
}

void draw_pixel_at(unsigned int x,unsigned int y,unsigned int colour){
    draw_pixel_at_buffer(graphics_info,x,y,colour);
}

void draw_pixel_at_buffer(GraphicsInfo* buffer,unsigned int x,unsigned int y,unsigned int colour){
    if(x>=buffer->Width){
        return;
    }
    if(y>=buffer->Height){
        return;
    }
    unsigned int BBP = 4;
    *(unsigned int*)((x*BBP) +(y*buffer->PixelsPerScanLine*BBP) + buffer->BaseAddress) = colour;
}

unsigned int get_pixel_at(unsigned int x,unsigned int y){
    unsigned int BBP = 4;
    return *(unsigned int*)((x*BBP) +(y*graphics_info->PixelsPerScanLine*BBP) + graphics_info->BaseAddress);
}

unsigned int get_pixel_at_buffer(GraphicsInfo* buffer,unsigned int x,unsigned int y){
    unsigned int BBP = 4;
    return *(unsigned int*)((x*BBP) +(y*buffer->PixelsPerScanLine*BBP) + buffer->BaseAddress);
}

void move_text_pointer(unsigned long x,unsigned long y){
    move_text_pointer_at_buffer(graphics_info,x,y);
}

void move_text_pointer_at_buffer(GraphicsInfo* buffer,unsigned long x,unsigned long y){
    buffer->pointerX = x;
    buffer->pointerY = y;
}

unsigned int create_colour_code(unsigned char red,unsigned char green,unsigned char blue,unsigned char alpha){
    return (alpha<<23) | (red<<16) | (green<<8) | blue;
}

void clear_screen(unsigned int colour){
    clear_screen_at_buffer(graphics_info,colour);
}

void clear_screen_at_buffer(GraphicsInfo* buffer,unsigned int colour){
    asm volatile("cli");
    if(buffer->strategy==1){
        unsigned int BBP = 4;
        for(unsigned int y = 0 ; y < buffer->Height ; y++){
            for(unsigned int x = 0 ; x < buffer->Width ; x++){
                draw_pixel_at_buffer(buffer,x,y,colour);
            }
        }
        buffer->pointerX = 50;
        buffer->pointerY = 50;
    }else{
        buffer->pointerX = 0;
        buffer->pointerY = 0;
        for(unsigned int y = 0 ; y < buffer->Height ; y++){
            for(unsigned int x = 0 ; x < buffer->Width ; x++){
                putc_at_buffer(buffer,' ');
            }
        }
        buffer->pointerX = 0;
        buffer->pointerY = 0;
    }
    asm volatile("sti");
}

void putc_at_buffer(GraphicsInfo* buffer,char deze){
    if(buffer->strategy==1){
        if(buffer->pointerY>(buffer->Height-50)){
            clear_screen(0xFFFFFFFF);
            buffer->pointerX = 50;
            buffer->pointerY = 50;
        }
        if(deze=='\n'||buffer->pointerX>(buffer->Width-50)){
            buffer->pointerX = 50;
            buffer->pointerY += 16;
        }else{
            drawCharacterAtBuffer(buffer,getActiveFont(),deze,0x00000000,buffer->pointerX,buffer->pointerY);
            buffer->pointerX += 8;
        }
    }else{
        if(buffer->pointerY>=25){
            buffer->pointerX = 0;
            buffer->pointerY = 0;
        }
        if(deze=='\n'){
            buffer->pointerX = 0;
            buffer->pointerY++;
            return;
        }
        ((char*)(buffer->BaseAddress+(160*buffer->pointerY)+(buffer->pointerX*2)))[0] = deze;
        buffer->pointerX++;
        if(buffer->pointerX==80){
            buffer->pointerX = 0;
            buffer->pointerY++;
        }
    }
}

void putc(char deze){
    if(is_com_enabled()){
        com_write_debug_serial(deze);
    }
    putc_at_buffer(graphics_info,deze);
}

char* strcpy(char* destination, char* source){
    int size = strlen(source);
    memcpy(destination,source,size);
    return destination;
}

void reverse(char *s){
    char *j;
    int i = strlen(s);

    strcpy(j,s);
    while (i-- >= 0){
        *(s++) = j[i];
    }
    *s = '\0';
}

void itoa(int n, char *buffer, int base)
{
    char *ptr = buffer;
    int lowbit;

    base >>= 1;
    do
    {
        lowbit = n & 1;
        n = (n >> 1) & 32767;
        *ptr = ((n % base) << 1) + lowbit;
        if (*ptr < 10){
            *ptr +='0';
        }else{
            *ptr +=55;
        }
        ++ptr;
    }
    while (n /= base);
    *ptr = '\0';
    reverse (buffer);   /* reverse string */
}

void printStringAt(GraphicsInfo* buffer,char* message){
    int length = 0;
    while(1){
        char deze = message[length];
        if(deze=='\0'){
            break;
        }else{
            putc_at_buffer(buffer,deze);
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

void k_printf_at(GraphicsInfo* buffer,char* format,...){
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
                putc_at_buffer(buffer,i);
            }else if(deze=='%'){
                putc_at_buffer(buffer,'%');
            }else if(deze=='s'){
                char *s = va_arg(arg,char *);
                printStringAt(buffer,s);
            }else if(deze=='x'){
                int t = va_arg(arg,unsigned int);
                putc_at_buffer(buffer,'0');
                putc_at_buffer(buffer,'x');
                char *convertednumber = convert(t,16);
                printStringAt(buffer,convertednumber);
            }else if(deze=='d'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,10);
                printStringAt(buffer,convertednumber);
            }else if(deze=='o'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,8);
                printStringAt(buffer,convertednumber);
            }
            length++;
        }else{
            putc_at_buffer(buffer,deze);
            length++;
        }
    }
    va_end(arg);
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
                printStringAt(graphics_info,s);
            }else if(deze=='x'){
                int t = va_arg(arg,unsigned int);
                putc('0');
                putc('x');
                char *convertednumber = convert(t,16);
                printStringAt(graphics_info,convertednumber);
            }else if(deze=='d'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,10);
                printStringAt(graphics_info,convertednumber);
            }else if(deze=='o'){
                int t = va_arg(arg,unsigned int);
                char *convertednumber = convert(t,8);
                printStringAt(graphics_info,convertednumber);
            }
            length++;
        }else{
            putc(deze);
            length++;
        }
    }
    va_end(arg);
}


unsigned long oldmousex = 0;
unsigned long oldmousey = 0;
char mousebuffer[10*10];

void restoreMouseImageBuffer(){
    for (unsigned long y = 0; y < 0 + getActiveFont()->psf1_Header->charsize; y++){
        for (unsigned long x = 0; x < 0+8; x++){
            draw_pixel_at(oldmousex + x,oldmousey + y,mousebuffer[(y*10)+x]);
        }
    }
}

void storeMouseImageBuffer(){
    for (unsigned long y = 0; y < 0 + getActiveFont()->psf1_Header->charsize; y++){
        for (unsigned long x = 0; x < 0+8; x++){
            mousebuffer[(y*10)+x] = get_pixel_at(oldmousex + x,oldmousey + y);
        }
    }
}

void drawMouseAt(unsigned long xOff,unsigned long yOff){
    restoreMouseImageBuffer();
    oldmousex = xOff;
    oldmousey = yOff;
    storeMouseImageBuffer();
    drawCharacter(getActiveFont(),'#',0x00000000,xOff,yOff);
}

void fillMouseImageBuffer(unsigned int val){
    for (unsigned long y = 0; y < 0 + getActiveFont()->psf1_Header->charsize; y++){
        for (unsigned long x = 0; x < 0+8; x++){
            mousebuffer[(y*10)+x] = val;
        }
    }
}


void initialise_graphics_driver(){
    clear_screen(0xFFFFFFFF);
    setActiveFont(getDefaultFont());
    k_printf("Graphics driver initialised!\nBase Address: %x\nHeight: %x \nPixelsPerScanline: %x \nWidth: %x \n",graphics_info->BaseAddress,graphics_info->Height,graphics_info->PixelsPerScanLine,graphics_info->Width);
}