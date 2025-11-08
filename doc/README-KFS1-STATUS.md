# KFS_1 Uygunluk Özeti

## Uygun Olanlar

- GRUB ile boot edilebilir çekirdek
  - `iso/boot/grub/grub.cfg` mevcut, `multiboot` ile `kernel.bin` yükleniyor.
  - `make iso` hedefi `grub-mkrescue` ile önyüklenebilir ISO üretiyor. `make run` QEMU ile çalıştırıyor. (Bkz. `Makefile`)
- ASM boot kodu ve Multiboot
  - `src/assambly/boot.s` içinde multiboot header tanımlı, stack kuruluyor, GRUB’tan gelen magic argümanı ile `kernel_main` çağrılıyor.
- Temel kernel kodu (C)
  - GDT/IDT/PIC kurulumu: `src/gdt.c`, `src/idt.c`, `src/pic.c`, yükleme stub’ları: `src/assambly/*.s`
  - IRQ1/klavye ISR’ı: `src/assambly/isr_irq1.s` ve `src/keyboard.c`
- Derleme bayrakları ve i386 mimarisi
  - `-m32`, `-ffreestanding`, `-fno-builtin`, `-fno-stack-protector`, `-nostdlib`, `-nodefaultlibs` kullanılıyor. (Bkz. `Makefile` → `CFLAGS`)
  - i386 gereksinimi: QEMU i386 ile çalıştırılıyor, linker `-m elf_i386` (Bkz. `LDFLAGS`).
- Linker dosyası ve özel yerleşim
  - `linker.ld` özelleştirilmiş, giriş `ENTRY(start)`, `. = 1M;`, multiboot/text/data/bss bölümleri hizalı.
- Ekran arayüzü (VGA text mode)
  - Temel I/O: `putchar`, `vga_print`, satır kaydırma (scroll), donanımsal imleç (cursor) güncelleme mevcut. (Bkz. `src/kernel.c`)
- Makefile
  - Hem ASM (`nasm`) hem C (`gcc`) için kurallar var; link adımı `ld` ile ve `linker.ld` kullanılarak yapılıyor. ISO ve çalıştırma hedefleri sağlanmış.
- Sanal imaja GRUB kurulumu
  - `grub-mkrescue` ile minimal modüller (`multiboot iso9660 normal`) eklenerek ISO üretiliyor; boyutu küçük tutmaya yardımcı.

## Bonus – Karşılananlar

- Scroll ve cursor desteği
  - `scroll()` ve `vga_update_hw_cursor()`, `vga_enable_cursor()` mevcut. İmleç pozisyon yönetimi yapılmış.
- Renk desteği
  - `vga_color` ve `VGA_COLOR()` ile öntanımlı renkler; F4 ile önyüz rengi döngüsü. (Bkz. `src/keyboard.c`)
- printf/printk benzeri yardımcılar
  - `src/printf.c` ile `printf` benzeri minimal biçimlendirme desteği.
- Klavye girdileri ve yazdırma
  - IRQ1 ile tarama kodları kuyruğa alınıyor; yazma/silme/sol-sağ ok, history gezinme vb. destekleniyor. (Bkz. `src/keyboard.c`, `src/kernel.c`, `src/screen.c`)
- Farklı ekranlar ve kısayollar
  - 3 sanal ekran ve F1–F3 ile geçiş; F5 ile aktif ekran temizleme/yenileme. (Bkz. `src/screen.c`, `src/keyboard.c`)
