# ==========================================================
# 🔧 KFS Kernel Makefile
# ==========================================================

TARGET   := kernel.bin
ISO      := kfs.iso
BUILD    := build

ASM      := nasm
CC       := gcc
LD       := ld
STRIP    := objcopy

ASMFLAGS := -f elf32
CFLAGS   := -m32 -ffreestanding -fno-builtin -fno-stack-protector -Os -Wall -Wextra -nostdlib -nodefaultlibs -fno-unwind-tables -g
LDFLAGS  := -m elf_i386 -T arch/boot/linker.ld -nostdlib -g

# ==========================================================
# 📁 Kaynak dosyalar
# ==========================================================

# Tüm .c ve .s dosyalarını otomatik bul
SRC_C := $(shell find arch kernel hw lib -type f -name "*.c")
SRC_S := $(shell find arch -type f -name "*.s")

# .o uzantılı hedefleri build klasörüne çevir
OBJ_C := $(SRC_C:%.c=$(BUILD)/%.o)
OBJ_S := $(SRC_S:%.s=$(BUILD)/%.o)
OBJS  := $(OBJ_C) $(OBJ_S)

# ==========================================================
# 🎯 Kurallar
# ==========================================================

.PHONY: all clean fclean iso run run-k info docker re

all: $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

# C kaynaklarını derle
$(BUILD)/%.o: %.c | $(BUILD)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assembly kaynaklarını derle
$(BUILD)/%.o: %.s | $(BUILD)
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) $< -o $@

# Linkleme
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	$(STRIP) --strip-unneeded $@

# ==========================================================
# 💿 ISO oluşturma ve QEMU ile çalıştırma
# ==========================================================

iso: $(TARGET)
	mkdir -p iso/boot/grub
	cp $(TARGET) iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "KFS Kernel" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/$(TARGET)' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --locales="" --fonts="" --themes="" \
		--modules="multiboot iso9660 normal" \
		-o $(ISO) iso

run: iso
	qemu-system-i386 -cdrom $(ISO)

run-k: $(TARGET)
	qemu-system-i386 -kernel $(TARGET)

# ==========================================================
# 🧹 Temizlik
# ==========================================================

clean:
	rm -rf $(BUILD)

fclean: clean
	rm -rf $(ISO) $(TARGET) iso

re: fclean all

# ==========================================================
# 🧠 Bilgi / Docker
# ==========================================================

info:
	objdump -h $(TARGET)
	readelf -l $(TARGET)

docker:
	docker build -t kfs-build .
	docker run --rm -v $$(pwd):/src kfs-build make iso
