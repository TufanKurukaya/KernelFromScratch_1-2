# VGA Text Mode Driver

## 📋 İçindekiler

- [Genel Bakış](#genel-bakış)
- [VGA Text Mode](#vga-text-mode)
- [VGA Buffer](#vga-buffer)
- [Renkler](#renkler)
- [Hardware Cursor](#hardware-cursor)
- [Implementation](#implementation)

---

## Genel Bakış

VGA (Video Graphics Array), ekrana metin ve grafik çıktısı vermek için kullanılan standart bir video adaptörüdür. KFS-1, **VGA text mode 3 (80x25)** kullanır.

---

## VGA Text Mode

### Mode 3 Özellikleri:

- **Çözünürlük**: 80 sütun x 25 satır = 2000 karakter
- **Renk Derinliği**: 16 ön plan + 8 arka plan rengi
- **Buffer Adresi**: `0xB8000`
- **Format**: Her karakter 2 byte (ASCII + Attribute)

---

## VGA Buffer

VGA buffer, bellekte `0xB8000` adresinde bulunur ve her karakter için 2 byte içerir:

```
┌──────────────┬──────────────┐
│   Byte 0     │   Byte 1     │
│   ASCII      │  Attribute   │
└──────────────┴──────────────┘
```

### Attribute Byte:

```
┌─────────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┬───────┐
│  Bit    │   7   │   6   │   5   │   4   │   3   │   2   │   1   │   0   │
├─────────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┼───────┤
│ Kullanım│ Blink │      Background       │         Foreground            │
└─────────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┴───────┘
```

### Örnek:

```c
// 'A' karakterini beyaz üzerine mavi yazmak
vga_buffer[0] = 'A';
vga_buffer[1] = 0x1F;  // 0001 1111 = Mavi arkaplan, Beyaz ön plan
```

---

## Renkler

### VGA Renk Paleti:

| Değer | Renk | Hex |
|-------|------|-----|
| 0 | Black | 0x0 |
| 1 | Blue | 0x1 |
| 2 | Green | 0x2 |
| 3 | Cyan | 0x3 |
| 4 | Red | 0x4 |
| 5 | Magenta | 0x5 |
| 6 | Brown | 0x6 |
| 7 | Light Grey | 0x7 |
| 8 | Dark Grey | 0x8 |
| 9 | Light Blue | 0x9 |
| 10 | Light Green | 0xA |
| 11 | Light Cyan | 0xB |
| 12 | Light Red | 0xC |
| 13 | Light Magenta | 0xD |
| 14 | Yellow | 0xE |
| 15 | White | 0xF |

### Renk Makrosu:

```c
#define VGA_COLOR(fg, bg) ((bg << 4) | fg)

// Örnek: Beyaz metin, mavi arkaplan
uint8_t color = VGA_COLOR(VGA_WHITE, VGA_BLUE);  // 0x1F
```

---

## Hardware Cursor

VGA, donanımsal bir cursor (imleç) destekler. Cursor pozisyonu, **CRT Controller** register'ları ile kontrol edilir.

### Cursor Port'ları:

| Port | Kullanım |
|------|----------|
| 0x3D4 | Index Register (hangi register'ı seçeceğimiz) |
| 0x3D5 | Data Register (değer yazma) |

### Cursor Pozisyonu Ayarlama:

```c
void vga_update_hw_cursor(void) {
    uint16_t position = cursor_y * VGA_WIDTH + cursor_x;
    
    // Cursor Location Low
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    
    // Cursor Location High
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}
```

### Cursor Şeklini Ayarlama:

```c
void vga_enable_cursor(uint8_t start, uint8_t end) {
    // start: Cursor başlangıç satırı (0-15)
    // end: Cursor bitiş satırı (0-15)
    
    outb(0x3D4, 0x0A);  // Cursor Start Register
    outb(0x3D5, start);
    
    outb(0x3D4, 0x0B);  // Cursor End Register
    outb(0x3D5, end);
}
```

---

## Implementation

### VGA Başlatma:

```c
// hw/vga/vga.c
#define VGA_BUFFER 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile uint16_t *vga_buffer = (uint16_t *)VGA_BUFFER;
volatile uint8_t vga_color = VGA_COLOR(VGA_WHITE, VGA_BLACK);
int cursor_x = 0;
int cursor_y = 0;

void vga_clear(uint8_t color) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int index = y * VGA_WIDTH + x;
            vga_buffer[index] = (' ' | (color << 8));
        }
    }
    cursor_x = 0;
    cursor_y = 0;
}
```

### Karakter Yazdırma:

```c
void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            int index = cursor_y * VGA_WIDTH + cursor_x;
            vga_buffer[index] = (' ' | (vga_color << 8));
        }
    } else {
        int index = cursor_y * VGA_WIDTH + cursor_x;
        vga_buffer[index] = (c | (vga_color << 8));
        cursor_x++;
        
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    
    if (cursor_y >= VGA_HEIGHT) {
        scroll();
    }
    
    vga_update_hw_cursor();
}
```

### Scroll:

```c
void scroll(void) {
    // Her satırı bir yukarı kaydır
    for (int y = 0; y < VGA_HEIGHT - 1; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int src = (y + 1) * VGA_WIDTH + x;
            int dst = y * VGA_WIDTH + x;
            vga_buffer[dst] = vga_buffer[src];
        }
    }
    
    // Son satırı temizle
    for (int x = 0; x < VGA_WIDTH; x++) {
        int index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        vga_buffer[index] = (' ' | (vga_color << 8));
    }
    
    cursor_y = VGA_HEIGHT - 1;
}
```

---

## Özet

- **VGA Buffer**: `0xB8000`, 80x25 karakterlik alan
- **Format**: 2 byte per char (ASCII + Attribute)
- **Renkler**: 16 ön plan, 8 arka plan
- **Hardware Cursor**: Port 0x3D4/0x3D5 ile kontrol

---

[⬅️ Ana Sayfa](README.md) | [Keyboard ➡️](keyboard.md)
