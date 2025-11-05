// src/kernel.c
#include "../arch/gdt/gdt.h"
#include "../arch/idt/idt.h"
#include "../arch/pic/pic.h"
#include "../arch/boot/io.h"
#include "../hw/keyboard/keyboard.h"
#include "../lib/printf.h"
#include "../hw/screen/screen.h"
#include "../lib/utils.h"
#include "../hw/vga/vga.h"
#include "../hw/shell/shell.h"
#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "../arch/tss/tss.h"
extern void user_main();
__attribute__((aligned(16))) uint8_t _user_stack[4096];
uint32_t _user_stack_top = (uint32_t)_user_stack + sizeof(_user_stack);
static uint8_t kernel_stack[4096] __attribute__((aligned(16)));

void	kernel_main(uint32_t magic)
{
	input_command_t	cmd;
	uint8_t			code;
	char			c;
	int				idx;

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
    uint32_t kstack_top = (uint32_t)kernel_stack + sizeof(kernel_stack);
    tss_init(kstack_top);
	idt_init();
	pic_remap(0x20, 0x28);
	pic_mask_all_irqs();
	idt_set_gate(0x21, (uint32_t)isr_irq1_stub, KERNEL_CS, 0x8E);
	pic_unmask_irq1();
	__asm__ __volatile__("sti");
	init_screen();
	vga_enable_cursor(0, 15);
	vga_update_hw_cursor();
    extern void enter_user_mode();
    enter_user_mode(user_main, _user_stack_top);
	for (;;)
	{
		while (input_poll(&cmd))
		{
			update_key_state(&cmd);
			if (cmd.type == 0)
			{
				code = cmd.scancode;
				if (code >= 0x3B && code <= 0x44)
				{
					idx = code - 0x3B;
					if (f_keys[idx] != NULL)
					{
						f_keys[idx]();
					}
					continue ;
				}
				if (code == 77 || code == 75 || code == 72 || code == 80)
				{
					handeler_arrow(code);
					continue ;
				}
				else if (code == 0x53)
				{
					if ((vga_buffer[cursor_y * VGA_WIDTH
							+ cursor_x] & 0xFF) != 0)
						shift_left_line();
					continue ;
				}
				c = scancode_to_char(code);
				if (isprint(c) || c == '\b')
				{
					if (c != '\b' && cursor_x < (VGA_WIDTH - 1))
						shift_right_line();
					putchar(c);
				}
				else if (c == '\n')
				{
					command_enter();
					if (cursor_y >= VGA_HEIGHT)
						scroll();
				}
				else if (c == '\t')
					navigate_history(72);
				vga_update_hw_cursor();
			}
		}
		handle_key_repeat();
		__asm__ __volatile__("hlt");
	}
}


void user_main()
{
		input_command_t	cmd;
	uint8_t			code;
	char			c;
	int				idx;
	putchar('s');
	for (;;)
	{
		// while (input_poll(&cmd))
		// {
		// 	update_key_state(&cmd);
		// 	if (cmd.type == 0)
		// 	{
		// 		code = cmd.scancode;
		// 		if (code >= 0x3B && code <= 0x44)
		// 		{
		// 			idx = code - 0x3B;
		// 			if (f_keys[idx] != NULL)
		// 			{
		// 				f_keys[idx]();
		// 			}
		// 			continue ;
		// 		}
		// 		if (code == 77 || code == 75 || code == 72 || code == 80)
		// 		{
		// 			handeler_arrow(code);
		// 			continue ;
		// 		}
		// 		else if (code == 0x53)
		// 		{
		// 			if ((vga_buffer[cursor_y * VGA_WIDTH
		// 					+ cursor_x] & 0xFF) != 0)
		// 				shift_left_line();
		// 			continue ;
		// 		}
		// 		c = scancode_to_char(code);
		// 		if (isprint(c) || c == '\b')
		// 		{
		// 			if (c != '\b' && cursor_x < (VGA_WIDTH - 1))
		// 				shift_right_line();
		// 			putchar(c);
		// 		}
		// 		else if (c == '\n')
		// 		{
		// 			command_enter();
		// 			if (cursor_y >= VGA_HEIGHT)
		// 				scroll();
		// 		}
		// 		else if (c == '\t')
		// 			navigate_history(72);
		// 		vga_update_hw_cursor();
		// 	}
		// }
		// handle_key_repeat();
		__asm__ __volatile__("hlt");
	}

}