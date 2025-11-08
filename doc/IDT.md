# IDT (Interrupt Descriptor Table)

## 📋 İçindekiler

- [IDT Nedir?](#idt-nedir)
- [Neden Gereklidir?](#neden-gereklidir)
- [IDT Entry Yapısı](#idt-entry-yapısı)
- [Interrupt Türleri](#interrupt-türleri)
- [Flags (Type ve Attributes)](#flags-type-ve-attributes)
- [IDT Nasıl Yüklenir?](#idt-nasıl-yüklenir)
- [Interrupt Handler (ISR) Yazma](#interrupt-handler-isr-yazma)
- [KFS-1'de IDT Kullanımı](#kfs-1de-idt-kullanımı)
- [Kaynaklar](#kaynaklar)

---

## IDT Nedir?

**IDT (Interrupt Descriptor Table)**, CPU'ya şunu söyleyen bir tablodur:

> "Hangi interrupt/exception geldiğinde hangi fonksiyona (handler) gidilecek?"

x86 Protected Mode'da çalışan bir kernel'de **IDT zorunludur**.  
IDT olmazsa:
- Hardware interrupt'ları (klavye, timer, vs.) işlenemez
- Exception'lar (divide by zero, page fault, vs.) handle edilemez
- Sistem çöker veya düzgün çalışmaz

---

## Neden Gereklidir?

### 1. **Exception Handling**
CPU bir hata ile karşılaştığında (örn: divide by zero, invalid opcode), IDT'ye bakarak hangi handler'ı çağıracağını belirler.

**Örnek Exception'lar:**
- **#DE (0)**: Divide by Zero
- **#GP (13)**: General Protection Fault
- **#PF (14)**: Page Fault
- **#DF (8)**: Double Fault

### 2. **Hardware Interrupts (IRQ)**
Donanım cihazları (klavye, timer, disk) bir olay gerçekleştiğinde CPU'ya sinyal gönderir.  
Bu sinyaller **IRQ (Interrupt Request)** olarak adlandırılır ve IDT üzerinden işlenir.

**Örnek IRQ'lar:**
- **IRQ0**: Timer (PIT - Programmable Interval Timer)
- **IRQ1**: Keyboard
- **IRQ14**: Primary IDE Hard Disk

### 3. **System Calls (Gelecekte)**
Kullanıcı programları kernel fonksiyonlarını çağırmak için **software interrupt** (örn: `INT 0x80`) kullanır.

---

## IDT Entry Yapısı

Her IDT girişi **8 byte (64 bit)** uzunluğundadır ve şu yapıya sahiptir:

```
┌───────────────┬────────────┬──────────┬───────────┬───────────────┐
│   base_lo     │    sel     │ always0  │   flags   │   base_hi     │
│   (16 bit)    │  (16 bit)  │ (8 bit)  │  (8 bit)  │   (16 bit)    │
└───────────────┴────────────┴──────────┴───────────┴───────────────┘
```

### Alan Açıklamaları:

| Alan | Boyut | Açıklama |
|------|-------|----------|
| **base_lo** | 16 bit | Handler fonksiyonunun adresinin düşük 16 biti |
| **sel** | 16 bit | Code segment selector (GDT'den, genelde `0x08` kernel code) |
| **always0** | 8 bit | Rezerve, her zaman 0 olmalı |
| **flags** | 8 bit | Interrupt gate tipi ve privilege level |
| **base_hi** | 16 bit | Handler fonksiyonunun adresinin yüksek 16 biti |

### C Struct Örneği:

```c
struct idt_entry {
    uint16_t base_lo;     // Handler adresinin 0-15 bitleri
    uint16_t sel;         // Kernel code segment (GDT'den 0x08)
    uint8_t  always0;     // Her zaman 0
    uint8_t  flags;       // Gate type ve DPL
    uint16_t base_hi;     // Handler adresinin 16-31 bitleri
} __attribute__((packed));
```

---

## Interrupt Türleri

CPU, 0-255 arası **256 farklı interrupt vektörü** destekler:

| Vektör | Tür | Açıklama |
|--------|-----|----------|
| **0-31** | CPU Exceptions | Intel tarafından rezerve (divide by zero, page fault, vb.) |
| **32-47** | Hardware IRQ | PIC tarafından yönetilen donanım interrupt'ları |
| **48-255** | User Defined | Kernel tarafından tanımlanabilir (system calls, vb.) |

### CPU Exception'ları (0-31):

```
0  - #DE  Divide by Zero
1  - #DB  Debug
2  -      Non-Maskable Interrupt
3  - #BP  Breakpoint
4  - #OF  Overflow
5  - #BR  Bound Range Exceeded
6  - #UD  Invalid Opcode
7  - #NM  Device Not Available
8  - #DF  Double Fault
10 - #TS  Invalid TSS
11 - #NP  Segment Not Present
12 - #SS  Stack-Segment Fault
13 - #GP  General Protection Fault
14 - #PF  Page Fault
16 - #MF  x87 Floating-Point Exception
17 - #AC  Alignment Check
18 - #MC  Machine Check
19 - #XM  SIMD Floating-Point Exception
```

### Hardware IRQ'lar (PIC Remapped):

8259 PIC varsayılan olarak IRQ0-7'yi INT 0-7'ye map eder, ama bu CPU exception'ları ile çakışır!  
Bu yüzden **PIC remapping** yapılır:

```
IRQ0-7   → INT 0x20-0x27  (Master PIC)
IRQ8-15  → INT 0x28-0x2F  (Slave PIC)
```

**Örnek:**
- IRQ1 (Keyboard) → INT 0x21
- IRQ0 (Timer) → INT 0x20

---

## Flags (Type ve Attributes)

`flags` alanı 8 bit'tir ve şu yapıya sahiptir:

```
┌─┬─┬─┬─┬─┬───────┐
│P│DPL│0│Type │   │
└─┴─┴─┴─┴─┴───────┘
 7 6 5 4 3  2 1 0
```

### Bit Açıklamaları:

| Bit | Alan | Açıklama |
|-----|------|----------|
| **7** | **P (Present)** | 1 = Entry geçerli, 0 = Entry kullanılmıyor |
| **6-5** | **DPL** | Descriptor Privilege Level (0-3, Ring level) |
| **4** | **0** | Her zaman 0 (Storage Segment için 1) |
| **3-0** | **Type** | Gate tipi |

### Gate Tipleri:

| Type | Değer | Açıklama |
|------|-------|----------|
| Task Gate | `0x5` | Task switch için (nadiren kullanılır) |
| **Interrupt Gate** | `0xE` | **En yaygın, interrupt'ları disable eder** |
| **Trap Gate** | `0xF` | Exception'lar için, interrupt'ları disable etmez |

### Yaygın Flag Değerleri:

```c
0x8E = 10001110 = Present, Ring 0, Interrupt Gate (32-bit)
0x8F = 10001111 = Present, Ring 0, Trap Gate (32-bit)
0xEE = 11101110 = Present, Ring 3, Interrupt Gate (user space için)
```

**KFS-1'de kullanılan:** `0x8E` (Ring 0, Interrupt Gate)

---

## IDT Nasıl Yüklenir?

### 1. IDT Pointer (IDTR) Yapısı

CPU'ya IDT'nin yerini söylemek için **IDTR (IDT Register)** kullanılır:

```c
struct idt_ptr {
    uint16_t limit;    // IDT boyutu - 1 (örn: 256*8-1 = 2047)
    uint32_t base;     // IDT'nin bellek adresi
} __attribute__((packed));
```

### 2. Assembly ile Yükleme

```asm
; arch/idt/idt_load.s
global idt_load

idt_load:
    mov eax, [esp + 4]    ; Stack'ten IDTR pointer'ı al
    lidt [eax]            ; LIDT instruction ile yükle
    ret
```

### 3. C Tarafında Kullanım

```c
// arch/idt/idt.c
static struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set_gate(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_init(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;
    
    // IDT'yi temizle
    memset(&idt, 0, sizeof(struct idt_entry) * 256);
    
    // IDT'yi yükle
    idt_load((uint32_t)&idtp);
}
```

---

## Interrupt Handler (ISR) Yazma

### 1. Assembly Stub (Wrapper)

Interrupt geldiğinde CPU otomatik olarak:
1. CS ve EIP'yi stack'e push eder
2. Interrupt handler'a jump eder

Ama bizim register'ları kaydetmemiz ve geri yüklememiz gerekir:

```asm
; arch/pic/isr_irq1.s (Klavye interrupt'ı)
global isr_irq1_stub
extern keyboard_handler

isr_irq1_stub:
    pusha              ; Tüm register'ları kaydet (EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI)
    
    call keyboard_handler  ; C handler'ı çağır
    
    mov al, 0x20       ; EOI (End of Interrupt) sinyali
    out 0x20, al       ; PIC'e gönder
    
    popa               ; Register'ları geri yükle
    iret               ; Interrupt'tan dön (CS:EIP geri yükle)
```

### 2. C Handler Fonksiyonu

```c
// hw/keyboard/keyboard.c
void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);  // Klavye port'undan scancode oku
    keyboard_isr(scancode);         // İşle
}
```

### 3. IDT'ye Kaydetme

```c
// kernel/kernel.c
idt_set_gate(0x21, (uint32_t)isr_irq1_stub, KERNEL_CS, 0x8E);
```

---

## KFS-1'de IDT Kullanımı

### Başlatma Sırası:

```c
void kernel_main(uint32_t magic) {
    // 1. GDT başlat
    gdt_init();
    
    // 2. IDT başlat
    idt_init();  // 256 entry'yi sıfırla ve LIDT yap
    
    // 3. PIC'i remap et
    pic_remap(0x20, 0x28);  // IRQ0-15 → 0x20-0x2F
    pic_mask_all_irqs();     // Tüm IRQ'ları maskele
    
    // 4. Klavye handler'ını yükle
    idt_set_gate(0x21, (uint32_t)isr_irq1_stub, KERNEL_CS, 0x8E);
    
    // 5. Klavye IRQ'sunu aç
    pic_unmask_irq1();
    
    // 6. Interrupt'ları etkinleştir
    __asm__ __volatile__("sti");
    
    // Ana döngü...
}
```

### İşleyiş Akışı:

```
1. Kullanıcı klavyeye basar
2. Klavye donanımı IRQ1 sinyali gönderir
3. PIC bu sinyali CPU'ya iletir (INT 0x21)
4. CPU, IDT[0x21]'e bakar
5. isr_irq1_stub çalışır
6. keyboard_handler() çağrılır
7. Scancode okunur ve işlenir
8. EOI sinyali PIC'e gönderilir
9. IRET ile normal akışa dönülür
```

---

## Kaynaklar

### Resmi Dokümantasyon
- [Intel Software Developer Manual Vol. 3A - Chapter 6: Interrupt and Exception Handling](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [OSDev Wiki - IDT](https://wiki.osdev.org/IDT)
- [OSDev Wiki - Interrupts](https://wiki.osdev.org/Interrupts)

### Diğer Kaynaklar
- [PIC (8259A) Documentation](8259A.md)
- [System Flow Diagram](system-flow.md)
- [James Molloy's Tutorial - The IDT](http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html)

---

## Özet

- **IDT**, interrupt ve exception'ların hangi handler'a yönlendirileceğini belirler
- **256 entry** içerir (0-255 arası interrupt vektörleri)
- Her entry **8 byte** (base address, segment selector, flags)
- **Flags**: Present bit, DPL (ring level), gate type (interrupt/trap)
- **LIDT instruction** ile CPU'ya yüklenir
- **ISR (Interrupt Service Routine)**: Assembly wrapper + C handler
- **EOI (End of Interrupt)**: PIC'e interrupt'ın işlendiğini bildir

**Sonuç:** IDT olmadan modern bir kernel çalışamaz!

---

[⬅️ Ana Sayfa](README.md) | [GDT Detayları ➡️](GDT.md)
