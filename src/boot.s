; src/boot.s
; NASM syntax (32-bit)

BITS 32
GLOBAL start
EXTERN kernel_main

; --- Multiboot header (GRUB'un bizi kernel olarak tanıması için) ---
; Magic   : 0x1BADB002
; Flags   : 0x00000000 (basit)
; Checksum: Magic + Flags + Checksum = 0 olacak şekilde

SECTION .multiboot
align 4
MULTIBOOT_MAGIC   equ 0x1BADB002
MULTIBOOT_FLAGS   equ 0
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

dd MULTIBOOT_MAGIC
dd MULTIBOOT_FLAGS
dd MULTIBOOT_CHECKSUM

; --- Stack alanı ---
SECTION .bss
align 16
stack_bottom:
    resb 16384           ; 16 KB stack
stack_top:

; --- Giriş noktası ---
SECTION .text
start:
    ; Basit bir stack kur
    mov esp, stack_top
    
    push ebx
    push eax
    ; kernel_main() çağır
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
