
# x86 GDT, Segment Limit ve Flat Memory Model – Kısa Doküman

Bu doküman, GDT’nin neden sıklıkla “tüm adres uzayını kapsayan” (4 GB) segment limitleri ile kurulduğunu,
granularity (G) bitinin limit hesabına etkisini ve Multiboot + flat model altında tipik kullanımı özetler.
Ayrıca istenirse segment-bazlı sınırlandırmanın nasıl yapılabileceğini de gösterir.

---

## 1) Büyük Resim: CPU → Segmentasyon → Paging

           +--------------------+
           |        CPU         |
           +--------------------+
                      |
                      |  CS/DS/SS... = Segment Selector (index + RPL)
                      v
           +--------------------+
           |   Segmentation     |  (GDT / LDT)
           |  base + limit +    |
           |  access/gran       |
           +--------------------+
                      |
                      |  Linear Address (virtual, 32-bit)
                      v
           +--------------------+
           |      Paging        |  (CR3, Page Directory/Tables)
           |  4KB/2MB/4MB      |
           |  sayfa çevirimi    |
           +--------------------+
                      |
                      |  Physical Address
                      v
           +--------------------+
           |     RAM / MMIO     |
           +--------------------+

**Önemli nokta:** Modern kernel’ler “flat memory model” kullanır. Segmentler yalnızca ayrıcalık seviyesi (ring) ve
varsayılan kod/veri tipleri için kullanılır; adres alanının sınırlandırılması ise **paging** ile yapılır.
Bu nedenle segment limitleri genellikle **tüm lineer adres uzayını** kapsayacak şekilde ayarlanır.

---

## 2) Segment Descriptor ve Limit Hesabı

Bir GDT girdisi (descriptor) kabaca şunları içerir:

- **Base** (tabanın 32 biti): segmentin başlangıç lineer adresi  
- **Limit** (20 bit): segment uzunluğu
- **Access** (ör. `0x9A`, `0x92`, `0xFA`, `0xF2`): tür, ring, present vb.
- **Granularity (G) + Flags** (ör. üst nibble `0xC?`): limitin birimini ve boyutu etkiler

**Granularity (G) bitinin etkisi:**

- `G = 0` → limit **bayt** cinsinden. `limit = 0xFFFF` → ~64 KB.
- `G = 1` → limit **4 KB sayfa** cinsinden. `limit = 0xFFFFF` → 1,048,575 × 4 KB ≈ **4 GB**.

Senin örneklerinde `gran = 0xCF` → G=1 olduğu için `limit = 0xFFFFF` tüm 4 GB’ı kapsar.

---

## 3) Neden Tüm Segmentlerde Limit 0xFFFFF? (Flat Model Mantığı)

- Segmentler **adreslemeyi** değil, **erişim seviyesini** (ring 0/ring 3) ve kod/veri ayrımını taşır.
- Adres uzayı **paging** ile yönetilir (koruma, izolasyon, kopya-üzerine-yaz vb.).
- Bu yüzden tipik GDT kurulumunda:
  - `CS` (ring 0 kod) → 4 GB
  - `DS/SS` (ring 0 veri/stack) → 4 GB
  - `CS` (ring 3 kod) → 4 GB
  - `DS/SS` (ring 3 veri/stack) → 4 GB

Buna **flat memory model** denir ve Linux/Windows/BSD gibi sistemler de bu yaklaşımı kullanır.

---

## 4) İstenirse Segment-Bazlı Sınırlandırma Nasıl Yapılır?

Segmentleri gerçekten **farklı taban/limit** ile kısıtlamak mümkün, ancak modern tasarımda nadir tercih edilir
(çünkü paging ile çakışan/karmaşıklaşan bir koruma modeli yaratır). Yine de örnek:

```c
// Kernel code: 0x00000000-0x000FFFFF (1 MB)
gdt_set(1, 0x00000000, 0x000FFFFF, 0x9A, 0xCF);

// Kernel data: 0x00100000-0x001FFFFF (1 MB)
gdt_set(2, 0x00100000, 0x000FFFFF, 0x92, 0xCF);

// User code: 0x00400000-0x007FFFFF (4 MB)
gdt_set(4, 0x00400000, 0x003FFFFF, 0xFA, 0xCF);

// User data: 0x00800000-0x00BFFFFF (4 MB)
gdt_set(5, 0x00800000, 0x003FFFFF, 0xF2, 0xCF);
```

Not: G=1 iken limit sayfa cinsinden yorumlanır. Tam byte hassasiyetiyle sınır istenirse G=0 kullanılabilir,
ancak bu da ~64 KB sınırına takılmanıza neden olur (klasik 16-bit tarzı).

## 5) Access Byte Örnekleri (Sık Kullanılanlar)

| Descriptor | Değer | Anlamı (özet)                     |
| ---------: | :---: | --------------------------------- |
|  Kernel CS |  0x9A | Code, exec/read, ring 0, present  |
|  Kernel DS |  0x92 | Data, read/write, ring 0, present |
|    User CS |  0xFA | Code, exec/read, ring 3, present  |
|    User DS |  0xF2 | Data, read/write, ring 3, present |

Üst nibble’daki 0xC? türü değerler granularity + size (ör. 32-bit) gibi bayrakları temsil eder (ör. 0xCF).


## 6) Multiboot Bağlamı ve 0x00000800 Gereksinimi


- GRUB çekirdeğini zaten protected mode’da yükler; kendi GDT’si aktif olabilir.

- Ödev/test gereği “GDT 0x00000800’de” isteniyorsa:

  1. GDT’yi .data içinde tanımla,

  2. Boot’ta 0x00000800 adresine kopyala,

  3. lgdt ile base=0x00000800 olan GDTR’ı yükle,

  4. Segment register’ları yeniden yükle (far jump + DS/ES/SS vb.).

Bu, kernel’in 1 MB’ten (. = 1M) başlaması ile çelişmez; yalnızca GDT’nin RAM’de konacağı sabit konumdur.

## 7) Kısa Özet (TL;DR)

- limit=0xFFFFF + G=1 ⇒ 4 GB segment → flat model.

- Modern kernel’lerde segmentler ring ayrımı içindir, adres alanını paging sınırlar.

- İstenirse farklı base/limit verilebilir ama pratikte nadir ve daha karmaşıktır.

- Multiboot’ta GDT’yi 0x800’e koymak bir protokol/ödev şartı olabilir; runtime’da kopyalayıp lgdt ile yükle.