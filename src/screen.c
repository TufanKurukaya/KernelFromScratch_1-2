#include "inc/screen.h"
extern volatile uint16_t *vga_buffer;
t_screen screens[3] = {0};
int current_screen = 0;
void screen_switch(int index)
{
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        screens[current_screen].buffer[i] = vga_buffer[i];
    get_cursor_pos(&screens[current_screen].cursor_pos_x ,&screens[current_screen].cursor_pos_y);

    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        vga_buffer[i] =  screens[index].buffer[i];
    set_cursor_pos(screens[index].cursor_pos_x, screens[index].cursor_pos_y);
    current_screen = index;
    vga_update_hw_cursor();
}