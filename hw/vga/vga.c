#include "vga.h"
#include "../screen/screen.h"
#include "../../arch/boot/io.h"

volatile uint16_t		*vga_buffer = (volatile uint16_t *)VGA_MEM;
volatile uint8_t		vga_color = 0x07;

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
