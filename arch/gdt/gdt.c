// src/gdt.c
#include "../../lib/utils.h"
#include "gdt.h"

static struct gdt_entry	gdt[6];
struct gdt_ptr			gdtp;
extern void				gdt_load(void);
extern void				gdt_reload_segments(void);

static void	gdt_set(int idx, uint32_t base, uint32_t limit, uint8_t access,
		uint8_t gran)
{
	gdt[idx].limit_low = limit & 0xFFFF;
	gdt[idx].base_low = base & 0xFFFF;
	gdt[idx].base_mid = (base >> 16) & 0xFF;
	gdt[idx].access = access;
	gdt[idx].gran = ((limit >> 16) & 0x0F) | (gran & 0xF0);
	gdt[idx].base_high = (base >> 24) & 0xFF;
}

/*
	ring 0
	kernel code segment =	0x9A ->	10011010
	kernel data segment =	0x92 ->	10010010

	ring 3
	user code segment = 	0xFA ->	11111010
	user data segment = 	0XF2 ->	11110010
*/

// gdt[0] = 0x0000000000000000;
// gdt[1] = 0x00CF9A000000FFFF;
// gdt[2] = 0x00CF92000000FFFF;
// gdt[3] = 0x00CFFA000000FFFF;
// gdt[4] = 0x00CFF2000000FFFF;

void	gdt_init(void)
{
	gdtp.limit = sizeof(gdt) - 1;
	gdtp.base = GDT_ADDR;
	gdt_set(0, 0, 0, 0, 0);
	gdt_set(1, 0, 0xFFFFF, 0x9A, 0xCF);
	gdt_set(2, 0, 0xFFFFF, 0x92, 0xCF);
	gdt_set(4, 0, 0xFFFFF, 0xFA, 0xCF);
	gdt_set(5, 0, 0xFFFFF, 0xF2, 0xCF);
	memmove((void *)GDT_ADDR, gdt, sizeof(gdt));
	gdt_load();
	gdt_reload_segments();
}
