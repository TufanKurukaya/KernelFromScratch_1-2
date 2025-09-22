; src/assambly/gdt_load.s
BITS 32
GLOBAL gdt_load
GLOBAL gdt_reload_segments
EXTERN gdtp

gdt_load:
    lgdt [gdtp]
    jmp 0x08:.flush
.flush:
    ret

gdt_reload_segments:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
