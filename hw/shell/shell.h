#ifndef SHELL_H
# define SHELL_H

void	command_enter(void);
void	process_command(char *vga_buf);
void	reboot(void);
void	halt(void);
void	print_stack(void);
void	cmd_clear(void);
void	cmd_help(void);

#endif