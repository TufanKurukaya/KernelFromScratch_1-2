# KFS_1 Uygunluk Özeti (27 Ekim 2025)

Bu dosya, "KFS_1 – Grub, boot and screen" PDF’indeki (kfs.1.en.pdf) gereksinimlere göre mevcut projenin durumunu özetler. Zorunlu ve bonus isterler PDF’in tamamı dikkate alınarak iki başlık altında listelenmiştir.

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

## Eksik Kalanlar / Dikkat Gerektirenler

- Zorunlu: "Ekrana 42 yazdır"
  - `kernel_main` başlangıçta "42" basmıyor; başlık metninde açıkça istenen çıktı eksik. Basit bir `vga_print("42\n");` eklenmeli.
- "Hello world" örneği
  - Genel hedefler arasında geçiyor; somut bir "Hello, world" veya benzeri karşılama mesajı yok.
- Sanal imaj teslimi ve 10 MB sınırı
  - `Makefile` ISO üretse de repoda hazır `.iso` dosyası yok; PDF teslimi "basic virtual image" ister. Üretilen ISO boyutu yüksek olmasa da <10 MB sınırı için kontrol/teslim süreci net değil.
- Komut işleme iskeleti tamamlanmamış
  - `process_command()` içinde `TODO` bulunuyor; komut yorumlama uygulanmamış. (Bkz. `src/kernel.c`)
- C++ bayrakları
  - PDF’teki `-fno-exception`/`-fno-rtti` C++ için; proje C kullandığından uygulanmıyor. Sorun değil, sadece not.
- Boot argümanları push sırası
  - `boot.s` hem `ebx` hem `eax` push ediyor; `kernel_main` tek parametre alıyor. İşlevsel olsa da `ebx` push’u gereksiz ve yığıt üzerinde asılı kalıyor.
- Exception ISR’ları
  - IDT kurulsa da yalnızca IRQ1 (klavye) handler tanımlı; CPU exception vektörleri (0–31) için ISR/IDT girişleri yok. Zorunlu değil ama faydalı.
- Dokümantasyon
  - Kısa GDT/IDT notları (`README.md`) var; KFS_1’in çalışma/kurulum/koşum adımlarını ve gereksinim eşleşmesini anlatan bölüm bu dosya ile tamamlanmış oldu.

