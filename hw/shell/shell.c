#include "../vga/vga.h"
#include "../screen/screen.h"
#include "../../lib/printf.h"
#include "../../lib/utils.h"
#include "shell.h"

void	process_command(char *vga_buf)
{
	char	command[VGA_WIDTH + 1];

	trim(command, vga_buf);
	if (!strcmp(command, "help"))
		cmd_help();
	else if (!strcmp(command, "clear"))
		cmd_clear();
	else if (!strcmp(command, "stack"))
		print_stack();
	else if (!strcmp(command, "reboot"))
		reboot();
	else if (!strcmp(command, "halt"))
		halt();
	else if (command[0] != '\0')
		printf("Unknown command: '%s'\nType 'help' for available commands.\n", command);
}

void	command_enter(void)
{
	char	vga_buf[VGA_WIDTH + 1];

	add_history_entry();
	read_vga(vga_buf);
	add_history(vga_buf);
	cursor_y++;
	cursor_x = 0;
	if (cursor_y == VGA_HEIGHT)
		scroll();
	process_command(vga_buf);
}