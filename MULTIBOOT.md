
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
