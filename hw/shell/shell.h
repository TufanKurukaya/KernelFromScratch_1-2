#ifndef SHELL_H
# define SHELL_H

void	command_enter(void);
void	process_command(char *vga_buf);
void    reboot();
void    halt();
void    print_stack();
void    cmd_clear();
void    cmd_help();

#endif