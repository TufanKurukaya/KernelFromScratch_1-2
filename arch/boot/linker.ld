/* linker.ld */
ENTRY(start)

SECTIONS
{
  /* Çekirdeği 1MB adresine yerleştir */
  . = 0x80000;

  .multiboot ALIGN(4K) : { build/arch/boot/boot.o(.multiboot) }
  .text ALIGN(4K) : { *(.text*) }
  .rodata ALIGN(4K) : { *(.rodata*) }
  .data ALIGN(4K) : { *(.data*) }
  .bss ALIGN(4K) : { *(COMMON) *(.bss*) }
  /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.multiboot) 
        *(.note*)
    }
}
