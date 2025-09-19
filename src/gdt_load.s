; gdt_load.s (NASM)
BITS 32
GLOBAL gdt_load
GLOBAL gdt_reload_segments
EXTERN gdtp

gdt_load:
    lgdt [gdtp]
    ; far jump ile CS'yi KERNEL_CS (0x08) yap
    jmp 0x08:.flush
.flush:
    ret

gdt_reload_segments:
    mov ax, 0x10        ; KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret
