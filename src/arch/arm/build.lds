OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
 . = 0x00000000;
 . = ALIGN(4);
 .text :
 {
  */boot.o (.text)
  */lowlevel.o (.text)
  */board.o (.text)
  *(.text)
 }
 . = ALIGN(4);
 .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }
 . = ALIGN(4);
 .data : { *(.data) }
 . = ALIGN(4);
 .got : { *(.got) }
 . = ALIGN(4);
 __bss_start = .;
 .bss (NOLOAD) : { *(.bss) . = ALIGN(4); }
 __bss_end = .;         /*defined*/
 PROVIDE(_end = .);     /*can redefine in the program, and can be referenced*/
}

