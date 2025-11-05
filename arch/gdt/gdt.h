// src/inc/gdt.h
#if !defined(GDT_H)
# define GDT_H

# define GDT_ADDR 0x00000800

# include <stdint.h>



struct __attribute__((packed)) gdt_entry
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t gran;
	uint8_t base_high;
};

struct __attribute__((packed)) gdt_ptr
{
	uint16_t limit;
	uint32_t base;
};

void	gdt_init(void);

void	gdt_set(int idx, uint32_t base, uint32_t limit, uint8_t access,
		uint8_t gran);
#define GDT_KERNEL_CODE 1
#define GDT_KERNEL_DATA 2
#define GDT_USER_CODE   3
#define GDT_USER_DATA   4
#define GDT_TSS         5   // yeni: TSS için yer

#define KERNEL_CS (GDT_KERNEL_CODE << 3) // 0x08
#define KERNEL_DS (GDT_KERNEL_DATA << 3) // 0x10

#define USER_CS  ((GDT_USER_CODE  << 3) | 3) // RPL=3
#define USER_DS  ((GDT_USER_DATA  << 3) | 3) // RPL=3
#define TSS_SEL   (GDT_TSS << 3)
#endif
