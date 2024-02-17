#include "../include/winman.h"
#include "../include/graphics.h"
#include "../include/memory.h"
#include "../include/device.h"

int window_manager_window_count = 0;
SWindow windows[10];

int window_manager_enabled = 0;

void window_manager_set_enabled(int a){
    window_manager_enabled = a;
}

int window_manager_is_enabled(){
    return window_manager_enabled;
}

int window_manager_get_retention_time(){
    return 50;
}

SWindow *getWindowFromId(int id){
    return (SWindow*) &windows[id];
}

int counting_multitask = 0;
void window_manager_interrupt(){
    counting_multitask++;
    if(counting_multitask!=window_manager_get_retention_time()){
        return;
    }
    counting_multitask = 0;

    if(window_manager_is_enabled()){
        // draw everything
        for(int i = 0 ; i < window_manager_window_count ; i++){
            SWindow window = windows[i];
            window_manager_draw_window(window,i);
        }
    }
}

int window_manager_create_window(char* title){
    asm volatile ("cli");
    for(int i = 0 ; i < window_manager_window_count ; i++){
        windows[i].is_active = 0;
    }
    int oldcount = window_manager_window_count;
    window_manager_window_count++;
    windows[oldcount].title = title;
    windows[oldcount].x = ( get_graphics_info()->Width / 2 ) - (WINDOW_MANAGER_WINDOW_INITIAL_WIDTH / 2) ;
    windows[oldcount].y = ( get_graphics_info()->Height / 2 ) - (WINDOW_MANAGER_WINDOW_INITIAL_HEIGHT / 2) ;
    windows[oldcount].gi = new_instance_of_graphics_info(WINDOW_MANAGER_WINDOW_INITIAL_WIDTH,WINDOW_MANAGER_WINDOW_INITIAL_HEIGHT);
    windows[oldcount].is_active = 1;
    windows[oldcount].focuslocation = 0;
    windows[oldcount].is_used = 1;
    clear_screen_at_buffer(windows[oldcount].gi,WINDOW_MANAGER_WINDOW_COLOR);
    asm volatile ("sti");
    return oldcount;
}

int window_manager_add_element(int window_id,int type,int x,int y,void* data){
    int innere = windows[window_id].inner_elements_count;
    windows[window_id].elements[innere].type = type;
    windows[window_id].elements[innere].x = x;
    windows[window_id].elements[innere].y = y;
    windows[window_id].elements[innere].data = data;
    windows[window_id].elements[innere].can_be_selected = type == WINDOW_MANAGER_COMPONENT_BUTTON ? 1 : 0;
    windows[window_id].is_used = 1;
    windows[window_id].inner_elements_count++;
    return innere;
}


void move_text_pointer_buffer(int window_id,unsigned long x,unsigned long y){
    move_text_pointer_at_buffer(windows[window_id].gi,x,y);
}

void window_manager_draw_window(SWindow window,int window_id){
    if(window.is_used==0){
        return;
    }
    // clear old buffer
    clear_screen_at_buffer(window.gi,WINDOW_MANAGER_WINDOW_COLOR);
    // setup title bar
    move_text_pointer_buffer(window_id,WINDOW_MANAGER_WINDOW_TITLE_LEFT_OFFSET,0);
    printStringAt(window.gi,window.title);
    // setup all the components
    for(int i = 0 ; i < window.inner_elements_count ; i++){
        if(window.elements[i].type==WINDOW_MANAGER_COMPONENT_LABEL){
            move_text_pointer_buffer(window_id,window.elements[i].x,WINDOW_MANAGER_WINDOW_TITLE_BAR_END + window.elements[i].y);
            printStringAt(window.gi,window.elements[i].data);
        }
        if(window.elements[i].type==WINDOW_MANAGER_COMPONENT_BUTTON){
            if(window.focuslocation==i){
                for(int x = 0 ; x < (strlen(window.elements[i].data)*8) ; x++){
                    for(int y = 0 ; y < 15 ; y++){
                        draw_pixel_at_buffer(window.gi,window.elements[i].x + x ,WINDOW_MANAGER_WINDOW_TITLE_BAR_END + window.elements[i].y + y, 0xFF0000FF);
                    }
                }
            }
            move_text_pointer_buffer(window_id,window.elements[i].x,WINDOW_MANAGER_WINDOW_TITLE_BAR_END + window.elements[i].y);
            printStringAt(window.gi,window.elements[i].data);
        }
    }
    // print screen
    for(int y = 0 ; y < window.gi->Height ; y++){
        for(int x = 0 ; x < window.gi->Width ; x++){
            draw_pixel_at(window.x + x,window.y + y,get_pixel_at_buffer(window.gi,x,y));
        }
    }
    // top
    for(int x = 0 ; x < window.gi->Width ; x++){
        draw_pixel_at(window.x + x,window.y,WINDOW_MANAGER_WINDOW_BORDER_COLOR);
        draw_pixel_at(window.x + x,window.y+WINDOW_MANAGER_WINDOW_TITLE_BAR_END,WINDOW_MANAGER_WINDOW_BORDER_COLOR);
    }
    // left
    for(int y = 0 ; y < window.gi->Height ; y++){
        draw_pixel_at(window.x + 0,window.y + y,WINDOW_MANAGER_WINDOW_BORDER_COLOR);
    }
    // right
    for(int y = 0 ; y < window.gi->Height ; y++){
        draw_pixel_at(window.x + window.gi->Width,window.y + y,WINDOW_MANAGER_WINDOW_BORDER_COLOR);
    }
    // bottom
    for(int x = 0 ; x < window.gi->Width ; x++){
        draw_pixel_at(window.x + x,window.y + window.gi->Height,WINDOW_MANAGER_WINDOW_BORDER_COLOR);
    }
}

void window_manager_focus_loop(int direction){
    nogmaals:
    for(int i = 0 ; i < window_manager_window_count ; i++){
        SWindow window = windows[i];
        if(window.is_used&&window.is_active){
            if(direction==1&&window.is_active){
                if(window.focuslocation<(window.inner_elements_count-1)){
                    windows[i].focuslocation++;
                }else{
                    windows[i].focuslocation = 0;
                }
            }else if(direction==2&&window.is_active){
                if(window.focuslocation>0){
                    windows[i].focuslocation--;
                }else{
                    windows[i].focuslocation = (window.inner_elements_count-1);
                }
            }
            if(windows[i].elements[windows[i].focuslocation].can_be_selected==0){
                if(direction==2&&windows[i].focuslocation==0){
                    windows[i].focuslocation = window.inner_elements_count;
                }
                if(direction==1&&windows[i].focuslocation==(window.inner_elements_count-1)){
                    windows[i].focuslocation = -1;
                }
                goto nogmaals;
            }
        }
    }
}

int window_manager_poll_event(){
    window_manager_focus_loop(1);
    // wait for input
    input_again:
    unsigned char z = getch(1) & 0x000000FF;
    if(!(z==0xCD||z==0xCC||z==0xCB||z==0xCE||z=='\n')){
        goto input_again;
    }
    // handle state
    // - right
    if(z==0xCD||z==0xCB){
        window_manager_focus_loop(1);
    }
    // - left
    if(z==0xCC||z==0xCE){
        window_manager_focus_loop(2);
    }
    // - ok
    if(z=='\n'){
        for(int i = 0 ; i < window_manager_window_count ; i++){
            SWindow window = windows[i];
            if(window.is_active){
                return window.focuslocation;
            }
        }
    }
    goto input_again;
}

void window_manager_clear_window(int window_id){
    windows[window_id].focuslocation = 0;
    for(int i = 0 ; i < windows[window_id].inner_elements_count ; i++){
        windows[window_id].elements[i].can_be_selected = 0;
        windows[window_id].elements[i].data = 0;
        windows[window_id].elements[i].type = 0;
        windows[window_id].elements[i].x = 0;
        windows[window_id].elements[i].y = 0;
    }
    windows[window_id].inner_elements_count = 0;
    windows[window_id].is_active = 0;
    windows[window_id].is_used = 0;
    windows[window_id].title = 0;
    windows[window_id].x = 0;
    windows[window_id].y = 0;
    window_manager_window_count--;
}

int window_manager_create_confirm_box(char* message){
    window_manager_set_enabled(1);
    int window_id = window_manager_create_window("Question");
    window_manager_add_element(window_id,WINDOW_MANAGER_COMPONENT_LABEL,10,10,message);
    window_manager_add_element(window_id,WINDOW_MANAGER_COMPONENT_BUTTON,10,30,"OK");
    window_manager_add_element(window_id,WINDOW_MANAGER_COMPONENT_BUTTON,50,30,"CANCEL");
    int res = window_manager_poll_event();
    window_manager_clear_window(window_id);
    return res;
}