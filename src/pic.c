// src/pic.c
#include "inc/io.h"
#include "inc/pic.h"

#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

void	pic_remap(int offset1, int offset2)
{
	uint8_t	a1;
	uint8_t	a2;

	a1 = inb(PIC1_DATA);
	a2 = inb(PIC2_DATA);
	outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, offset1);
	outb(PIC2_DATA, offset2);
	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
	outb(PIC1_DATA, a1);
	outb(PIC2_DATA, a2);
}

void	pic_unmask_irq1(void)
{
	uint8_t	mask;

	mask = inb(PIC1_DATA);
	mask &= ~(1 << 1);
	outb(PIC1_DATA, mask);
}

void	pic_send_eoi_master(void)
{
	outb(PIC1_CMD, 0x20);
}

void	pic_mask_all_irqs(void)
{
	outb(PIC1_DATA, 0xFF);
	outb(PIC2_DATA, 0xFF);
}
