// src/inc/gdt.h
#if !defined(GDT_H)
# define GDT_H

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

# define GDT_KERNEL_CODE 1
# define GDT_KERNEL_DATA 2
# define KERNEL_CS (GDT_KERNEL_CODE << 3)
# define KERNEL_DS (GDT_KERNEL_DATA << 3)

#endif