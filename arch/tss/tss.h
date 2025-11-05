// src/inc/tss.h
#ifndef TSS_H
#define TSS_H

#include "../gdt/gdt.h"

struct __attribute__((packed)) tss_entry {
    uint16_t link, _res0;
    uint32_t esp0;      // ring0 stack pointer
    uint16_t ss0, _res1;// ring0 stack segment
    uint32_t esp1;
    uint16_t ss1, _res2;
    uint32_t esp2;
    uint16_t ss2, _res3;
    uint32_t cr3;
    uint32_t eip, eflags;
    uint32_t eax, ecx, edx, ebx;
    uint32_t esp, ebp, esi, edi;
    uint16_t es, _res4;
    uint16_t cs, _res5;
    uint16_t ss, _res6;
    uint16_t ds, _res7;
    uint16_t fs, _res8;
    uint16_t gs, _res9;
    uint16_t ldt, _res10;
    uint16_t trap, iomap_base;
};

void    tss_init(uint32_t kernel_stack_top);
void    tss_set_kernel_stack(uint32_t kernel_stack_top);

#endif
