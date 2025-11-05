#include "screen.h"

extern volatile uint16_t	*vga_buffer;
extern volatile uint8_t		vga_color;
t_screen					screens[3] = {0};
int							current_screen = 0;
size_t						cursor_x = 0, cursor_y = 0;
volatile t_history			history = {.edit_backup[0] = '\0', .index = 0,
			.cursor = HISTORY_MAX - 1};

void	set_cursor_pos(size_t x, size_t y)
{
	if (x >= VGA_WIDTH)
		x = VGA_WIDTH - 1;
	if (y >= VGA_HEIGHT)
		y = VGA_HEIGHT - 1;
	cursor_x = x;
	cursor_y = y;
}

void	get_cursor_pos(size_t *x, size_t *y)
{
	if (x)
		*x = cursor_x;
	if (y)
		*y = cursor_y;
}

uint16_t	get_cursor_value(void)
{
	return (vga_buffer[cursor_y * VGA_WIDTH + cursor_x]);
}

void	draw_color_indicator(void)
{
	const char	*label = "[CLR]";
	size_t		len;
	size_t		x;

	len = 5;
	x = VGA_WIDTH - len;
	for (size_t i = 0; i < len; ++i)
		vga_buffer[0 * VGA_WIDTH + x
			+ i] = ((uint16_t)vga_color << 8) | (uint8_t)label[i];
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
		screens[index].buffer[pos
			+ i] = str[i] | (uint16_t)screens[index].color << 8;
		i++;
	}
	pos += i;
	if (pos >= total)
		pos = total - 1;
	screens[index].cursor_pos_y = pos / VGA_WIDTH;
	screens[index].cursor_pos_x = pos % VGA_WIDTH;
}

void	screen_reset_active(int index)
{
	char	*header;
	uint8_t	color;

	header = "42        This Screen [ ]";
	if (index == -1)
		index = current_screen;
	color = screens[index].default_color;
	screens[index].color = color;
	for (size_t j = 0; j < VGA_WIDTH * VGA_HEIGHT; j++)
		screens[index].buffer[j] = ((uint16_t)color << 8) | ' ';
	header[23] = (char)('1' + index);
	screen_put_string(index, header, 0, 0);
	screens[index].cursor_pos_y = 1;
	screens[index].cursor_pos_x = 0;
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

void	init_screen(void)
{
	screens[0].default_color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
	screens[1].default_color = VGA_COLOR(VGA_LIGHT_RED, VGA_BLACK);
	screens[2].default_color = VGA_COLOR(VGA_LIGHT_GREEN, VGA_BLACK);
	for (size_t i = 0; i < 3; i++)
	{
		screens[i].color = screens[i].default_color;
		screen_reset_active(i);
	}
	screen_apply_active();
}

void	screen_switch(int index)
{
	if (index == current_screen || index > 2)
		return ;
	for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		screens[current_screen].buffer[i] = vga_buffer[i];
	get_cursor_pos(&screens[current_screen].cursor_pos_x,
		&screens[current_screen].cursor_pos_y);
	screens[current_screen].color = vga_color;
	for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
		vga_buffer[i] = screens[index].buffer[i];
	set_cursor_pos(screens[index].cursor_pos_x, screens[index].cursor_pos_y);
	vga_color = screens[index].color;
	screens[index].color = screens[index].default_color;
	current_screen = index;
	vga_update_hw_cursor();
	draw_color_indicator();
}

void	add_history(char *in)
{
	int	i;

	i = 0;
	while (i < VGA_WIDTH)
	{
		history.cmds[history.index][i] = in[i];
		i++;
	}
	history.cmds[history.index][i] = '\0';
	history.index++;
	if (history.index == HISTORY_MAX)
		history.index = 0;
}

void	add_history_entry(void)
{
	static int	i = 0;

	history.cursor = history.index;
	history.edit_backup[0] = '\0';
	if (history.index == HISTORY_MAX - 1)
		history.cmds[0][0] = '\0';
	else
		history.cmds[history.index + 1][0] = '\0';
	while (i < HISTORY_MAX)
	{
		history.cmds[i][0] = '\0';
		i++;
	}
}

void	change_cursor_pos(void)
{
	size_t	y;
	int		i;
	int		x;

	y = 0;
	get_cursor_pos((void *)0, &y);
	i = ((y + 1) * VGA_WIDTH);
	x = (y * VGA_WIDTH);
	while (i != x)
	{
		if ((vga_buffer[i] & 0xFF) != ' ')
			break ;
		i--;
	}
	set_cursor_pos((i % VGA_WIDTH) + 1, y);
}

void	display_history_entry(char *in)
{
	size_t	y;
	int		x;
	int		i;

	y = 0;
	x = 0;
	get_cursor_pos((void *)0, &y);
	i = (y * VGA_WIDTH);
	while (x < VGA_WIDTH)
	{
		vga_buffer[i] = (uint16_t)(in[x] ? in[x] : ' ') | ((uint16_t)vga_color << 8);
		i++;
		x++;
	}
	change_cursor_pos();
}

void	navigate_history(char direction)
{
	int	next_cursor;

	if (direction == 72)
	{
		if (history.cmds[history.cursor][0] == '\0')
			return ;
		if (history.edit_backup[0] == '\0')
			read_vga((char *)history.edit_backup);
		display_history_entry((char *)history.cmds[history.cursor]);
		history.cursor = (history.cursor == 0) ? HISTORY_MAX
			- 1 : history.cursor - 1;
	}
	else
	{
		next_cursor = (history.cursor + 1) % HISTORY_MAX;
		if (next_cursor == history.index)
		{
			if (history.edit_backup[0] == '\0')
				return ;
			display_history_entry((char *)history.edit_backup);
			history.edit_backup[0] = '\0';
			return ;
		}
		history.cursor = next_cursor;
		display_history_entry((char *)history.cmds[history.cursor]);
	}
}
