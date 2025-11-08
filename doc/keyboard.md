# Keyboard Driver (Klavye Sürücüsü)

## 📋 İçindekiler

- [Genel Bakış](#genel-bakış)
- [Klavye Donanımı](#klavye-donanımı)
- [Scancode](#scancode)
- [Interrupt-Driven Yaklaşım](#interrupt-driven-yaklaşım)
- [Circular Buffer (Dairesel Tampon)](#circular-buffer-dairesel-tampon)
- [Scancode Translation](#scancode-translation)
- [Key State Tracking](#key-state-tracking)
- [Key Repeat](#key-repeat)
- [Özel Tuşlar](#özel-tuşlar)
- [Implementation](#implementation)

---

## Genel Bakış

KFS-1 keyboard driver'ı, **interrupt-driven** bir yaklaşım kullanır. Klavyeden gelen her tuş basımı bir **IRQ1 interrupt**'ı tetikler ve kernel bu interrupt'ı işler.

### Özellikler:

- ✅ Interrupt-based input (polling değil)
- ✅ Circular buffer queue (input kaybı yok)
- ✅ Scancode to ASCII translation
- ✅ Modifier key support (Shift, Ctrl, Alt, Caps Lock)
- ✅ Key repeat functionality
- ✅ F-key custom handlers
- ✅ Arrow key navigation

---

## Klavye Donanımı

### PS/2 Keyboard Controller

KFS-1, PS/2 klavye standardını kullanır. Klavye, **8042 keyboard controller** üzerinden CPU ile iletişim kurar.

### Port Adresleri:

| Port | Kullanım |
|------|----------|
| **0x60** | Data Port (Scancode okuma/yazma) |
| **0x64** | Status/Command Port |

### Scancode Okuma:

```c
uint8_t scancode = inb(0x60);  // 0x60 portundan scancode oku
```

---

## Scancode

**Scancode**, klavyeden gelen ham tuş kodu bilgisidir. Her tuşa bir veya daha fazla scancode karşılık gelir.

### Scancode Türleri:

1. **Make Code (Press)**: Tuşa basıldığında gönderilir
   - Örnek: A tuşu → `0x1E`

2. **Break Code (Release)**: Tuş bırakıldığında gönderilir
   - Örnek: A tuşu bırakma → `0x9E` (make code + 0x80)

### Scancode Set 1 (KFS-1'de kullanılan):

```
┌─────┬─────┬─────┬─────┐
│ Esc │ F1  │ F2  │ F3  │
│ 0x01│0x3B │0x3C │0x3D │
├─────┼─────┼─────┼─────┤
│  1  │  2  │  3  │  4  │
│0x02 │0x03 │0x04 │0x05 │
├─────┼─────┼─────┼─────┤
│  Q  │  W  │  E  │  R  │
│0x10 │0x11 │0x12 │0x13 │
└─────┴─────┴─────┴─────┘
```

**Önemli:** Scancode ≠ ASCII! Translation gereklidir.

Referans: [Keyboard Scancodes](https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html)

---

## Interrupt-Driven Yaklaşım


### IRQ1 (Keyboard Interrupt):

1. Kullanıcı tuşa basar
2. Klavye donanımı **IRQ1** sinyali gönderir
3. **8259 PIC** bu sinyali CPU'ya iletir (INT 0x21)
4. CPU, **IDT[0x21]** adresindeki handler'ı çalıştırır
5. **isr_irq1_stub** (Assembly) → **keyboard_handler** (C)
6. Scancode okunur ve queue'ya eklenir
7. **EOI (End of Interrupt)** PIC'e gönderilir
8. Normal akışa dönülür

### ISR (Interrupt Service Routine):

```asm
; arch/pic/isr_irq1.s
global isr_irq1_stub
extern keyboard_handler

isr_irq1_stub:
    pusha                      ; Tüm register'ları kaydet
    call keyboard_handler      ; C handler
    mov al, 0x20              ; EOI
    out 0x20, al              ; PIC'e gönder
    popa                       ; Register'ları geri yükle
    iret                       ; Interrupt'tan dön
```

---

## Circular Buffer (Dairesel Tampon)

Input kaybını önlemek için **circular queue** kullanılır.

### Circular Queue Yapısı:

```c
#define CMD_QUEUE_SIZE 16

typedef struct {
    uint8_t scancode;
    uint8_t type;      // 0 = press, 1 = release
} input_command_t;

static input_command_t cmd_queue[CMD_QUEUE_SIZE];
static int head = 0;  // Okuma pozisyonu
static int tail = 0;  // Yazma pozisyonu
```

### Enqueue (Ekleme):

```c
void enqueue(input_command_t cmd) {
    if (queue_is_full())
        return;
    cmd_queue[tail] = cmd;
    tail = (tail + 1) % CMD_QUEUE_SIZE;  // Circular
}
```

### Dequeue (Çıkarma):

```c
int dequeue(input_command_t *out) {
    if (queue_is_empty())
        return 0;
    *out = cmd_queue[head];
    head = (head + 1) % CMD_QUEUE_SIZE;  // Circular
    return 1;
}
```

### Avantajları:

- ✅ Fixed size (dinamik allocation yok)
- ✅ Thread-safe (interrupt context'te güvenli)
- ✅ O(1) complexity
- ✅ Overflow kontrolü

---

## Scancode Translation

Scancode'u ASCII karaktere çevirmek için **lookup table** kullanılır.

### Translation Table:

```c
// hw/keyboard/utils.c
static const char scancode_to_ascii_table[128] = {
    0,   0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', ...
};

static const char scancode_to_ascii_shift[128] = {
    0,   0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', ...
};
```

### Translation Fonksiyonu:

```c
char scancode_to_char(uint8_t scancode) {
    if (scancode >= 128)
        return 0;  // Break code, ignore
    
    // Shift basılı mı?
    if (is_shift_pressed())
        return scancode_to_ascii_shift[scancode];
    
    // Caps Lock aktif mi?
    if (is_caps_lock_active() && is_letter(scancode))
        return to_upper(scancode_to_ascii_table[scancode]);
    
    return scancode_to_ascii_table[scancode];
}
```

---

## Key State Tracking

Modifier tuşların (Shift, Ctrl, Alt, Caps Lock) durumunu takip etmek gerekir.

### Key State Array:

```c
static uint8_t key_state[128];  // Her tuş için state

// Bit 0: Pressed (basılı mı?)
// Bit 1: Toggled (Caps/Num/Scroll Lock için)
```

### State Update:

```c
void update_key_state(input_command_t *cmd) {
    uint8_t sc = cmd->scancode;
    
    if (cmd->type == 0) {  // Press
        key_state[sc] |= (1 << 0);  // Pressed bit = 1
    } else {  // Release
        key_state[sc] &= ~(1 << 0);  // Pressed bit = 0
    }
    
    // Toggle keys (Caps Lock, Num Lock, Scroll Lock)
    if (sc == 0x3A && cmd->type == 0) {  // Caps Lock
        key_state[sc] ^= (1 << 1);  // Toggle bit flip
    }
}
```

### Modifier Check:

```c
int is_shift_pressed(void) {
    return (key_state[0x2A] & 1) || (key_state[0x36] & 1);
    // 0x2A = Left Shift, 0x36 = Right Shift
}

int is_caps_lock_active(void) {
    return (key_state[0x3A] & 2) != 0;  // Toggle bit
}
```

---

## Key Repeat

Bir tuşa uzun süre basıldığında karakterin tekrarlanması (typematic).

### Repeat Logic:

```c
typedef struct {
    uint8_t key;        // Hangi tuş
    uint32_t delay;     // İlk repeat için bekleme
    uint32_t rate;      // Sonraki repeat'ler arası süre
    uint32_t counter;   // Zaman sayacı
} repeat_state_t;

static repeat_state_t repeat = {0};

void handle_key_repeat(void) {
    if (repeat.key == 0)
        return;
    
    repeat.counter++;
    
    if (repeat.counter >= repeat.delay) {
        // Tuşu tekrarla
        process_key(repeat.key);
        repeat.counter = repeat.delay - repeat.rate;
    }
}
```

**Ana döngüde çağrılır:**

```c
while (1) {
    while (input_poll(&cmd)) {
        // ...
    }
    handle_key_repeat();  // Repeat kontrolü
    __asm__ volatile("hlt");
}
```

---

## Özel Tuşlar

### F-Keys (F1-F10):

```c
// hw/keyboard/f_keys.c
typedef void (*f_key_handler_t)(void);
f_key_handler_t f_keys[10];

void f1_handler(void) {
    printf("F1 pressed - Help\n");
}

void f_keys_init(void) {
    f_keys[0] = f1_handler;  // F1
    f_keys[1] = NULL;        // F2 (tanımsız)
    // ...
}

// Ana döngüde:
if (scancode >= 0x3B && scancode <= 0x44) {
    int idx = scancode - 0x3B;
    if (f_keys[idx])
        f_keys[idx]();
}
```

### Arrow Keys:

```c
void handle_arrow(uint8_t scancode) {
    switch (scancode) {
        case 72: cursor_up(); break;     // Up arrow
        case 80: cursor_down(); break;   // Down arrow
        case 75: cursor_left(); break;   // Left arrow
        case 77: cursor_right(); break;  // Right arrow
    }
}
```

---

## Implementation

### Keyboard Handler (C):

```c
// hw/keyboard/keyboard.c
void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    keyboard_isr(scancode);
}

void keyboard_isr(unsigned char scancode) {
    input_command_t cmd;
    cmd.scancode = scancode & 0x7F;  // Alt 7 bit
    cmd.type = (scancode & 0x80) ? 1 : 0;  // Bit 7 = release
    enqueue(cmd);
}
```

### Main Loop Poll:

```c
// kernel/kernel.c
while (1) {
    while (input_poll(&cmd)) {
        update_key_state(&cmd);
        
        if (cmd.type == 0) {  // Press
            char c = scancode_to_char(cmd.scancode);
            if (c != 0) {
                putchar(c);
            }
        }
    }
    
    handle_key_repeat();
    __asm__ volatile("hlt");
}
```

---

## Özet

1. **IRQ1 Interrupt** → Klavye tuşu basıldı
2. **ISR** → Scancode oku, queue'ya ekle
3. **Main Loop** → Queue'dan al, translate et
4. **ASCII** → Ekrana yazdır veya komut işle
5. **State Tracking** → Modifier tuşları takip et
6. **Key Repeat** → Uzun basışta tekrarla

**Sonuç:** Interrupt-driven, queue-based, robust bir klavye driver!

---

[⬅️ Ana Sayfa](README.md) | [VGA Driver ➡️](vga.md) | [Shell ➡️](shell.md)
