#include "keyboard.h"
#include "../vga/vga.h"
#include "../screen/screen.h"
#include "../../lib/utils.h"

static const char		scancode_table[128] = {// keyboard
	0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-',  '=',
		 '\b', '\t',

	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']', '\n', 0,

	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,    '\\',
		'z',  'x',  'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0,

	' ',

	0,

};

static const char		scancode_table_shifted[128] = {// keyboard
	0,   27,  '!', '@', '#', '$', '%', '^', '&', '*', '(',  ')', '_',  '+',
		'\b', '\t',

	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{',  '}', '\n', 0,

	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0,    '|', 'Z',
		 'X',  'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0,

	' ',

	0,

};

char	scancode_to_char(uint8_t scancode)
{
	char	c_shitf;
	char	c;
	int		shift;
	int		caps;

	if (scancode > 127)
		return (0);
	c_shitf = scancode_table_shifted[scancode];
	c = scancode_table[scancode];
	shift = is_key_down(0x2A) || is_key_down(0x36);
	caps = is_key_toggled(0x3A);
	if (isalpha(c))
		return ((shift ^ caps) ? c_shitf : c);
	else
		return (shift ? c_shitf : c);
}

void	handeler_arrow(char c)
{
	switch (c)
	{
		case 77:
			if (cursor_x < VGA_WIDTH - 1)
				cursor_x++;
			break ;
		case 75:
			if (cursor_x > 0)
				cursor_x--;
			break ;
		default:
			navigate_history(c);
			break ;
	}
	vga_update_hw_cursor();
}
