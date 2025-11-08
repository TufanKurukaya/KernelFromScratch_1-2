# KFS-1: Kernel From Scratch

[](https://42.fr)
[](https://en.wikipedia.org/wiki/X86)
[](https://www.google.com/search?q=LICENSE)

> Önyükleyici, kesme yönetimi ve temel bir kabuk içeren, sıfırdan minimal bir x86 çekirdek uygulaması.

## 📋 İçindekiler

  - [Genel Bakış](https://www.google.com/search?q=%23overview)
  - [Özellikler](https://www.google.com/search?q=%23features)
  - [Mimari](https://www.google.com/search?q=%23architectu)
  - [Derleme ve Çalıştırma](https://www.google.com/search?q=%23building--running)
  - [Proje Yapısı](https://www.google.com/search?q=%23project-structure)
  - [Dokümantasyon](https://www.google.com/search?q=%23documentation)
  - [Kabuk Komutları](https://www.google.com/search?q=%23shell-commands)
  - [Teknik Detaylar](https://www.google.com/search?q=%23technical-details)
  - [Kaynaklar](https://www.google.com/search?q=%23resources)

## 🎯 Genel Bakış

KFS-1, C ve Assembly (AT\&T sözdizimi) ile yazılmış minimalist bir x86 çekirdeğidir. Aşağıdakiler de dahil olmak üzere temel işletim sistemi kavramlarını gösterir:

  - **Korumalı Mod**: Gerçek moddan 32-bit korumalı moda geçiş
  - **Bellek Segmentasyonu**: Bellek koruması için GDT (Global Descriptor Table) kurulumu
  - **Kesme Yönetimi**: IDT (Interrupt Descriptor Table) ve donanım kesme yönetimi
  - **Cihaz Sürücüleri**: Kesme tabanlı girişli klavye sürücüsü
  - **Kullanıcı Arayüzü**: İmleç destekli VGA metin modu ekranı
  - **Etkileşimli Kabuk**: Sistem etkileşimi için komut satırı arayüzü

Bu proje 42 Okulu müfredatının bir parçasıdır ve düşük seviyeli sistem programlamasına odaklanmaktadır.

## ✨ Özellikler

### Temel Özellikler

  - ✅ Multiboot uyumlu önyükleyici (GRUB)
  - ✅ 32-bit korumalı mod işlemi
  - ✅ Çekirdek kodu/veri segmentleri ile GDT
  - ✅ 256 kesme vektörü ile IDT
  - ✅ 8259 PIC (Programlanabilir Kesme Kontrolcüsü) yapılandırması
  - ✅ Tarama kodu çevirisi ile klavye sürücüsü
  - ✅ Renk destekli VGA metin modu (80x25)
  - ✅ Donanım imleç kontrolü

### Etkileşimli Özellikler

  - ✅ Komut işlemcili kabuk
  - ✅ Gezinme özellikli komut geçmişi (Tab/Ok tuşları)
  - ✅ Satır düzenleme (Insert, Delete, Backspace)
  - ✅ Özel işlevler için F-tuşu desteği
  - ✅ Gerçek zamanlı tuş tekrarı yönetimi

### Kabuk Komutları

  - `help` - Mevcut komutları gösterir
  - `clear` - Ekranı temizler
  - `stack` - Çekirdek yığın bilgilerini gösterir
  - `reboot` - Sistemi yeniden başlatır
  - `halt` - Sistemi durdurur

## 🏗️ Mimari

```
┌─────────────────────────────────────────────────────────┐
│                    KFS-1 Mimarisi                       │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │
│  │ Kullanıcı Alanı│  │    Kabuk     │  │   Komutlar   │   │
│  │  (Gelecek)   │  │   (Aktif)    │  │  help/stack  │   │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘   │
│         │                 │                  │          │
│  ┌──────▼─────────────────▼──────────────────▼───────┐  │
│  │           Çekirdek Katmanı (Ring 0)                 │  │
│  │  - GDT/IDT Yönetimi                               │  │
│  │  - Kesme İşleyicileri                             │  │
│  │  - Bellek Yönetimi (Temel)                        │  │
│  └───────────────────────┬───────────────────────────┘  │
│                          │                              │
│  ┌───────────────────────▼───────────────────────────┐  │
│  │    Donanım Soyutlama Katmanı (HAL)                  │  │
│  │  - Klavye Sürücüsü    - VGA Sürücüsü              │  │
│  │  - PIC Kontrolcüsü    - I/O Portları              │  │
│  └───────────────────────┬───────────────────────────┘  │
│                          │                              │
│  ┌───────────────────────▼───────────────────────────┐  │
│  │              Donanım Katmanı                        │  │
│  │  CPU (x86) | RAM | Klavye | VGA Ekran             │  │
│  └───────────────────────────────────────────────────┘  │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

Detaylı mimari ve sistem akışı için bkz. [System Flow Diagram](https://www.google.com/search?q=system-flow.md).

## 🚀 Derleme ve Çalıştırma

### Ön Gereksinimler

```bash
# Ubuntu/Debian
sudo apt-get install build-essential nasm grub-pc-bin xorriso qemu-system-x86

# Arch Linux
sudo pacman -S base-devel nasm grub xorriso qemu-arch-extra

# macOS (with Homebrew)
brew install nasm i386-elf-gcc i386-elf-binutils qemu
```

### Derleme

```bash
# Depoyu klonlayın
git clone https://github.com/yourusername/kfs-1.git
cd kfs-1

# Çekirdeği derleyin
make

# Önyüklenebilir ISO oluşturun
make iso

# Derleme çıktılarını temizleyin
make clean
```

### Çalıştırma

```bash
# QEMU'da çalıştırın
make run

# Hata ayıklama ile çalıştırın
make debug

# QEMU'da monitör ile çalıştırın
qemu-system-i386 -cdrom kfs.iso -monitor stdio
```

### GDB ile Hata Ayıklama

```bash
# Terminal 1: QEMU'yu GDB sunucusu ile başlatın
qemu-system-i386 -cdrom kfs.iso -s -S

# Terminal 2: GDB'ye bağlanın
gdb build/kernel.bin
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

## 📁 Proje Yapısı

```
kfs-1/
├── arch/                      # Mimariye özel kod
│   ├── boot/                  # Önyükleme ve düşük seviye kod
│   │   ├── boot.s            # Önyükleyici giriş noktası
│   │   ├── io.h              # I/O port işlemleri
│   │   └── linker.ld         # Bağlayıcı betiği
│   ├── gdt/                   # Global Descriptor Table
│   │   ├── gdt.c             # GDT uygulaması
│   │   ├── gdt.h             # GDT başlık dosyası
│   │   └── gdt_load.s        # GDT yükleme assembly kodu
│   ├── idt/                   # Interrupt Descriptor Table
│   │   ├── idt.c             # IDT uygulaması
│   │   ├── idt.h             # IDT başlık dosyası
│   │   └── idt_load.s        # IDT yükleme assembly kodu
│   └── pic/                   # Programlanabilir Kesme Kontrolcüsü
│       ├── pic.c             # PIC uygulaması
│       ├── pic.h             # PIC başlık dosyası
│       └── isr_irq1.s        # Klavye ISR
├── hw/                        # Donanım sürücüleri
│   ├── keyboard/             # Klavye sürücüsü
│   │   ├── keyboard.c        # Klavye işleyicisi
│   │   ├── keyboard.h        # Klavye başlık dosyası
│   │   ├── f_keys.c          # F-tuşu işleyicileri
│   │   └── utils.c           # Tarama kodu çevirisi
│   ├── screen/               # Ekran yönetimi
│   │   ├── screen.c          # Ekran tamponu & geçmişi
│   │   └── screen.h          # Ekran başlık dosyası
│   ├── shell/                # Kabuk uygulaması
│   │   ├── shell.c           # Komut işlemcisi
│   │   ├── shell.h           # Kabuk başlık dosyası
│   │   └── commands.c        # Kabuk komutları
│   └── vga/                  # VGA metin modu sürücüsü
│       ├── vga.c             # VGA uygulaması
│       └── vga.h             # VGA başlık dosyası
├── kernel/                    # Çekirdek merkezi
│   ├── kernel.c              # Ana çekirdek fonksiyonu
│   └── kernel.h              # Çekirdek başlık dosyası
├── lib/                       # Standart kütüphane fonksiyonları
│   ├── printf.c              # Printf uygulaması
│   ├── printf.h              # Printf başlık dosyası
│   ├── utils.c               # String yardımcı fonksiyonları
│   └── utils.h               # Yardımcı fonksiyonlar başlık dosyası
├── iso/                       # ISO önyükleme yapılandırması
│   └── boot/grub/grub.cfg    # GRUB yapılandırması
├── Makefile                   # Derleme sistemi
└── Dockerfile                 # Docker derleme ortamı
```

## 📚 Dokümantasyon

Her bileşen için ayrıntılı dokümantasyon:

### 🚀 Önyükleme & Başlatma

  - **[Boot Process](doc/boot-process.md)** - Sistemin açılışından çekirdeğe kadar olan süreç

      - GRUB önyükleyici yapılandırması
      - Multiboot spesifikasyonu detayları
      - Assembly giriş noktası (boot.s)
      - Yığın kurulumu ve çekirdeğe geçiş
      - Önyükleme sonrası bellek düzeni
      - Önyükleme süreci akış şemaları

  - **[Multiboot Specification](doc/MULTIBOOT.md)** - Önyükleyici-çekirdek arayüzü

      - Multiboot başlık yapısı
      - Sihirli sayılar ve sağlama toplamları
      - GRUB entegrasyon rehberi

### 🔧 Temel Bileşenler

  - **[GDT (Global Descriptor Table)](doc/GDT.md)** - Bellek segmentasyonu ve koruması

      - GDT nedir ve neden gereklidir
      - Flat ve Segmented bellek modelleri
      - Segment descriptor yapısı (base, limit, access, granularity)
      - Ring seviyeleri (ayrıcalık seviyeleri 0-3)
      - Kod/Veri/Yığın segmentleri

  - **[IDT (Interrupt Descriptor Table)](doc/IDT.md)** - Kesme ve istisna yönetimi

      - IDT giriş yapısı ve formatı
      - CPU istisnaları (0-31) vs Donanım IRQ'ları (32-255)
      - ISR (Interrupt Service Routine) uygulaması
      - Kapı tipleri (Interrupt Gate, Trap Gate, Task Gate)
      - Assembly sapması + C işleyicisi modeli
      - LIDT komutu ile IDT yükleme

  - **[8259 PIC](doc/8259A.md)** - Programlanabilir Kesme Kontrolcüsü

      - PIC mimarisi (Master + Slave)
      - PIC başlatma ve yeniden haritalama
      - IRQ maskeleme/maske kaldırma işlemleri
      - EOI (Kesme Sonu) sinyali
      - Kaskad modu yapılandırması

### 🖥️ Donanım Sürücüleri

  - **[Keyboard Driver](doc/keyboard.md)** - PS/2 klavye giriş yönetimi

      - Kesme güdümlü vs yoklama yaklaşımları
      - Tarama kodundan ASCII'ye çeviri
      - Döngüsel tampon kuyruğu uygulaması
      - Tuş durumu takibi (Shift, Ctrl, Alt, Caps Lock)
      - Tuş tekrarı işlevselliği
      - Özel tuşlar (F tuşları, Ok tuşları)

  - **[VGA Text Mode](doc/vga.md)** - Ekran çıktı sürücüsü

      - VGA Mod 3 (80x25 metin modu)
      - VGA tampon yapısı (0xB8000)
      - Renk paleti ve nitelikler
      - Donanım imleç kontrolü
      - Kaydırma uygulaması

  - **[Shell](doc/shell.md)** - Etkileşimli komut satırı arayüzü

      - Komut ayrıştırma ve çalıştırma
      - Komut geçmişi ve gezinme
      - Satır düzenleme özellikleri
      - Dahili komutlar (help, clear, stack, reboot, halt)
      - Genişletilebilir komut sistemi

### 📊 Sistem Tasarımı

  - **[System Flow Diagram](doc/system-flow.md)** - Tüm sistem mimarisi
      - Önyükleme süreci akışı
      - Kesme işleme sırası
      - Bellek düzeni görselleştirmesi
      - Durum diyagramları
      - Tüm bileşenler için Mermaid diyagramları

### 📝 Proje Durumu

  - **[KFS-1 Status](doc/README-KFS1-STATUS.md)** - Uygulama durumu ve TODO listesi
      - Tamamlanan özellikler
      - Devam eden çalışmalar
      - Gelecekteki geliştirmeler

## 🖥️ Kabuk Komutları

Çekirdek, aşağıdaki komutları içeren etkileşimli bir kabuk içerir:

| Komut | Açıklama | Örnek |
|---------|-------------|---------|
| `help` | Tüm mevcut komutları gösterir | `help` |
| `clear` | Ekranı temizler ve imleci sıfırlar | `clear` |
| `stack` | Çekirdek yığın bilgilerini gösterir (ESP, EBP, kullanım) | `stack` |
| `reboot` | Sistemi klavye denetleyicisi aracılığıyla yeniden başlatır | `reboot` |
| `halt` | CPU'yu durdurur (CLI + HLT) | `halt` |

### Klavye Kısayolları

  - **Enter**: Komutu çalıştırır
  - **Backspace**: İmleçteki karakteri siler
  - **Delete**: İmlecin altındaki karakteri siler
  - **Tab**: Komut geçmişinde gezinir (yukarı)
  - **Arrow Keys**: Satır ve geçmiş içinde gezinir
  - **F1-F10**: Özel fonksiyon tuşları (genişletilebilir)

## 🔧 Teknik Detaylar

### Önyükleme Süreci

1.  **GRUB Önyükleyici** çekirdeği 1MB (0x100000) adresine yükler
2.  **Multiboot sihirli sayısı** (0x2BADB002) doğrulaması
3.  **Yığın kurulumu** (16KB yığın)
4.  `kernel_main` fonksiyonuna atlanır

### Başlatma Sırası

```
1. Kesmeleri devre dışı bırak (CLI)
2. VGA metin modunu başlat
3. Multiboot sihirli sayısını doğrula
4. GDT'yi kur (3 segment: null, kernel kodu, kernel verisi)
5. GDT'yi yükle ve segment register'larını yeniden yükle
6. IDT'yi kur (256 kesme vektörü)
7. 8259 PIC'i yeniden haritala (IRQ0-15 → INT 0x20-0x2F)
8. Klavye işleyicisini kur (IRQ1 → INT 0x21)
9. Kesmeleri etkinleştir (STI)
10. Ana olay döngüsüne gir
```

### Bellek Düzeni

```
0x00000000 - 0x000003FF   Real Mode IVT (kullanılmıyor)
0x00000400 - 0x000004FF   BIOS Veri Alanı
0x00000500 - 0x00007BFF   Boş geleneksel bellek
0x00007C00 - 0x00007DFF   Önyükleyici
0x00000800 - 0x00000FFF   GDT konumu
0x00010000 - 0x0009FFFF   Genişletilmiş geleneksel bellek
0x000A0000 - 0x000BFFFF   Video belleği
0x000B8000 - 0x000B8FA0   VGA metin tamponu (80x25)
0x000C0000 - 0x000FFFFF   BIOS ROM
0x00100000 - ...          Çekirdek buraya yüklenir (1MB)
Yığın: 16KB (stack_top'tan aşağı doğru büyür)
```

### Kesme Yönetimi

```c
// IRQ0-7  → INT 0x20-0x27 (Master PIC)
// IRQ8-15 → INT 0x28-0x2F (Slave PIC)

IRQ0  → Zamanlayıcı (kullanılmıyor)
IRQ1  → Klavye (0x21) ✓ Uygulandı
IRQ2  → Kaskad (dahili)
IRQ3-7 → COM2, COM1, LPT2, Floppy, LPT1
IRQ8-15 → RTC, ACPI, vb.
```

### Klavye Tarama Kodu Yönetimi

1.  Tuşa basma/bırakma donanım kesmesi (IRQ1) oluşturur
2.  ISR, 0x60 portundan tarama kodunu okur
3.  Tarama kodu döngüsel tampona kuyruğa alınır
4.  Ana döngü tamponu yoklar ve tarama kodunu ASCII'ye çevirir
5.  Karakter görüntülenir veya komut çalıştırılır

## 📖 Kaynaklar

### Resmi Dokümantasyon

  - [Intel 64 ve IA-32 Mimarileri Yazılım Geliştirici Kılavuzları](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
  - [OSDev Wiki](https://wiki.osdev.org/)
  - [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)

### Eğitimler & Referanslar

  - [Sıfırdan Basit bir İşletim Sistemi Yazmak](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)
  - [James Molloy'un Çekirdek Eğitimleri](http://www.jamesmolloy.co.uk/tutorial_html/)
  - [Bran'ın Çekirdek Geliştirme Eğitimi](http://www.osdever.net/bkerndev/Docs/intro.htm)

### Donanım Referansları

  - [8259A PIC Datasheet](https://pdos.csail.mit.edu/6.828/2008/readings/hardware/8259A.pdf)
  - [VGA Hardware](http://www.osdever.net/FreeVGA/vga/vga.htm)
  - [Keyboard Scancodes](https://www.win.tue.nl/~aeb/linux/kbd/scancodes.html)

## 🎓 Öğrenme Çıktıları

Bu projeyi tamamladıktan sonra şunları anlayacaksınız:

  - ✅ Bir bilgisayarın açılıştan çekirdek çalıştırılmasına kadar nasıl önyüklendiği
  - ✅ Korumalı mod ve bellek segmentasyonu
  - ✅ Kesme yönetimi ve donanım iletişimi
  - ✅ Cihaz sürücüsü geliştirme (klavye, VGA)
  - ✅ Düşük seviyeli C ve assembly programlama
  - ✅ Bağlayıcı betikleri ve bellek düzeni
  - ✅ GDB ve QEMU ile çekirdek seviyesinde kod hata ayıklama

## 🤝 Katkıda Bulunma

Bu bir eğitim projesidir. Çekinmeyin:

  - Hataları veya sorunları bildirin
  - İyileştirmeler önerin
  - Yeni özellikler ekleyin
  - Dokümantasyonu iyileştirin

## 👨‍💻 Yazar

**Sahipler**

  - [tkurukay](https://github.com/TufanKurukaya)
  - [mula](https://github.com/MustafaUla)

-----

*42 Okulu müfredatının bir parçası olarak ❤️ ile oluşturulmuştur*