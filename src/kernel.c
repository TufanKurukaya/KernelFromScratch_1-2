// src/kernel.c
#include "idt.h"
#include "pic.h"
#include "vga.h"
#include <stddef.h>
#include <stdint.h>

// Function prototypes
void				vga_print(const char *s);
int					u32_to_hex(uint32_t n, char *out);
int					u32_to_dec(uint32_t v, char *out);
void				keyboard_handler(void);

uint8_t             key_flag = 0;

static uint16_t *const vga_buffer = (uint16_t *)VGA_MEM;
static size_t		cursor_x = 0, cursor_y = 0;
static uint8_t		vga_color = 0x07;
extern void			isr_irq1_stub(void);
extern void			default_exception_stub(void);
static inline void	putchar(char c);

// Basit US QWERTY scancode tablosu (sadece küçük harfler ve rakamlar)
static const char	scancode_table[128] = {
	0,
	27,
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	'0',
	'-',
	'=',
	'\b', /* 0x0E = backspace */
	'\t',

	/* 0x0F = tab */
	'q',
	'w',
	'e',
	'r',
	't',
	'y',
	'u',
	'i',
	'o',
	'p',
	'[',
	']',
	'\n',
	/* 0x1C = Enter */
	0,

	/* 0x1D = Ctrl */
	'a',
	's',
	'd',
	'f',
	'g',
	'h',
	'j',
	'k',
	'l',
	';',
	'\'',
	'`',
	0,
	/* 0x2A = Left shift */
	'\\',
	'z',
	'x',
	'c',
	'v',
	'b',
	'n',
	'm',
	',',
	'.',
	'/',
	0,
	/* 0x36 = Right shift */
	'*',
	0,

	/* 0x38 = Alt */
	' ',

	/* 0x39 = Space */
	0,

	/* Caps lock */
	// buradan sonrası F1-F12 ve özel tuşlar
};

char	scancode_to_char(unsigned char scancode)
{
	if (scancode > 127)
		return (0);
	return (scancode_table[scancode]);
}

unsigned char	inb(unsigned short port)
{
	unsigned char	ret;

	asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
	return (ret);
}

void	keyboard_handler(void)
{
	unsigned char	scancode;
	char			c;
	
	scancode = inb(0x60);
	if (scancode & 0x80)
		return ;
	c = scancode_to_char(scancode);
	if ((c < 127 && c > 31) || (c <= 13 &&  c >= 9))
		putchar(c);
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
			+ x] = (uint16_t)' ' | (uint16_t)VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK) << 8;
	}
	cursor_y = VGA_HEIGHT - 1;
}

static inline void	putchar(char c)
{
	if (c == '\n')
	{
		cursor_x = 0;
		cursor_y++;
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

void	kernel_main(uint32_t magic, uint32_t addr)
{
	char	str[12] = {0};

	// Önce interrupt'ları kapat
	__asm__ __volatile__("cli");
	vga_clear(VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
	vga_color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
	vga_print("Kernel starting...\n");
	if (magic != 0x2BADB002)
	{
		vga_print("Bad multiboot magic\n");
		while (1)
			__asm__ __volatile__("hlt");
	}

	idt_init();
	pic_remap(0x20, 0x28);
	pic_mask_all_irqs();
	idt_set_gate(0x21, (uint32_t)isr_irq1_stub, 0x08, 0x8E);
	pic_unmask_irq1();
	__asm__ __volatile__("sti");
	for (;;)
		__asm__ __volatile__("hlt");
}
