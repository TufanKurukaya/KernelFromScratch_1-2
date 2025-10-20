#ifndef SCREEN_H
#define SCREEN_H

# include <stddef.h>
# include <stdint.h>
# include "vga.h"

typedef struct s_screen
{
    uint16_t buffer[VGA_WIDTH * VGA_HEIGHT];
    size_t cursor_pos_x;
    size_t cursor_pos_y;
} t_screen;


void screen_switch(int index);
void get_cursor_pos(size_t *x, size_t *y);
void set_cursor_pos(size_t x ,size_t y);
void init_screen();

#endif
