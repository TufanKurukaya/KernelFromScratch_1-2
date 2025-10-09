// src/gdt.c
#include "inc/gdt.h"

static struct gdt_entry	gdt[3];
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

void	gdt_init(void)
{
	gdtp.limit = sizeof(gdt) - 1;
	gdtp.base = (uint32_t)&gdt;
	gdt_set(0, 0, 0, 0, 0);
	gdt_set(1, 0, 0xFFFFF, 0x9A, 0xCF);
	gdt_set(2, 0, 0xFFFFF, 0x92, 0xCF);
	// user icin de ayarlanacak
	gdt_load();
	gdt_reload_segments();
}
