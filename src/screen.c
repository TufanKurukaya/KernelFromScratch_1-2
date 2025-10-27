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
	char	*str = "42        This Screen [ ]";

	screens[0].default_color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
	screens[1].default_color = VGA_COLOR(VGA_RED, VGA_BLACK);
	screens[2].default_color = VGA_COLOR(VGA_GREEN, VGA_BLACK);

	screens[0].color = screens[0].default_color;
	screens[1].color = screens[1].default_color;
	screens[2].color = screens[2].default_color;
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < VGA_WIDTH * VGA_HEIGHT; j++)
		{
			screens[i].buffer[j] = (uint16_t)screens[i].color << 8 | ' ';
		}
		str[23] = i + '1';
		screen_put_string(i, str, 0, 0);
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

void	add_history(char *in)
{
	int i;

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

void	add_history_entry()
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

void	change_cursor_pos()
{
	size_t	y = 0;
	get_cursor_pos((size_t *)"", &y);
	int	i = ((y + 1) * VGA_WIDTH);
	int x = (y * VGA_WIDTH);

	while (i != x)
	{
		if ((vga_buffer[i] & 0xFF) != ' ')
			break;
		i--;
	}
	set_cursor_pos((i % 80) + 1, y);
}

void	display_history_entry(char *in)
{
	size_t	y = 0;
	int		x = 0;
	get_cursor_pos((size_t *)"", &y);
	int 	i = (y * VGA_WIDTH);

	while (x < VGA_WIDTH)
	{
		vga_buffer[i] = (uint16_t)(in[x] ? in[x] : ' ') | ((uint16_t)vga_color << 8);
		i++;
		x++;
	}
	change_cursor_pos();
}
//set
void	navigate_history(char direction)
{
	if (direction == 72) // U
	{
		if (history.cmds[history.cursor][0] == '\0')
            return;
		if (history.edit_backup[0] == '\0')
			read_vga((char *)history.edit_backup);
		display_history_entry((char *)history.cmds[history.cursor]);
		history.cursor = (history.cursor == 0) ? HISTORY_MAX - 1 : history.cursor - 1;
	}
	else
	{
		int next_cursor = (history.cursor + 1) % HISTORY_MAX;

		if (next_cursor == history.index)
		{
			if (history.edit_backup[0] == '\0')
				return;
			display_history_entry((char *)history.edit_backup);
			history.edit_backup[0] = '\0';
			return;
		}
		history.cursor = next_cursor;
		display_history_entry((char *)history.cmds[history.cursor]);
	}
}