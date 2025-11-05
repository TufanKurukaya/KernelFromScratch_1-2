// src/tss.c
#include "tss.h"
#include "../../lib/utils.h"

extern void tss_load_tr(uint16_t selector); // asm: ltr

static struct tss_entry tss __attribute__((aligned(16)));

void tss_set_kernel_stack(uint32_t kernel_stack_top)
{
    tss.esp0 = kernel_stack_top;
    tss.ss0  = KERNEL_DS;   // 0x10
}
extern struct gdt_entry	gdt[6];
void tss_init(uint32_t kernel_stack_top)
{
    memset(&tss, 0, sizeof(tss));

    // Kernel stack bilgisi:
    tss_set_kernel_stack(kernel_stack_top);

    // I/O permission bitmap'i kapat (IOMap yok => erişim yasak; sonuna işaret et)
    tss.iomap_base = sizeof(tss);

    // GDT'ye TSS tanımlayıcı ekle (idx ve selector sabitleri için gdt.h'a bak)
    // Not: System segmentlerde G=0, D/B=0 olmalı; limit tam boy-1 verilir.
    gdt_set(GDT_TSS, (uint32_t)&tss, sizeof(tss) - 1, 0x89, 0x00); // 0x89 = Avail 32-bit TSS
    memmove((void*)GDT_ADDR, gdt, sizeof gdt);
    // TR yükle
    tss_load_tr(TSS_SEL);
}
