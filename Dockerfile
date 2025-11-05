# Temel imaj olarak Ubuntu 22.04 (LTS) kullanalım.
# Geliştirme araçları için stabil bir temel sağlar.
FROM ubuntu:22.04

# Kurulum sırasında interaktif sorguları (örn. zaman dilimi) atlamak için
ENV DEBIAN_FRONTEND=noninteractive

# Gerekli paketleri kur
# RUN komutunu tek bir katmanda birleştirerek imaj boyutunu optimize ediyoruz.
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    grub-pc-bin \
    xorriso \
    make \
    nasm \
    grub-efi-amd64-bin \
    mtools \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# build-essential: gcc, g++, ld (binutils aracılığıyla), make vb. içerir.
# grub-pc-bin:   'grub-mkrescue' komutunu sağlar.
# xorriso:       'grub-mkrescue' tarafından ISO oluşturmak için kullanılır.
# make:          Genellikle C projelerinde derleme sürecini otomatikleştirmek için kullanılır.
# nasm:          Assembly (topluluk) kodu için gerekebilir (opsiyonel ama genelde gerekir).

# Konteyner başladığında kaynak kodun bulunacağı varsayılan dizin
WORKDIR /src

# Konteynerin varsayılan olarak interaktif bir shell başlatmasını sağla
CMD ["/bin/bash"]