; src/idt_load.s (NASM, 32-bit)
BITS 32
GLOBAL idt_load
EXTERN idtp

idt_load:
    lidt [idtp]
    ret
