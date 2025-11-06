# GDT (Global Descriptor Table)

## GDT Nedir?

Bilgisayarın işlemcisi (CPU), belleği tek bir büyük alan olarak göremez.  
Çalışabilmesi için “bu kısım kod için, bu kısım veri için” gibi etiketler ister.  
**GDT (Global Descriptor Table)** tam olarak bu etiketlerin listesidir.

CPU’nun segmentleme mekanizmasını tanımlar.  
Her **entry** (satır) bir segmenti tarif eder:

- **base** → Segmentin bellekte başladığı adres
- **limit** → Segmentin uzunluğu
- **access** → Kod mu, veri mi, hangi yetkide (Ring 0 / Ring 3)
- **granularity** → Limitin ölçü birimi (byte mı 4KB mı) ve 32-bit ayarları

### Neden Gerekli?

- **Korumalı moda geçmek için** CPU mutlaka bir GDT görmek ister.
- Segment register’ları (**CS**, **DS**, **SS** vb.) boş olamaz.
- GDT olmadan CPU gerçek modda kalır ve bellek koruması sağlanamaz.

### Segment Koruması

Korumalı mod sayesinde:
- Belleğin belirli bölümleri yalnızca **kod** veya **veri** olarak işaretlenebilir.
- Ring seviyeleri (0–3) ile çekirdek (kernel) ve kullanıcı (user) alanı ayrılır.
- Yanlış veya yetkisiz bellek erişimleri engellenir.

Eğer korumalı mod ve GDT kullanılmazsa:
- Tüm bellek tek bir blok gibi görünür.
- Programlar çekirdek alanına rahatlıkla yazabilir.
- Çökme veya güvenlik açıkları çok daha kolay oluşur.

## GDT Yapısı

Bir GDT girdisi bellekte aşağıdaki gibi 8 byte (64 bit) uzunluğundadır:

|<---- 16 ---->|<------ 16 ------>|<--8-->|<--8-->|<---4-->|<---4-->|<--8-->|
| limit_low | base_low |base_mid|access|limit_hi| gran |base_hi|


Bu proje 3 adet GDT girişi tanımlar:

| Entry | Segment | Açıklama |
|------:|--------|----------|
| 0 | **Null Segment** | Her zaman boş, güvenlik için |
| 1 | **Kernel Code Segment** | Çekirdeğin kodları için |
| 2 | **Kernel Data Segment** | Çekirdeğin verileri için |

## Kod Örneği

Aşağıdaki kod, bu 3 segmenti tanımlar:

```c
gdt_set(0, 0, 0, 0, 0);             // Null Segment
gdt_set(1, 0, 0xFFFFF, 0x9A, 0xCF); // Kernel Code Segment
gdt_set(2, 0, 0xFFFFF, 0x92, 0xCF); // Kernel Data Segment

```


# IDT (Interrupt Descriptor Table)

IDT, CPU’ya “hangi interrupt/exception geldiğinde hangi fonksiyona gideceğini” söyleyen tablodur.  
Korumalı modda veya kernel geliştirmede IDT olmazsa interrupt/exception’lar işlenemez ve sistem çöker.

## IDT Nedir?

- **Exceptions:** CPU’dan kaynaklanan hatalar (divide by zero, page fault, general protection, …)
- **Hardware Interrupts (IRQ):** Dış cihazlardan gelen sinyaller (timer, klavye, vs.)

IDT, her vektör numarası (0–255) için hangi ISR (Interrupt Service Routine) çağrılacağını belirtir.  
CPU, interrupt geldiğinde IDT’de ilgili vektör numarasına bakar ve handler adresine gider.

## IDT Entry Yapısı

Her IDT girişi 8 byte uzunluğundadır:

```text
| base_lo (16) | sel (16) | always0 (8) | flags (8) | base_hi (16) |