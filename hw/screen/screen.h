#ifndef SCREEN_H
# define SCREEN_H

# include "../vga/vga.h"
# include <stddef.h>
# include <stdint.h>

# define HISTORY_MAX 32
extern size_t		cursor_x;
extern size_t		cursor_y;

typedef struct s_history
{
	char			cmds[HISTORY_MAX][VGA_WIDTH + 1];
	int				index;
	char			edit_backup[VGA_WIDTH + 1];
	int				cursor;
}					t_history;

typedef struct s_screen
{
	uint16_t		buffer[VGA_WIDTH * VGA_HEIGHT];
	size_t			cursor_pos_x;
	size_t			cursor_pos_y;
	uint8_t			color;
	uint8_t			default_color;
	t_history		s_history;
}					t_screen;

extern t_history	history;

void				screen_switch(int index);
void				get_cursor_pos(size_t *x, size_t *y);
void				set_cursor_pos(size_t x, size_t y);
uint16_t			get_cursor_value(void);
void				init_screen(void);
void				draw_color_indicator(void);
void				screen_reset_active(int index);
void				screen_apply_active(void);
void				add_history(char *in);
void				add_history_entry(void);
void				navigate_history(char direction);
void				read_vga(char *out);

#endif
