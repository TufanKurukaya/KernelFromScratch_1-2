# Multiboot Specification

## 📋 İçindekiler

- [Genel Bakış](#genel-bakış)
- [Multiboot Nedir?](#multiboot-nedir)
- [Multiboot Header](#multiboot-header)
- [Multiboot Info Structure](#multiboot-info-structure)
- [Boot Sürecinde Multiboot](#boot-sürecinde-multiboot)
- [KFS-1'de Multiboot Kullanımı](#kfs-1de-multiboot-kullanımı)
- [GRUB vs Diğer Bootloader'lar](#grub-vs-diğer-bootloaderlar)
- [Debug ve Test](#debug-ve-test)

---

## Genel Bakış

**Multiboot**, bootloader ve işletim sistemi kernel'i arasındaki standart bir arayüzdür. Bu spesifikasyon, farklı bootloader'ların (GRUB, LILO, vb.) aynı kernel'i yükleyebilmesini sağlar.

### Neden Multiboot?

- ✅ **Taşınabilirlik**: Kernel, farklı bootloader'larla çalışabilir
- ✅ **Standart**: Herkes aynı protokolü kullanır
- ✅ **Bilgi Aktarımı**: Bootloader, kernel'e bellek, modül vb. bilgileri verir
- ✅ **Esneklik**: Kernel'in ihtiyaçlarını belirtebilir (bellek, video modu vb.)

---

## Multiboot Nedir?

Multiboot, iki ana bileşenden oluşur:

1. **Multiboot Header** (Kernel'de)
   - Kernel binary'sinin ilk 8KB'sında bulunmalı
   - Bootloader'a "ben multiboot-compatible bir kernel'im" der

2. **Multiboot Info Structure** (Bootloader'dan)
   - Bootloader, kernel'e sistem bilgilerini bu yapı ile aktarır
   - Bellek miktarı, boot cihazı, komut satırı, vb.

### Multiboot Versiyonları:

| Versiyon | Durum | Kullanım |
|----------|-------|----------|
| **Multiboot 1** | ✅ Stable | KFS-1'de kullanılan, yaygın |
| **Multiboot 2** | ✅ Stable | Daha modern, daha fazla özellik |

KFS-1, **Multiboot 1** kullanır (basit ve yeterli).

---

## Multiboot Header

Kernel binary'sinin **ilk 8192 byte**'ında bulunmalıdır. GRUB, bu header'ı arayarak kernel'in multiboot-compatible olup olmadığını anlar.

### Header Yapısı:

```c
struct multiboot_header {
    uint32_t magic;         // 0x1BADB002 (sabit değer)
    uint32_t flags;         // Özellik bayrakları
    uint32_t checksum;      // -(magic + flags)
    
    // flags bit 16 set ise (AOUT_KLUDGE):
    uint32_t header_addr;   // Header'ın fiziksel adresi
    uint32_t load_addr;     // Kernel'in yükleneceği adres
    uint32_t load_end_addr; // Yüklemenin biteceği adres
    uint32_t bss_end_addr;  // BSS section'ın sonu
    uint32_t entry_addr;    // Kernel entry point
    
    // flags bit 2 set ise (VIDEO_MODE):
    uint32_t mode_type;     // 0=linear, 1=text
    uint32_t width;         // Genişlik (karakter veya pixel)
    uint32_t height;        // Yükseklik
    uint32_t depth;         // Bit derinliği (grafik modda)
} __attribute__((packed));
```

### Magic Number:

```c
#define MULTIBOOT_HEADER_MAGIC   0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
```

- **0x1BADB002**: Kernel'in header'ında olmalı
- **0x2BADB002**: Bootloader, kernel'e bu magic'i EAX'te verir

### Flags Biti Anlamları:

| Bit | Mask | Alan | Anlamı |
|-----|------|------|--------|
| **0** | 0x00000001 | Page-align modules | Modülleri 4KB hizalı yükle |
| **1** | 0x00000002 | Memory info | Bellek bilgisi zorunlu |
| **2** | 0x00000004 | Video mode | Video modu isteği |
| **16** | 0x00010000 | AOUT kludge | Adres alanları kullan |

### Checksum:

Checksum, magic ve flags toplamının negatifi olmalıdır:

```c
checksum = -(magic + flags)
```

Doğrulama:
```c
if (magic + flags + checksum == 0) {
    // Header geçerli
}
```

### KFS-1 Multiboot Header (Assembly):

```asm
; arch/boot/boot.s
SECTION .multiboot
align 4

MULTIBOOT_MAGIC     equ 0x1BADB002
MULTIBOOT_FLAGS     equ 0x00000003    ; Bit 0 + Bit 1
MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

dd MULTIBOOT_MAGIC
dd MULTIBOOT_FLAGS
dd MULTIBOOT_CHECKSUM
```

**Açıklama:**
- `MULTIBOOT_FLAGS = 0x03` → Bit 0 (page-align) + Bit 1 (memory info)
- `MULTIBOOT_CHECKSUM = -(0x1BADB002 + 0x03) = 0xE4524FFB`

---

## Multiboot Info Structure

Bootloader (GRUB), kernel'i çağırırken **EBX registerında** multiboot info structure'ın adresini verir.

### Register Durumu (Kernel Entry):

```
EAX = 0x2BADB002    // Multiboot magic (bootloader'dan)
EBX = <address>     // multiboot_info structure adresi
```

### Multiboot Info Yapısı:

```c
struct multiboot_info {
    uint32_t flags;              // Hangi alanlar geçerli?
    
    // flags[0] = 1 ise geçerli:
    uint32_t mem_lower;          // KB cinsinden < 1MB bellek
    uint32_t mem_upper;          // KB cinsinden > 1MB bellek
    
    // flags[1] = 1 ise geçerli:
    uint32_t boot_device;        // Boot edilen cihaz
    
    // flags[2] = 1 ise geçerli:
    uint32_t cmdline;            // Komut satırı string adresi
    
    // flags[3] = 1 ise geçerli:
    uint32_t mods_count;         // Yüklenen modül sayısı
    uint32_t mods_addr;          // Modül bilgileri adresi
    
    // flags[4] veya [5] = 1 ise geçerli:
    uint32_t syms[4];            // a.out veya ELF sembol tablosu
    
    // flags[6] = 1 ise geçerli:
    uint32_t mmap_length;        // Memory map uzunluğu
    uint32_t mmap_addr;          // Memory map adresi
    
    // flags[7] = 1 ise geçerli:
    uint32_t drives_length;      // BIOS drive bilgisi
    uint32_t drives_addr;
    
    // flags[8] = 1 ise geçerli:
    uint32_t config_table;       // ROM config table
    
    // flags[9] = 1 ise geçerli:
    uint32_t boot_loader_name;   // Bootloader adı string adresi
    
    // flags[10] = 1 ise geçerli:
    uint32_t apm_table;          // APM table
    
    // flags[11] = 1 ise geçerli:
    uint32_t vbe_control_info;   // VBE bilgisi
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    
    // flags[12] = 1 ise geçerli:
    uint64_t framebuffer_addr;   // Framebuffer adresi
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;    // Bits per pixel
    uint8_t  framebuffer_type;
    // ...color info...
} __attribute__((packed));
```

### Flags Biti Detayları:

| Bit | Hex | Alan | Anlamı |
|-----|-----|------|--------|
| **0** | 0x00000001 | mem_lower, mem_upper | Bellek boyutları (KB) |
| **1** | 0x00000002 | boot_device | Boot cihazı bilgisi |
| **2** | 0x00000004 | cmdline | Kernel komut satırı |
| **3** | 0x00000008 | mods_count, mods_addr | Modüller (initrd vb.) |
| **4** | 0x00000010 | syms | a.out sembol tablosu |
| **5** | 0x00000020 | syms | ELF section header |
| **6** | 0x00000040 | mmap_addr, mmap_length | Bellek haritası |
| **7** | 0x00000080 | drives | BIOS disk sürücüleri |
| **8** | 0x00000100 | config_table | ROM config table |
| **9** | 0x00000200 | boot_loader_name | Bootloader adı |
| **10** | 0x00000400 | apm_table | APM bilgisi |
| **11** | 0x00000800 | vbe_* | VESA BIOS Extension |
| **12** | 0x00001000 | framebuffer_* | Framebuffer bilgisi |

### Örnek Kullanım:

```c
void kernel_main(uint32_t magic, uint32_t multiboot_addr) {
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        // Multiboot değil, hata!
        return;
    }
    
    struct multiboot_info *mbi = (struct multiboot_info *)multiboot_addr;
    
    if (mbi->flags & 0x01) {
        // Bellek bilgisi var
        printf("Lower memory: %d KB\n", mbi->mem_lower);
        printf("Upper memory: %d KB\n", mbi->mem_upper);
    }
    
    if (mbi->flags & 0x200) {
        // Bootloader adı var
        printf("Booted by: %s\n", (char *)mbi->boot_loader_name);
    }
}
```

---

## Boot Sürecinde Multiboot

### Adım Adım Akış:

```
1. BIOS/UEFI POST
   ↓
2. GRUB Stage 1 yüklenir (MBR)
   ↓
3. GRUB Stage 2 yüklenir
   ↓
4. GRUB grub.cfg'yi okur
   ↓
5. GRUB kernel binary'sini tara:
   - İlk 8KB'de multiboot header var mı?
   - Magic = 0x1BADB002 ?
   - Checksum doğru mu?
   ↓
6. Header geçerliyse:
   - Kernel'i belleğe yükle (varsayılan: 0x100000)
   - Multiboot info structure'ı hazırla
   - Protected mode'a geç (32-bit)
   - A20 hattını aç
   ↓
7. Kernel'e geç:
   - EAX = 0x2BADB002 (magic)
   - EBX = multiboot_info adresi
   - CS = kernel code segment
   - DS/ES/FS/GS/SS = kernel data segment
   - ESP = stack (bootloader tarafından set edilmiş)
   - EFLAGS = IF clear (interrupt'lar kapalı)
   ↓
8. Kernel çalışmaya başlar
```

### GRUB Yapılandırması:

```cfg
# iso/boot/grub/grub.cfg
menuentry "KFS-1" {
    multiboot /boot/kernel.bin
    # boot
}
```

**Basit!** GRUB otomatik olarak:
- Multiboot header'ı bulur
- Kernel'i yükler
- Protected mode'a geçer
- Kernel'i çalıştırır

---

## KFS-1'de Multiboot Kullanımı

### 1. Multiboot Header (boot.s):

```asm
SECTION .multiboot
align 4
dd 0x1BADB002           ; Magic
dd 0x00000003           ; Flags (page-align + memory)
dd -(0x1BADB002 + 0x03) ; Checksum
```

### 2. Kernel Entry Point:

```asm
SECTION .text
start:
    mov esp, stack_top   ; Stack setup
    push eax             ; Magic number'ı push et
    call kernel_main     ; C kernel'e geç
```

### 3. C Kernel:

```c
void kernel_main(uint32_t magic) {
    if (magic != 0x2BADB002) {
        vga_print("Error: Not booted by Multiboot!\n");
        while (1) __asm__ volatile("hlt");
    }
    
    // Kernel initialization...
}
```

**Not:** KFS-1 şu anda multiboot_info structure'ı kullanmıyor (sadece magic kontrolü). Gelecekte bellek haritası, modül yükleme vb. için kullanılabilir.

---

## GRUB vs Diğer Bootloader'lar

### Bootloader Karşılaştırması:

| Özellik | GRUB | Limine + Stivale2 | SYSLINUX |
|---------|------|-------------------|----------|
| **Protokol** | Multiboot 1/2 | Limine/Stivale2 | SYSLINUX |
| **Amaç** | Genel amaçlı | OS dev için optimize | Basit boot |
| **Esneklik** | Çok yüksek | Yüksek | Orta |
| **Karmaşıklık** | Yüksek | Düşük | Düşük |
| **Config** | grub.cfg | Minimal | syslinux.cfg |
| **Modern OS** | ✅ | ✅ | ⚠️ |
| **Legacy** | ✅ | ❌ | ✅ |
| **Kernel bilgi** | Multiboot yapıları | Stivale2 yapıları | Basit |

### Neden GRUB?

- ✅ **Yaygın**: Her Linux dağıtımında var
- ✅ **Multiboot standardı**: Tanınmış, dokümante
- ✅ **Kolay test**: QEMU ile hızlı boot
- ✅ **Güvenilir**: Yıllardır kullanılıyor

---

## Debug ve Test

### Multiboot Header Doğrulama:

```bash
# grub-file ile kontrol
grub-file --is-x86-multiboot kernel.bin
echo $?  # 0 = geçerli, 1 = geçersiz

# objdump ile header görüntüleme
objdump -s -j .multiboot kernel.bin
```

### Beklenen Çıktı:

```
Contents of section .multiboot:
 100000 02b0ad1b 03000000 fbff524e  ..........RN
```

- `02b0ad1b` = 0x1BADB002 (little-endian)
- `03000000` = 0x00000003 (flags)
- `fbff524e` = checksum

### QEMU ile Debug:

```bash
# QEMU log
qemu-system-i386 -cdrom kfs.iso -d int,cpu_reset

# GDB debug
qemu-system-i386 -cdrom kfs.iso -s -S
gdb kernel.bin
(gdb) target remote localhost:1234
(gdb) break start
(gdb) continue
(gdb) info registers eax ebx  # Magic ve multiboot_info
```

### Yaygın Hatalar:

| Hata | Sebep | Çözüm |
|------|-------|-------|
| GRUB kernel'i yüklemiyor | Header yok/yanlış | İlk 8KB'de olmalı |
| "Not a multiboot kernel" | Magic yanlış | 0x1BADB002 olmalı |
| Checksum hatası | Hesaplama yanlış | -(magic + flags) |
| Kernel çöküyor | Magic kontrol yok | EAX = 0x2BADB002? |

---

## Özet

1. **Multiboot Header** → Kernel'in ilk 8KB'sinde, magic + flags + checksum
2. **GRUB** → Header'ı bulur, kernel'i yükler, protected mode'a geçer
3. **EAX** → 0x2BADB002 (bootloader magic)
4. **EBX** → multiboot_info structure adresi (opsiyonel kullanım)
5. **Kernel** → Magic'i kontrol eder ve başlar

**Sonuç:** Multiboot, bootloader ve kernel arasında standart bir arayüz sağlar!

---

## Kaynaklar

- [GNU Multiboot Specification 1.0](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- [Multiboot 2 Specification](https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html)
- [OSDev Wiki - Multiboot](https://wiki.osdev.org/Multiboot)
- [GRUB Manual](https://www.gnu.org/software/grub/manual/)

---

[⬅️ Ana Sayfa](README.md) | [Boot Process ➡️](boot-process.md) | [GRUB Config ➡️](../iso/boot/grub/grub.cfg)
