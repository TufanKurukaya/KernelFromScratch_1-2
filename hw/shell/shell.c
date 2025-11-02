#include "../vga/vga.h"
#include "../screen/screen.h"
#include "../../lib/printf.h"
#include "../../lib/utils.h"

void	process_command(char *vga_buf)
{
	char	command[VGA_WIDTH + 1];

	trim(command, vga_buf);
	printf("TODO: Command processing not implemented yet");
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
	cursor_y++;
	cursor_x = 0;
}