# Makefile
TARGET   := kernel.bin
ISO      := kfs.iso
BUILD    := build
SRC_DIR  := src
ASM      := nasm
CC       := gcc
LD       := ld
STRIP    := objcopy

ASMFLAGS := -f elf32
CFLAGS   := -m32 -ffreestanding -fno-builtin -fno-stack-protector -Os -Wall -Wextra -nostdlib -nodefaultlibs -fno-unwind-tables
LDFLAGS  := -m elf_i386 -T linker.ld -nostdlib

SRCS_C   := $(SRC_DIR)/kernel.c $(SRC_DIR)/idt.c $(SRC_DIR)/pic.c
SRCS_S   := $(SRC_DIR)/boot.s $(SRC_DIR)/idt_load.s $(SRC_DIR)/isr_irq1.s
OBJS     := $(BUILD)/boot.o $(BUILD)/kernel.o $(BUILD)/idt.o $(BUILD)/idt_load.o $(BUILD)/pic.o $(BUILD)/isr_irq1.o

.PHONY: all clean run iso

all: $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(SRC_DIR)/%.s | $(BUILD)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	$(STRIP) --strip-unneeded $@

iso: $(TARGET)
	mkdir -p iso/boot/grub
	cp $(TARGET) iso/boot/
	grub-mkrescue --locales="" --fonts="" --themes="" \
		--modules="multiboot iso9660 normal" \
		-o $(ISO) iso
		
run: iso
	qemu-system-i386 -cdrom $(ISO)

run-k: $(TARGET)
	qemu-system-i386 -kernel $(TARGET)

	
clean:
	rm -rf $(BUILD) $(TARGET) $(ISO)
	rm -rf iso/boot/kernel.bin

info:
	objdump -h $(TARGET)
	readelf -l $(TARGET)
