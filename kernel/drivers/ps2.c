#include "../include/ps2.h"
#include "../include/ports.h"
#include "../include/graphics.h"
#include "../include/timer.h"
#include "../include/device.h"

unsigned char kbdus[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',    /* 9 */
    '9', '0', '-', '=', '\b',                         /* Backspace */
    '\t',                                             /* Tab */
    'q', 'w', 'e', 'r',                               /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     /* Enter key */
    0,                                                /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`', VK_SHIFT,                              /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',               /* 49 */
    'm', ',', '.', '/', VK_SHIFT,                     /* Right shift */
    '*',
    0,   /* Alt */
    ' ', /* Space bar */
    0,   /* Caps lock */
    0,   /* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0,
    0,     /* < ... F10 */
    0,     /* 69 - Num lock*/
    0,     /* Scroll Lock */
    0,     /* Home key */
    VK_UP, /* Up Arrow */
    0,     /* Page Up */
    '-',
    VK_LEFT, /* Left Arrow */
    0,
    VK_RIGHT, /* Right Arrow */
    '+',
    1,       /* 79 - End key*/
    VK_DOWN, /* Down Arrow */
    0,       /* Page Down */
    0,       /* Insert Key */
    0,       /* Delete Key */
    0, 0, 0,
    0, /* F11 Key */
    0, /* F12 Key */
    0, /* All other keys are undefined */
};

unsigned char KBDUS[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*',    /* 9 */
    '(', ')', '_', '+', '\b',                         /* Backspace */
    '\t',                                             /* Tab */
    'Q', 'W', 'E', 'R',                               /* 19 */
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',     /* Enter key */
    0,                                                /* 29   - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
    '\"', '~', VK_SHIFT,                              /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N',                /* 49 */
    'M', '<', '>', '?', VK_SHIFT,                     /* Right shift */
    '*',
    0,   /* Alt */
    ' ', /* Space bar */
    0,   /* Caps lock */
    0,   /* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0,
    0,     /* < ... F10 */
    0,     /* 69 - Num lock*/
    0,     /* Scroll Lock */
    0,     /* Home key */
    VK_UP, /* Up Arrow */
    0,     /* Page Up */
    '-',
    VK_LEFT, /* Left Arrow */
    0,
    VK_RIGHT, /* Right Arrow */
    '+',
    1,       /* 79 - End key*/
    VK_DOWN, /* Down Arrow */
    0,       /* Page Down */
    0,       /* Insert Key */
    0,       /* Delete Key */
    0, 0, 0,
    0, /* F11 Key */
    0, /* F12 Key */
    0, /* All other keys are undefined */
};

uint8_t ps2_get_status()
{
    return inportb(PS2_STATUS);
}

uint8_t ps2_ready_to_write()
{
    int i = 0;
    while (ps2_get_status() & PS2_MASK_NOT_READY_TO_WRITE)
    {
        i++;
        sleep(1);
        if (i > 10)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t ps2_ready_to_read()
{
    int i = 0;
    while (!(ps2_get_status() & PS2_MASK_NOT_READY_TO_READ))
    {
        i++;
        sleep(1);
        if (i > 10)
        {
            return 0;
        }
    }
    return 1;
}

uint8_t write_to_first_ps2_port(uint8_t data)
{
    if (!ps2_ready_to_write())
    {
        return 0;
    }
    outportb(PS2_DATA, data);
    return 1;
}

uint8_t write_to_second_ps2_port(uint8_t data)
{
    outportb(PS2_STATUS, 0xD4);
    if (!ps2_ready_to_write())
    {
        return 0;
    }
    outportb(PS2_DATA, data);
    return 1;
}

uint8_t ps2_wait_for_result(uint8_t expected_result)
{
    int i = 0;
    while (inportb(PS2_DATA) != expected_result)
    {
        i++;
        if (i > 10)
        {
            return 0;
        }
        sleep(1);
    }
    return 1;
}

uint8_t ps2_echo_check()
{
    if (!write_to_first_ps2_port(PS2_DATA_ECHO))
    {
        return 0;
    }
    return ps2_wait_for_result(PS2_DATA_RESULT_ECHO);
}

static uint8_t shift_pressed = 0;
static uint8_t last_pressed_key = 0;

uint8_t ps2_scancode_to_char(uint8_t ps)
{
    uint8_t ts = ps;
    ts &= ~0x80;
    return (shift_pressed == 1 ? KBDUS : kbdus)[ts];
}

static uint8_t mouse_cycle = 0;
static uint8_t mouse_1 = 0;
static uint8_t mouse_2 = 0;
static uint8_t mouse_3 = 0;
static unsigned int mouse_x = 50;
static unsigned int mouse_y = 50;

unsigned int mousecoordinatesbuffer[2];

unsigned int* getMouseCoordinates(){
    mousecoordinatesbuffer[0] = mouse_x;
    mousecoordinatesbuffer[1] = mouse_y;
    return (unsigned int*)&mousecoordinatesbuffer;
}

__attribute__((interrupt)) void irq_mouse(interrupt_frame *frame)
{
    uint8_t status = inportb(0x64);
    if (status & 1)
    {
        uint8_t sts = inportb(0x60);
        if(mouse_cycle==0){
            mouse_1 = sts;
            mouse_cycle = 1;
        }else if(mouse_cycle==1){
            mouse_2 = sts;
            mouse_cycle = 2;
        }else if(mouse_cycle==2){
            mouse_3 = sts;
            mouse_cycle = 0;
            if(mouse_1&0x4){
                k_printf("mouse: Middle button\n");
            }else if(mouse_1&0x2){
                k_printf("mouse: Right button\n");
            }else if(mouse_1&0x1){
                k_printf("mouse: Left button\n");
            }
            if(mouse_1&0x10){
                mouse_x -= 1;
            }else if(mouse_2){
                mouse_x += 1;
            }
            if(mouse_1&0x20){
                mouse_y += 1;
            }else if(mouse_3){
                mouse_y -= 1;
            }
        }
    }
    
    if(mouse_x<10){
        mouse_x = 10;
    }
    if(mouse_y<10){
        mouse_y = 10;
    }
    if(mouse_x>get_graphics_info()->Width){
        mouse_x = get_graphics_info()->Width;
    }
    if(mouse_y>get_graphics_info()->Height){
        mouse_y = get_graphics_info()->Height;
    }

    repaint();

    // EOI
    outportb(0x20, 0x20);
    outportb(0xA0, 0x20);
}

__attribute__((interrupt)) void irq_keyboard(interrupt_frame *frame)
{
    uint8_t key = inportb(PS2_DATA);
    if (key == 42 || key == 54)
    {
        shift_pressed = 1;
    }
    else if (key == 170 || key == 182)
    {
        shift_pressed = 0;
    }
    else if (key & 0x80)
    {
        last_pressed_key = ps2_scancode_to_char(key);
    }
    outportb(0x20, 0x20);
}

uint8_t get2ch(uint8_t wait)
{
    if (wait)
    {
        while (last_pressed_key == 0)
            ;
    }
    uint8_t tmp = last_pressed_key;
    last_pressed_key = 0;
    return tmp;
}

uint8_t initialise_ps2_mouse()
{
    sleep(1);
    outportb(0x64, 0xA8);
    sleep(1);
    outportb(0x64, 0x20);
    sleep(1);
    unsigned char status = inportb(0x60);
    inportb(0x60);
    status |= (1 << 1);
    status &= ~(1 << 5);
    sleep(1);
    outportb(0x64, 0x60);
    sleep(1);
    outportb(0x60, status);

    write_to_second_ps2_port(0xF6);
    ps2_wait_for_result(PS2_DATA_RESULT_ACK);
    write_to_second_ps2_port(0xF4);
    ps2_wait_for_result(PS2_DATA_RESULT_ACK);

    setInterrupt(12, irq_mouse);
    k_printf("ps2: initialisation of ps2-mouse succeed\n");
    return 1;

}

uint8_t initialise_ps2_keyboard()
{
    if (!write_to_first_ps2_port(0xF4))
    {
        goto error;
    }
    ps2_wait_for_result(PS2_DATA_RESULT_ACK);

    setInterrupt(1, irq_keyboard);
    registerHIDDevice(get2ch);
    k_printf("ps2: initialisation of ps2-keyboard succeed\n");
    return 1;

error:
    k_printf("ps2: initialisation of ps2-keyboard failed\n");
    return 0;
}

void initialise_ps2_driver()
{
    k_printf("ps2: ps2 driver loaded!\n");
    if (!ps2_echo_check())
    {
        k_printf("ps2: ping failed!\n");
        return;
    }
    k_printf("ps2: ping succeed!\n");
    initialise_ps2_mouse();
    initialise_ps2_keyboard();
}