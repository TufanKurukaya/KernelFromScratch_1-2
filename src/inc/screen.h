#ifndef SCREEN_H
# define SCREEN_H

# include "vga.h"
# include <stddef.h>
# include <stdint.h>

typedef struct s_history
{
	char		up_history[(VGA_WIDTH * VGA_HEIGHT) / 2];
	char		down_history[(VGA_WIDTH * VGA_HEIGHT) / 2];
	char		cmd_histtory[1024];
}				t_history;

typedef struct s_screen
{
	uint16_t	buffer[VGA_WIDTH * VGA_HEIGHT];
	size_t		cursor_pos_x;
	size_t		cursor_pos_y;
	uint8_t		color;
}				t_screen;

void			screen_switch(int index);
void			get_cursor_pos(size_t *x, size_t *y);
void			set_cursor_pos(size_t x, size_t y);
void			init_screen(void);

#endif
