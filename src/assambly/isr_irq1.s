; src/assambly/isr_irq1.s
BITS 32
GLOBAL isr_irq1_stub
EXTERN keyboard_handler
EXTERN pic_send_eoi_master

isr_irq1_stub:
    pusha
    cld
    call pic_send_eoi_master
    call keyboard_handler
    popa
    iretd
