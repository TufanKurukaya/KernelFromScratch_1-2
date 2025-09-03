# Makefile
TARGET   := kernel.bin
ISO      := kfs.iso
BUILD    := build
SRC_DIR  := src
ASM      := nasm
CC       := gcc
LD       := ld

ASMFLAGS := -f elf32
CFLAGS   := -m32 -ffreestanding -fno-builtin -fno-stack-protector -O2 -Wall -Wextra -nostdlib -nodefaultlibs
LDFLAGS  := -m elf_i386 -T linker.ld -nostdlib

SRCS_C   := $(SRC_DIR)/kernel.c
SRCS_S   := $(SRC_DIR)/boot.s
OBJS     := $(BUILD)/boot.o $(BUILD)/kernel.o

.PHONY: all clean run iso

all: $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: $(SRCS_S) | $(BUILD)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD)/kernel.o: $(SRCS_C) | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

iso: $(TARGET)
	mkdir -p iso/boot/grub
	cp $(TARGET) iso/boot/
	grub-mkrescue -o $(ISO) iso

run: iso
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf $(BUILD) $(TARGET) $(ISO)
	rm -rf iso/boot/kernel.bin
