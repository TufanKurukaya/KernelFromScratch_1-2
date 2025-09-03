// src/vga.h
#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

#define VGA_BLACK        0x0
#define VGA_BLUE         0x1
#define VGA_GREEN        0x2
#define VGA_CYAN         0x3
#define VGA_RED          0x4
#define VGA_MAGENTA      0x5
#define VGA_BROWN        0x6
#define VGA_LIGHT_GREY   0x7
#define VGA_DARK_GREY    0x8
#define VGA_LIGHT_BLUE   0x9
#define VGA_LIGHT_GREEN  0xA
#define VGA_LIGHT_CYAN   0xB
#define VGA_LIGHT_RED    0xC
#define VGA_LIGHT_MAGENTA 0xD
#define VGA_YELLOW       0xE
#define VGA_WHITE        0xF

static inline uint8_t VGA_COLOR(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

static inline void vga_clear(uint8_t color) {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    const size_t width = 80;
    const size_t height = 25;
    uint16_t entry = ((uint16_t)color) << 8 | ' ';
    for (size_t i = 0; i < width * height; ++i) {
        vga[i] = entry;
    }
}

#endif

