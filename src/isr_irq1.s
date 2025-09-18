; src/isr_irq1.asm (NASM, 32-bit)
BITS 32
GLOBAL isr_irq1_stub
GLOBAL default_exception_stub
EXTERN keyboard_handler
EXTERN pic_send_eoi_master
EXTERN default_exception_handler

isr_irq1_stub:
    pusha
    cld
    
    ; Önce PIC'e EOI gönder
    call pic_send_eoi_master
    
    ; Sonra handler'ı çağır
    call keyboard_handler
    
    popa
    iretd

default_exception_stub:
    pusha
    cld
    call default_exception_handler
    popa
    iretd
