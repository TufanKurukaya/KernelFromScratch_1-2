// src/kernel.c
#include <stddef.h>
#include <stdint.h>
#include "vga.h"

// Çok erken aşamada stdlib yok; kendi basit fonksiyonlarımızı yazacağız.
static inline void vga_putc_at(char c, uint8_t color, size_t x, size_t y) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    const size_t width = 80;
    vga[y * width + x] = ((uint16_t)color << 8) | (uint8_t)c;
}

static void vga_print(const char* s, uint8_t color, size_t x, size_t y) {
    size_t i = 0;
    while (s[i]) {
        vga_putc_at(s[i], color, x + i, y);
        i++;
    }
}

static int u32_to_dec(uint32_t v, char* out) {
    // v'yi decimal string'e çevirir, uzunluk döner
    if (v == 0) { out[0]='0'; out[1]=0; return 1; }
    char tmp[12]; int n=0;
    while (v) { uint32_t q=v/10, r=v%10; tmp[n++] = (char)('0'+r); v=q; }
    for (int i=0;i<n;i++) out[i]=tmp[n-1-i];
    out[n]=0; return n;
}

void kernel_main(uint32_t magic, uint32_t addr) {
    char str[12] = {0};
    u32_to_dec(magic, str);
    vga_clear(VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    vga_print(str, VGA_COLOR(VGA_LIGHT_GREEN, VGA_BLACK), 38, 12);
    typedef struct {
         uint32_t flags;
         uint32_t mem_lower;
         uint32_t mem_upper;
         // ... diğer alanlar ...
         uint32_t mmap_length;
         uint32_t mmap_addr;
     } mb_info_t;
     
     mb_info_t *a= (mb_info_t *)addr;
     u32_to_dec(a->flags,str);
     vga_print(str, VGA_COLOR(VGA_DARK_GREY, VGA_WHITE), 38, 2);

    while (1) { __asm__ __volatile__("hlt"); }
}
