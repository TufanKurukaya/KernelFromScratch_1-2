#include "keyboard.h"
#include "../vga/vga.h"
#include "../screen/screen.h"

typedef void			(*f_key_handler_t)(void);
f_key_handler_t	f_keys[12] = {f1_handler, f2_handler, f3_handler,
		f4_handler, f5_handler, f6_handler, f7_handler, f8_handler, f9_handler,
		f10_handler, NULL, NULL};

static uint8_t				fg_index = 0;

static const uint8_t		fg_cycle[] = {VGA_WHITE, VGA_LIGHT_GREY, VGA_YELLOW,
			VGA_LIGHT_RED, VGA_LIGHT_GREEN, VGA_LIGHT_CYAN, VGA_LIGHT_BLUE,
			VGA_LIGHT_MAGENTA, VGA_MAGENTA, VGA_RED, VGA_GREEN, VGA_CYAN,
			VGA_BLUE, VGA_BROWN, VGA_DARK_GREY};


// F-key handler functions
void	f1_handler(void)
{
	screen_switch(0);
}

void	f2_handler(void)
{
	screen_switch(1);
}

void	f3_handler(void)
{
	screen_switch(2);
}

void	f4_handler(void)
{
	fg_index = (fg_index + 1) % (sizeof(fg_cycle) / sizeof(fg_cycle[0]));
	vga_color = VGA_COLOR(fg_cycle[fg_index], VGA_BLACK);
	draw_color_indicator();
}

void	f5_handler(void)
{
	screen_reset_active(-1);
	screen_apply_active();
}

void	f6_handler(void)
{
}

void	f7_handler(void)
{
}

void	f8_handler(void)
{
}

void	f9_handler(void)
{
}

void	f10_handler(void)
{
}
