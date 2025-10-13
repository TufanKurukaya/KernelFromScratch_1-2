
Multiboot_info
---
| Bit    | Hex mask   | Alan(lar)                           | Anlamı                                                       |
| ------ | ---------- | ----------------------------------- | ------------------------------------------------------------ |
| **0**  | 0x00000001 | `mem_lower`, `mem_upper`            | 1 MB altı ve üstü bellek boyutları geçerli. (KB cinsinden)   |
| **1**  | 0x00000002 | `boot_device`                       | Kernel’in hangi cihazdan boot edildiği bilgisi var.          |
| **2**  | 0x00000004 | `cmdline`                           | Kernel komut satırı (`grub.cfg` içinden) adresi geçerli.     |
| **3**  | 0x00000008 | `mods_count`, `mods_addr`           | Yüklenmiş modüller (initrd, ramdisk vb.) bilgisi geçerli.    |
| **4**  | 0x00000010 | a.out sembol tablosu                | Kernel a.out formatında ise sembol bilgileri geçerli.        |
| **5**  | 0x00000020 | ELF section header tablosu          | Kernel ELF formatında ise section header bilgileri geçerli.  |
| **6**  | 0x00000040 | `mmap_addr`, `mmap_length`          | Bellek haritası (hangi bölge RAM, hangisi rezervli) geçerli. |
| **7**  | 0x00000080 | `drives_addr`, `drives_length`      | BIOS disk sürücüleri listesi geçerli.                        |
| **8**  | 0x00000100 | `config_table`                      | ROM config table (eski makineler için) geçerli.              |
| **9**  | 0x00000200 | `boot_loader_name`                  | Kullanılan bootloader’ın adı geçerli (örn. “GRUB 2.06”).     |
| **10** | 0x00000400 | `apm_table`                         | APM (Advanced Power Management) tabloları geçerli.           |
| **11** | 0x00000800 | `vbe_control_info`, `vbe_mode_info` | VESA BIOS Extension (grafik mod bilgisi) geçerli.            |
| **12** | 0x00001000 | `framebuffer_addr`, `framebuffer_*` | Framebuffer bilgisi (modern grafik modu) geçerli.            |

Boot Loader karşılaştırma
---
| Özellik                | GRUB                       | Limine + Stivale2                  |
| ---------------------- | -------------------------- | ---------------------------------- |
| Amaç                   | Genel amaçlı bootloader    | OS geliştiriciler için optimize    |
| Protokol               | Multiboot v1/v2            | Limine protokolü, Stivale/Stivale2 |
| Destek                 | Birçok OS, eski sistemler  | Özellikle yeni kernel’ler          |
| Karmaşıklık            | Ağır, config dosyaları var | Çok basit, tek binary yeterli      |
| Geliştirme hızı        | Eski, yavaş                | Aktif, modern                      |
| Kernel bilgisi aktarma | Multiboot yapıları         | Çok daha düzenli Stivale2 yapıları |

MULTIBOOT_FLAGS bitleri
---

| Bit   | Anlamı                               | Açıklama                                                                   | 
| ----- | ------------------------------------ | -------------------------------------------------------------------------- | 
| 0     | Page-align modules                   | Modülleri 4 KB hizalı yükle                                                | 
| 1     | Memory info required                 | Bellek bilgisi verilmezse boot etme                                        | 
| 2     | Video mode request                   | Grafik modu iste                                                           | 
| 3–15  | Reserved                             | Kullanılmaz, hep 0 olmalı                                                  | 
| 16    | AOUT kludge (adres alanları geçerli) | load\_addr, load\_end\_addr, bss\_end\_addr, entry\_addr alanlarını kullan | 
| 17–31 | Reserved                             | Kullanılmaz, hep 0 olmalı                                                  | 

outb inb Portlar
---
| Port  | Cihaz           | Görev                               |
| ----- | --------------- | ----------------------------------- |
| 0x3D4 | VGA (CRTC)      | Hangi cursor register’ını seçer     |
| 0x3D5 | VGA (CRTC)      | Cursor register değerini okur/yazar |
| 0x20  | Master PIC CMD  | Komut (init, EOI vs)                |
| 0x21  | Master PIC DATA | Maske/ayar                          |
| 0xA0  | Slave PIC CMD   | Komut                               |
| 0xA1  | Slave PIC DATA  | Maske/ayar                          |


Uniq Key Scancode
---
| Key     | Value |
| ------- | ----- |
| ALT     |  56   |
| CTRL    |  29   |
| L-SHIFT |  42   |
| R-SHIFT |  54   |
| CAPSLCK |  58   |
| ESC     |  1    |
| DEL     |  83   |
| R-ARROW |  77   |
| L-ARROW |  75   |
| U-ARROW |  72   |
| ALT-GR  |  56   |
| D-ARROW |  80   |


Segment register'ı
---
| Register | Ne için kullanılır?                   | Senin durumda değer  |
| -------- | ------------------------------------- | -------------------- |
| **CS**   | Kod (instruction fetch)               | `0x08` (kernel code) |
| **DS**   | Veri (normal değişkenler)             | `0x10` (kernel data) |
| **SS**   | Stack (push/pop/call/ret)             | `0x10` (kernel data) |
| **ES**   | Ek veri, string komut hedefi          | `0x10` (kernel data) |
| **FS**   | Ek veri, modern OS’te thread-local    | `0x10` (kernel data) |
| **GS**   | Ek veri, modern OS’te CPU/thread info | `0x10` (kernel data) |

Access byte 0x9A
---
| Bit(ler) | Adı (alan)          | Değer | Anlamı (kod segmenti için)                                                              |
| -------- | ------------------- | ----: | --------------------------------------------------------------------------------------- |
| 7        | Present (P)         |     1 | Segment bellek­te mevcut; değilse erişimde #NP hatası.                                  |
| 6–5      | DPL                 |    00 | Ayrıcalık seviyesi Ring 0.                                                              |
| 4        | Descriptor Type (S) |     1 | Kod/veri segmenti (sistem segmenti değil).                                              |
| 3        | Executable (E)      |     1 | Bu bir **kod** segmenti.                                                                |
| 2        | Conforming (C)      |     0 | **Non-conforming**: Sadece aynı ayrıcalık seviyesinden (CPL = DPL) aktarım yapılabilir. |
| 1        | Readable (R)        |     1 | Kod okunabilir (yazılamaz).                                                             |
| 0        | Accessed (A)        |     0 | Henüz erişilmemiş; CPU erişince 1 yapar.                                                |


IDT gate “type/attributes” baytı (8 bit)
---
| Bit(ler) | Alan            | Anlam                                                                                                                                                                            |
| -------- | --------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 7        | **P (Present)** | 1 ise giriş geçerli; 0 ise vektör tetiklendiğinde “descriptor not present” hatası. ([wiki.osdev.org][1])                                                                         |
| 6–5      | **DPL**         | `INT` ile **yazılımsal** tetikleme yapabilmek için gereken en az ayrıcalık seviyesi (CPL ≤ DPL olmalı). Donanım kesmeleri/istisnalar DPL’yi kontrol etmez. ([wiki.osdev.org][1]) |
| 4        | **S**           | “Storage Segment” biti. **IDT gate’lerinde her zaman 0** (gate = sistem tanımlayıcısı). ([wiki.osdev.org][2])                                                                    |
| 3–0      | **Type**        | Kapı türünü belirtir (aşağıdaki tam tablo). ([wiki.osdev.org][2])                                                                                                                |

[1]: https://wiki.osdev.org/Interrupt_Descriptor_Table "Interrupt Descriptor Table - OSDev Wiki"
[2]: https://wiki.osdev.org/Descriptor "Descriptor - OSDev Wiki"



Gate Type
---
| b3..b0 | Hex | Adı/Tanım                      | IDT’de? | Not                                                                                                                          |
| -----: | --: | ------------------------------ | :-----: | ---------------------------------------------------------------------------------------------------------------------------- |
|   0000 | 0x0 | Rezerve                        |    ❌    | Kullanılmaz. ([scs.stanford.edu][1])                                                                                         |
|   0001 | 0x1 | 16-bit **TSS (Available)**     |    ❌    | Yalnız **GDT**’de kullanılır. ([scs.stanford.edu][1])                                                                        |
|   0010 | 0x2 | **LDT**                        |    ❌    | Yalnız GDT. ([scs.stanford.edu][1])                                                                                          |
|   0011 | 0x3 | 16-bit **TSS (Busy)**          |    ❌    | Yalnız GDT. ([scs.stanford.edu][1])                                                                                          |
|   0100 | 0x4 | **Call Gate (16-bit)**         |    ❌    | GDT/LDT’de; **IDT’ye konmaz**. ([wiki.osdev.org][2])                                                                         |
|   0101 | 0x5 | **Task Gate**                  |    ✅*   | 32-bit korumalı kipte görev geçişi; modern sistemlerde nadir. *x86-64 (IA-32e) kipte **desteklenmez**. ([wiki.osdev.org][3]) |
|   0110 | 0x6 | **Interrupt Gate (16-bit)**    |    ✅    | 16-bit ISR; girişte **IF=0** yapılır. ([wiki.osdev.org][2])                                                                  |
|   0111 | 0x7 | **Trap Gate (16-bit)**         |    ✅    | 16-bit ISR; **IF değişmez**. ([wiki.osdev.org][2])                                                                           |
|   1000 | 0x8 | Rezerve                        |    ❌    | Kullanılmaz. ([scs.stanford.edu][1])                                                                                         |
|   1001 | 0x9 | 32-bit **TSS (Available)**     |    ❌    | Yalnız GDT; 32-bit TSS seçicisi. ([scs.stanford.edu][4])                                                                     |
|   1010 | 0xA | Rezerve                        |    ❌    | Kullanılmaz. ([scs.stanford.edu][1])                                                                                         |
|   1011 | 0xB | 32-bit **TSS (Busy)**          |    ❌    | Yalnız GDT. ([scs.stanford.edu][4])                                                                                          |
|   1100 | 0xC | **Call Gate (32-bit)**         |    ❌    | GDT/LDT’de; **IDT’ye konmaz**. ([wiki.osdev.org][2])                                                                         |
|   1101 | 0xD | Rezerve                        |    ❌    | Kullanılmaz. ([scs.stanford.edu][1])                                                                                         |
|   1110 | 0xE | **Interrupt Gate (32/64-bit)** |    ✅    | 32-bit korumalı kipte 32-bit ISR; **x86-64’te 64-bit ISR**. IF temizlenir. ([wiki.osdev.org][2])                             |
|   1111 | 0xF | **Trap Gate (32/64-bit)**      |    ✅    | 32-bit korumalı kipte 32-bit ISR; **x86-64’te 64-bit ISR**. IF değişmez. ([wiki.osdev.org][2])                               |

[1]: https://www.scs.stanford.edu/05au-cs240c/lab/i386/s06_03.htm?utm_source=chatgpt.com "6.3 Segment-Level Protection"
[2]: https://wiki.osdev.org/Descriptor "Descriptor - OSDev Wiki"
[3]: https://wiki.osdev.org/Interrupt_Descriptor_Table "Interrupt Descriptor Table - OSDev Wiki"
[4]: https://www.scs.stanford.edu/05au-cs240c/lab/i386/s07_02.htm?utm_source=chatgpt.com "7.2 TSS Descriptor"
