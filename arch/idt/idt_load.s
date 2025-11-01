; src/assambly/idt_load.s
BITS 32
GLOBAL idt_load
EXTERN idtp

idt_load:
    lidt [idtp]
    ret
