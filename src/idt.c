// src/idt.c
#include "inc/idt.h"

static struct idt_entry	idt[256];
struct idt_ptr			idtp;
extern void				idt_load(void);

void	idt_set_gate(int num, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt[num].base_lo = (base & 0xFFFF);
	idt[num].base_hi = (base >> 16) & 0xFFFF;
	idt[num].sel = sel;
	idt[num].always0 = 0;
	idt[num].flags = flags;
}

void	idt_init(void)
{
	idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
	idtp.base = (uint32_t)&idt;
	idt_load();
}
