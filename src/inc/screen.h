#ifndef SCREEN_H
# define SCREEN_H

# include "vga.h"
# include <stddef.h>
# include <stdint.h>

# define HISTORY_MAX 32

typedef struct s_history
{
	char    cmds[HISTORY_MAX][VGA_WIDTH];
	int     index;
	char    edit_backup[VGA_WIDTH];
}               t_history;

typedef struct s_screen
{
	uint16_t	buffer[VGA_WIDTH * VGA_HEIGHT];
	size_t		cursor_pos_x;
	size_t		cursor_pos_y;
	uint8_t		color;
	t_history	s_history;
}               t_screen;

void            screen_switch(int index);
void            get_cursor_pos(size_t *x, size_t *y);
void            set_cursor_pos(size_t x, size_t y);
void            init_screen(void);

#endif
