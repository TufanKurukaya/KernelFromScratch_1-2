#include "inc/screen.h"

extern volatile uint16_t	*vga_buffer;
extern volatile uint8_t		vga_color;
t_screen					screens[3] = {0};
int							current_screen = 0;

void	screen_put_string(size_t index, const char *str, size_t x, size_t y)
{
	size_t	pos;

	if (x > VGA_WIDTH || y > VGA_HEIGHT)
		return ;
	pos = y * VGA_HEIGHT + x;
	for (size_t i = 0; str[i]; i++)
		screens[index].buffer[pos
			+ i] = str[i] | (uint16_t)screens[index].color << 8;
}

void	init_screen(void)
{
	char	str[4] = "[ ]";

	screens[0].color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
	screens[1].color = VGA_COLOR(VGA_RED, VGA_BLACK);
	screens[2].color = VGA_COLOR(VGA_GREEN, VGA_BLACK);
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < VGA_WIDTH * VGA_HEIGHT; j++)
		{
			screens[i].buffer[j] = (uint16_t)screens[i].color << 8 | ' ';
		}
		str[1] = i + '1';
		screen_put_string(i, str, 10, 0);
	}
	for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++)
		vga_buffer[i] = screens[0].buffer[i];
}

void	screen_switch(int index)
{
	if (index == current_screen || index > 2)
		return ;
	// current screen save
	for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		screens[current_screen].buffer[i] = vga_buffer[i];
	get_cursor_pos(&screens[current_screen].cursor_pos_x,
		&screens[current_screen].cursor_pos_y);
	screens[current_screen].color = vga_color;
	// put new screen
	for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		vga_buffer[i] = screens[index].buffer[i];
	set_cursor_pos(screens[index].cursor_pos_x, screens[index].cursor_pos_y);
	vga_color = screens[index].color;
	current_screen = index;
	vga_update_hw_cursor();
}
