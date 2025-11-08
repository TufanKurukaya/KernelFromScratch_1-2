#include "../../lib/printf.h"
#include "../../lib/utils.h"
#include "../screen/screen.h"
#include "../vga/vga.h"
#include "shell.h"

unsigned int	djb_two(const char *str) // bernstein-djb2
{
	int i = 0;
	unsigned int hash = 1; // 5381

	while (str[i])
	{
		hash = ((hash << 5) + hash) + str[i]; // hash * 33 + str[i]
		i++;
	}
	return (hash);
}

void	process_command(char *vga_buf)
{
	char			command[VGA_WIDTH + 1];
	unsigned int	h;

	trim(command, vga_buf);
	h = djb_two(command);
	switch (h)
	{
	case 5037034: // help
		cmd_help();
		break ;
	case 160536072: // clear
		cmd_clear();
		break ;
	case 179794007: // stack
		print_stack();
		break ;
	case 1581359980: // reboot
		reboot();
		break ;
	case 5032682: // halt
		halt();
		break ;
	case 1: // '\0'
		break ;
	default: // asd
		printf("Unknown command: '%s'\nType 'help' for available commands.\n",
			command);
		break ;
	}
}

void	command_enter(void)
{
	char vga_buf[VGA_WIDTH + 1];

	add_history_entry();
	read_vga(vga_buf);
	add_history(vga_buf);
	cursor_y++;
	cursor_x = 0;
	if (cursor_y == VGA_HEIGHT)
		scroll();
	process_command(vga_buf);
}