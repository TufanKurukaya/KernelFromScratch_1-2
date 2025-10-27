# Makefile
TARGET   := kernel.bin
ISO      := kfs.iso
BUILD    := build
SRC_DIR  := src
ASM      := nasm
CC       := x86_64-elf-gcc
LD       := x86_64-elf-ld
STRIP    := x86_64-elf-objcopy


ASMFLAGS := -f elf32 #-g -F dwarf
CFLAGS   := -m32 -ffreestanding -fno-builtin -fno-stack-protector -Os -Wall -Wextra -nostdlib -nodefaultlibs -fno-unwind-tables -g
LDFLAGS  := -m elf_i386 -T linker.ld -nostdlib -g

SRCS_C   := $(SRC_DIR)/kernel.c $(SRC_DIR)/idt.c $(SRC_DIR)/pic.c $(SRC_DIR)/gdt.c $(SRC_DIR)/utils.c $(SRC_DIR)/printf.c $(SRC_DIR)/screen.c $(SRC_DIR)/keyboard.c
SRCS_S   := $(SRC_DIR)/assambly/boot.s $(SRC_DIR)/assambly/idt_load.s $(SRC_DIR)/assambly/isr_irq1.s $(SRC_DIR)/assambly/gdt_load.s
OBJS     := $(BUILD)/boot.o $(BUILD)/kernel.o $(BUILD)/idt.o $(BUILD)/idt_load.o $(BUILD)/pic.o $(BUILD)/isr_irq1.o $(BUILD)/gdt.o $(BUILD)/gdt_load.o $(BUILD)/utils.o $(BUILD)/printf.o $(BUILD)/screen.o $(BUILD)/keyboard.o

.PHONY: all clean run iso

all: $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(SRC_DIR)/assambly/%.s | $(BUILD)
	$(ASM) $(ASMFLAGS) $< -o $@

$(BUILD)/%.o: $(SRC_DIR)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)
	$(STRIP) --strip-unneeded $@

iso: $(TARGET)
	mkdir -p iso/boot/grub
	cp $(TARGET) iso/boot/
	i686-elf-grub-mkrescue --locales="" --fonts="" --themes="" \
		--modules="multiboot iso9660 normal" \
		-o $(ISO) iso
		
run: iso
	qemu-system-i386 -cdrom $(ISO)

run-k: $(TARGET)
	qemu-system-i386 -kernel $(TARGET)

	
clean:
	rm -rf $(BUILD) 

fclean: clean
	rm -rf $(ISO) $(TARGET) 
	rm -rf iso/boot/kernel.bin

info:
	objdump -h $(TARGET)
	readelf -l $(TARGET)

re: fclean all
