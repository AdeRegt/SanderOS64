#include "graphics.h"

#define WINDOW_MANAGER_WINDOW_COLOR 0xFFFFFFFF
#define WINDOW_MANAGER_WINDOW_TITLE_LEFT_OFFSET 10
#define WINDOW_MANAGER_WINDOW_TITLE_RIGHT_OFFSET 30
#define WINDOW_MANAGER_WINDOW_INITIAL_X 100
#define WINDOW_MANAGER_WINDOW_INITIAL_Y 100
#define WINDOW_MANAGER_WINDOW_INITIAL_WIDTH 300
#define WINDOW_MANAGER_WINDOW_INITIAL_HEIGHT 300
#define WINDOW_MANAGER_WINDOW_TITLE_BAR_END 15
#define WINDOW_MANAGER_WINDOW_BORDER_COLOR 0xFF00FF00
#define WINDOW_MANAGER_COMPONENT_LABEL 1
#define WINDOW_MANAGER_COMPONENT_BUTTON 2

typedef struct {
    int x;
    int y;
    int type;
    void *data;
    int can_be_selected;
} __attribute__ ((packed)) SElement;

typedef struct {
    char* title;
    int x;
    int y;
    int inner_elements_count;
    int focuslocation;
    SElement elements[100];
    GraphicsInfo *gi;
    int is_active;
    int is_used;
} __attribute__ ((packed)) SWindow;

int window_manager_create_window(char* title);
int window_manager_create_confirm_box(char* message);
int window_manager_poll_event();
int window_manager_add_element(int window_id,int type,int x,int y,void* data);
void move_text_pointer_buffer(int window_id,unsigned long x,unsigned long y);
void window_manager_clear_window(int window_id);
void window_manager_interrupt();
void window_manager_draw_window(SWindow window,int window_id);
int window_manager_get_retention_time();
int window_manager_is_enabled();
void window_manager_set_enabled(int a);
SWindow *getWindowFromId(int id);