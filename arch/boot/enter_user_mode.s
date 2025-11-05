; src/assambly/enter_user_mode.s
BITS 32
GLOBAL enter_user_mode

; void enter_user_mode(void (*entry)(), uint32_t user_stack_top)
; [esp+4] = entry, [esp+8] = stack top
enter_user_mode:
    mov eax, [esp+8]         ; user stack
    mov ebx, [esp+4]         ; user entry (EIP)

    mov ax, 0x23             ; USER_DS (DPL=3)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; IRET frame hazırlığı (ring 3'e geçiş)
    push dword 0x23          ; SS
    push eax                 ; ESP
    push dword 0x202         ; EFLAGS (IF=1)
    push dword 0x1B          ; CS
    push ebx                 ; EIP
    iretd

user_entry:
    .loop:
        nop
        jmp .loop
