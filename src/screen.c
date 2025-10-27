#include "inc/screen.h"

extern volatile uint16_t	*vga_buffer;
extern volatile uint8_t		vga_color;
t_screen					screens[3] = {0};
int							current_screen = 0;

void		draw_color_indicator(void)
{
	size_t	cx, cy;
	const char *label = "[CLR]";
	size_t	len = 5;
	size_t	x = (VGA_WIDTH >= len) ? (VGA_WIDTH - len) : 0;

	get_cursor_pos(&cx, &cy);
	for (size_t i = 0; i < len; ++i)
		vga_buffer[0 * VGA_WIDTH + x + i] = ((uint16_t)vga_color << 8) | (uint8_t)label[i];
	set_cursor_pos(cx, cy);
	vga_update_hw_cursor();
}


void	screen_put_string(size_t index, const char *str, size_t x, size_t y)
{
	size_t	pos;
	size_t	i;
	size_t	total;

	if (x > VGA_WIDTH || y > VGA_HEIGHT)
		return ;
	pos = y * VGA_WIDTH + x;
	total = VGA_WIDTH * VGA_HEIGHT;
	i = 0;
	while (str[i] && (pos + i) < total)
	{
		screens[index].buffer[pos + i]
			= str[i] | (uint16_t)screens[index].color << 8;
		i++;
	}
	pos += i;
	if (pos >= total)
		pos = total - 1;
	screens[index].cursor_pos_y = pos / VGA_WIDTH;
	screens[index].cursor_pos_x = pos % VGA_WIDTH;
}

void	init_screen(void)
{
	char	*str = "This Screen [ ]";

	// Set default colors per screen (distinct defaults)
	screens[0].default_color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
	screens[1].default_color = VGA_COLOR(VGA_RED, VGA_BLACK);
	screens[2].default_color = VGA_COLOR(VGA_GREEN, VGA_BLACK);
	// Initialize current colors to default
	screens[0].color = screens[0].default_color;
	screens[1].color = screens[1].default_color;
	screens[2].color = screens[2].default_color;
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < VGA_WIDTH * VGA_HEIGHT; j++)
		{
			screens[i].buffer[j] = (uint16_t)screens[i].color << 8 | ' ';
		}
		str[13] = i + '1';
		screen_put_string(i, str, 10, 0);
		screens[i].cursor_pos_y += 1;
		screens[i].cursor_pos_x = 0;
	
	}
	for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++)
		vga_buffer[i] = screens[0].buffer[i];
	vga_color = screens[0].color;
	set_cursor_pos(screens[0].cursor_pos_x,screens[0].cursor_pos_y);
	draw_color_indicator();
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
	screens[index].color = screens[index].default_color;
	current_screen = index;
	vga_update_hw_cursor();
	draw_color_indicator();
}

void	screen_reset_active(void)
{
	uint8_t color = screens[current_screen].default_color;
	screens[current_screen].color = color;
	for (size_t j = 0; j < VGA_WIDTH * VGA_HEIGHT; j++)
		screens[current_screen].buffer[j] = ((uint16_t)color << 8) | ' ';
	char header[] = "This Screen [ ]";
	header[13] = (char)('1' + current_screen);
	screen_put_string(current_screen, header, 10, 0);
	screens[current_screen].cursor_pos_y = 1;
	screens[current_screen].cursor_pos_x = 0;
}

void	screen_apply_active(void)
{
	for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		vga_buffer[i] = screens[current_screen].buffer[i];
	vga_color = screens[current_screen].color;
	set_cursor_pos(screens[current_screen].cursor_pos_x,
		screens[current_screen].cursor_pos_y);
	vga_update_hw_cursor();
	draw_color_indicator();
}
