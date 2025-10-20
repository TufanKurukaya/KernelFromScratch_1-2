// src/kernel.c
#include "inc/gdt.h"
#include "inc/idt.h"
#include "inc/io.h"
#include "inc/pic.h"
#include "inc/printf.h"
#include "inc/utils.h"
#include "inc/vga.h"
#include "inc/screen.h"
#include "inc/keyboard.h"
#include <stddef.h>
#include <stdint.h>
#define ALT_CTRL 0x3

void				vga_print(const char *s);
int					u32_to_dec(uint32_t v, char *out);
void				keyboard_handler(void);
extern repeat_state_t repeat;

volatile uint16_t *vga_buffer = (volatile uint16_t *)VGA_MEM;
static size_t	cursor_x = 0, cursor_y = 0;
static uint8_t	vga_color = 0x07;
extern void			isr_irq1_stub(void);
void				putchar(char c);

static const char	scancode_table[128] = {
	0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-',  '=',
		 '\b', '\t',

	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']', '\n', 0,

	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,    '\\',
		'z',  'x',  'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,

	' ',

	0,

};

static const char	scancode_table_shifted[128] = {
	0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(',  ')', '_',  '+',
		'\b', '\t',

	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{',  '}', '\n', 0,

	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,    '|', 'Z',
		 'X',  'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0,

	' ',

	0,

};

void	print_bits(unsigned char bits)
{
	for (uint16_t i = 0; i < 8; i++)
	{
		if (bits & 0x80)
			putchar('1');
		else
			putchar('0');
		bits = bits << 1;
	}
}
void set_cursor_pos(size_t x ,size_t y)
{
	if (x >= VGA_WIDTH) x = VGA_WIDTH - 1;
	if (y >= VGA_HEIGHT) y = VGA_HEIGHT - 1;
	cursor_x = x;
	cursor_y = y;
}

void get_cursor_pos(size_t *x, size_t *y)
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


char scancode_to_char(uint8_t scancode)
{
    if (scancode > 127)
        return 0;

    int shift = is_key_down(0x2A) || is_key_down(0x36);
    int caps = is_key_toggled(0x3A);

    if ((shift && !caps) || (!shift && caps))
        return scancode_table_shifted[scancode];
    else
        return scancode_table[scancode];
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

void	vga_disable_cursor(void)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
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

static inline void	vga_put_entry_at(char c, uint8_t color, size_t x, size_t y)
{
	volatile uint16_t	*vga;

	vga = (uint16_t *)VGA_MEM;
	vga[y * VGA_WIDTH + x] = ((uint16_t)color << 8) | (uint8_t)c;
}

void	handeler_arrow(char c) // TODO: Bukadar serbest harekete icin verilmeyecek
{
	if (c == 77)
	{
		if (cursor_x < VGA_WIDTH - 1)
			cursor_x++;
	}
	else if (c == 75)
	{
		if (cursor_x > 0)
			cursor_x--;
	}
	else if (c == 72)
	{
		if (cursor_y > 0)
			cursor_y--;
	}
	else
	{
		if (cursor_y < VGA_HEIGHT - 1)
			cursor_y++;
	}
	vga_update_hw_cursor();
}


void	shift_right_line(void)
{
	size_t start_pos = cursor_y * VGA_WIDTH + cursor_x;
	size_t line_start = cursor_y * VGA_WIDTH;
	size_t line_end = line_start + VGA_WIDTH - 1;

	if (cursor_x >= VGA_WIDTH)
		return;
	if (start_pos > line_end)
		return;

	for (size_t i = line_end; i > start_pos; --i)
		vga_buffer[i] = vga_buffer[i - 1];

	vga_buffer[start_pos] = (uint16_t)(vga_color << 8);
}

void	shift_left_line(void)
{
	size_t line_start = cursor_y * VGA_WIDTH;
	size_t line_end = line_start + VGA_WIDTH - 1;
	size_t start_pos = line_start + cursor_x;

	if (cursor_x >= VGA_WIDTH)
		return;
	if (start_pos > line_end)
		return;

	if (start_pos < line_end)
	{
		for (size_t i = start_pos; i < line_end; ++i)
			vga_buffer[i] = vga_buffer[i + 1];
	}

	vga_buffer[line_end] = (uint16_t)(vga_color << 8);
}
void	keyboard_handler(void)
{
    uint8_t scancode = inb(0x60);
    keyboard_isr(scancode);
}

void	scroll(void)
{
	for (size_t y = 1; y < VGA_HEIGHT; y++)
	{
		for (size_t x = 0; x < VGA_WIDTH; x++)
		{
			vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
		}
	}
	for (size_t x = 0; x < VGA_WIDTH; x++)
	{
		vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH
			+ x] = (uint16_t)0 | (uint16_t)vga_color << 8;
	}
	cursor_y = VGA_HEIGHT - 1;
}

void	putchar(char c)
{
	if (c == '\n')
	{
		cursor_x = 0;
		cursor_y++;
	}
	else if (c == '\t')
		cursor_x = (cursor_x + 4) & ~(size_t)3;
	else if (c == '\b')
	{
		if (cursor_x > 0)
		{
			cursor_x--;
			shift_left_line();
		}
		else if (cursor_y > 0)
		{
			cursor_y--;
			size_t x = 0;
			while (x < VGA_WIDTH && (vga_buffer[cursor_y * VGA_WIDTH + x] & 0xFF) != 0)
				x++;
			cursor_x = (x < VGA_WIDTH) ? x : (VGA_WIDTH - 1);
			if (cursor_x > 0)
			{
				cursor_x--;
				shift_left_line();
			}
		}
	}
	else
	{
		vga_buffer[cursor_y * VGA_WIDTH
			+ cursor_x] = (uint16_t)c | (uint16_t)vga_color << 8;
		cursor_x++;
		if (cursor_x >= VGA_WIDTH)
		{
			cursor_x = 0;
			cursor_y++;
		}
	}
	if (cursor_y >= VGA_HEIGHT)
		scroll();
	vga_update_hw_cursor();
}

void	vga_print(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		putchar(s[i]);
		i++;
	}
}

int	u32_to_dec(uint32_t v, char *out)
{
	char	tmp[12];
	int		n;

	uint32_t q, r;
	if (v == 0)
	{
		out[0] = '0';
		out[1] = 0;
		return (1);
	}
	n = 0;
	while (v)
	{
		q = v / 10, r = v % 10;
		tmp[n++] = (char)('0' + r);
		v = q;
	}
	for (int i = 0; i < n; i++)
		out[i] = tmp[n - 1 - i];
	out[n] = 0;
	return (n);
}

void	kernel_main(uint32_t magic)
{
	__asm__ __volatile__("cli");
	vga_color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
	vga_clear(VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
	if (magic != 0x2BADB002)
	{
		vga_print("Bad multiboot magic\n");
		while (1)
			__asm__ __volatile__("hlt");
	}
	gdt_init();
	idt_init();
	pic_remap(0x20, 0x28);
	pic_mask_all_irqs();
	idt_set_gate(0x21, (uint32_t)isr_irq1_stub, KERNEL_CS, 0x8E);
	pic_unmask_irq1();
	__asm__ __volatile__("sti");
	init_screen();
	vga_enable_cursor(0, 15);
	vga_update_hw_cursor();
	input_command_t cmd;
	for (;;)
	{
		while(input_poll(&cmd))
		{
			update_key_state(&cmd);
			if (cmd.type == 0) // pressed (0 is pressed, 1 is released)
			{
                uint8_t code = cmd.scancode;
                if (code == 0x3B) {
                    screen_switch(0);
                    vga_update_hw_cursor();
                    continue;
                } else if (code == 0x3C) {
                    screen_switch(1);
                    vga_update_hw_cursor();
                    continue;
                }
    
                // --- Ok tuşları ---
                if (code == 77 || code == 75 || code == 72 || code == 80) {
                    handeler_arrow(code);
                    continue;
                }
                if (code == 0x53) {
                    if ((vga_buffer[cursor_y * VGA_WIDTH + cursor_x] & 0xFF) != 0)
                        shift_left_line();
                    continue;
                }
    
                // --- Normal karakter ---
                char c = scancode_to_char(code);
                if (c && c < 127) {
                    if (c != '\n' && c != '\b' &&
                        cursor_x < (VGA_WIDTH - 1) &&
                        (get_cursor_value() & 0xFF) != 0)
                        shift_right_line();
                    putchar(c);
                }
    
			}
		}
		
		handle_key_repeat();
		
		__asm__ __volatile__("hlt");
	}
}
