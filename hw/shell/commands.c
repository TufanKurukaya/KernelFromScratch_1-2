#include "../../arch/boot/io.h"
#include "../../lib/printf.h"
#include "../screen/screen.h"
#include "../vga/vga.h"
#include "../keyboard/keyboard.h"
#include <stdint.h>

extern uint32_t stack_top;
extern uint32_t stack_bottom;

void reboot()
{
    printf("Rebooting system...\n");
    outb(0xCF9, 0x02);   // Reset enable
    outb(0xCF9, 0x06);   // Hard reset
}

void halt()
{
    printf("System halted. You can now power off.\n");
    asm volatile("cli");
    asm volatile("hlt");
}

void print_stack()
{
    uint32_t *esp;
    uint32_t *ebp;
    uint32_t top_addr = (uint32_t)&stack_top;
    uint32_t bottom_addr = (uint32_t)&stack_bottom;
    
    asm volatile("mov %%esp, %0": "=r"(esp):);
    asm volatile("mov %%ebp, %0": "=r"(ebp):);
    
    printf("Kernel Stack Information:\n");
    printf("  EBP:          0x%x\n", (uint32_t)ebp);
    printf("  ESP:          0x%x\n", (uint32_t)esp);
    printf("  STACK_TOP:    0x%x\n", top_addr);
    printf("  STACK_BOTTOM: 0x%x\n", bottom_addr);
    printf("  Stack Size:   %d bytes\n", top_addr - bottom_addr);
    printf("  Stack Used:   %d bytes\n", top_addr - (uint32_t)esp);
}

void cmd_clear()
{
	screen_reset_active(-1);
	screen_apply_active();
}

void cmd_help()
{
    printf("KFS-1 Shell - Available Commands:\n");
    printf("  help     - Display this help message\n");
    printf("  clear    - Clear the screen\n");
    printf("  stack    - Print kernel stack dump\n");
    printf("  reboot   - Reboot the system\n");
    printf("  halt     - Halt the system\n");
}
