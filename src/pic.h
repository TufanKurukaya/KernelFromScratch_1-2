// src/pic.h
#ifndef PIC_H
#define PIC_H
#include <stdint.h>

void pic_remap(int offset1, int offset2);
void pic_mask_all_irqs(void);
void pic_unmask_irq1(void);
void pic_send_eoi_master(void);

#endif
