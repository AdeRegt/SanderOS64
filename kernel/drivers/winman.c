#include "../include/winman.h"
#include "../include/graphics.h"
#include "../include/memory.h"

int window_manager_window_count = 0;
SWindow windows[10];

int window_manager_create_window(char* title){
    int oldcount = window_manager_window_count;
    window_manager_window_count++;
    windows[oldcount].title = title;
    windows[oldcount].x = WINDOW_MANAGER_WINDOW_INITIAL_X;
    windows[oldcount].y = WINDOW_MANAGER_WINDOW_INITIAL_Y;
    windows[oldcount].gi = new_instance_of_graphics_info(WINDOW_MANAGER_WINDOW_INITIAL_WIDTH,WINDOW_MANAGER_WINDOW_INITIAL_HEIGHT);
    clear_screen_at_buffer(windows[oldcount].gi,WINDOW_MANAGER_WINDOW_COLOR);
    return oldcount;
}

int window_manager_add_element(int window_id,int type,int x,int y,void* data){
    int innere = windows[window_id].inner_elements_count;
    windows[window_id].elements[innere].type = type;
    windows[window_id].elements[innere].x = x;
    windows[window_id].elements[innere].y = y;
    windows[window_id].elements[innere].data = data;
    windows[window_id].inner_elements_count++;
    return innere;
}


void move_text_pointer_buffer(int window_id,unsigned long x,unsigned long y){
    move_text_pointer_at_buffer(windows[window_id].gi,x,y);
}

void window_manager_draw_window(SWindow window,int window_id){
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

int window_manager_poll_event(){
    for(int i = 0 ; i < window_manager_window_count ; i++){
        SWindow window = windows[i];
        window_manager_draw_window(window,i);
    }
    for(;;);
}

int window_manager_create_confirm_box(char* message){
    int window_id = window_manager_create_window("Question");
    window_manager_add_element(window_id,WINDOW_MANAGER_COMPONENT_LABEL,10,10,message);
    window_manager_add_element(window_id,WINDOW_MANAGER_COMPONENT_BUTTON,10,20,"OK");
    window_manager_add_element(window_id,WINDOW_MANAGER_COMPONENT_BUTTON,50,20,"CANCEL");
    window_manager_poll_event();
    for(;;);
}