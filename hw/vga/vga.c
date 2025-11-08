#include "../../arch/boot/io.h"
#include "../screen/screen.h"
#include "vga.h"

volatile uint16_t	*vga_buffer = (volatile uint16_t *)VGA_MEM;
volatile uint8_t	vga_color = 0x07;

void	vga_print(const char *s) // vga
{
	size_t i;

	i = 0;
	while (s[i])
	{
		putchar(s[i]);
		i++;
	}
}

void	read_vga(char *out)
{
	int	i;
	int	x;

	out[0] = '\0';
	i = (cursor_y * VGA_WIDTH);
	x = 0;
	while (x < VGA_WIDTH)
	{
		out[x] = (char)(vga_buffer[i] & 0xFF);
		i++;
		x++;
	}
	out[x] = '\0';
}

static inline void	vga_put_entry_at(char c, uint8_t color, size_t x, size_t y)
{
	volatile uint16_t	*vga;

	vga = (uint16_t *)VGA_MEM;
	vga[y * VGA_WIDTH + x] = ((uint16_t)color << 8) | (uint8_t)c;
}

void	vga_update_hw_cursor(void)
{
	uint16_t	pos;

	pos = (uint16_t)(cursor_y * VGA_WIDTH + cursor_x);
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF)); // low
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF)); // high
}

void	vga_disable_cursor(void)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void	vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	uint8_t	cur_start;
	uint8_t	cur_end;

	outb(0x3D4, 0x0A);
	cur_start = inb(0x3D5);
	outb(0x3D5, (cur_start & 0xC0) | (cursor_start & 0x1F));
	outb(0x3D4, 0x0B);
	cur_end = inb(0x3D5);
	outb(0x3D5, (cur_end & 0xE0) | (cursor_end & 0x1F));
}

void	scroll(void)
{
	for (size_t y = 1; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			vga_buffer[((y - 1) * VGA_WIDTH) + x] = vga_buffer[(y * VGA_WIDTH)
				+ x];
		}
	}
	for (size_t x = 0; x < VGA_WIDTH; x++)
	{
		vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH
			+ x] = (uint16_t)' ' | (uint16_t)vga_color << 8;
	}
	cursor_y = VGA_HEIGHT - 1;
}

void	shift_right_line(void)
{
	size_t	start_pos;
	size_t	line_start;
	size_t	line_end;

	start_pos = cursor_y * VGA_WIDTH + cursor_x;
	line_start = cursor_y * VGA_WIDTH;
	line_end = line_start + VGA_WIDTH - 1;
	if (start_pos > line_end)
		return ;
	for (size_t i = line_end; i > start_pos; --i)
		vga_buffer[i] = vga_buffer[i - 1];
	vga_buffer[start_pos] = (uint16_t)(vga_color << 8);
}

void	shift_left_line(void)
{
	int			start_pos;
	int			end;
	static int	i = 1;

	start_pos = cursor_y * VGA_WIDTH + cursor_x;
	end = (1 + cursor_y) * VGA_WIDTH - 1;
	if (cursor_x != 0)
		i = 1;
	while (start_pos < end && i != -1)
	{
		vga_buffer[start_pos] = vga_buffer[start_pos + 1];
		start_pos++;
	}
	vga_buffer[(1 + cursor_y) * VGA_WIDTH
		- 1] = (uint16_t)' ' | (uint16_t)vga_color << 8;
	i = cursor_x - 1;
}

void	putchar(char c)
{
	if (c == '\n')
	{
		cursor_x = 0;
		cursor_y++;
	}
	else if (c == '\b')
	{
		if (cursor_x == VGA_WIDTH - 1)
			vga_buffer[cursor_y * VGA_WIDTH
				+ cursor_x] = ' ' | (uint16_t)vga_color << 8;
		else
			shift_left_line();
		if (cursor_x > 0)
			cursor_x--;
	}
	else
	{
		vga_buffer[cursor_y * VGA_WIDTH
			+ cursor_x] = (uint16_t)c | (uint16_t)vga_color << 8;
		cursor_x++;
		if (cursor_x == VGA_WIDTH)
		{
			cursor_x = 0;
			cursor_y++;
		}
	}
	if (cursor_y >= VGA_HEIGHT)
		scroll();
	vga_update_hw_cursor();
}
