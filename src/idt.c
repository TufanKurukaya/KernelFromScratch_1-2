// src/idt.c
#include "idt.h"

static struct idt_entry	idt[256];
struct idt_ptr			idtp;

extern void	idt_load(void); // lidt yapan küçük bir asm

void	idt_set_gate(int num, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt[num].base_lo = (base & 0xFFFF);
	idt[num].base_hi = (base >> 16) & 0xFFFF;
	idt[num].sel = sel; // genelde 0x08 (code segment)
	idt[num].always0 = 0;
	idt[num].flags = flags; // 0x8E: present|ring0|32-bit interrupt gate
}


void	idt_init(void)
{
	idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
	idtp.base = (uint32_t)&idt;
	idt_load(); // lidt
}
