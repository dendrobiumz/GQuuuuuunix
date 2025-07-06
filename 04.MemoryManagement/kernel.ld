OUTPUT_ARCH( "riscv" )
ENTRY( _entry )

MEMORY
{
  code    (rx)  : ORIGIN = 0x80000000, LENGTH = 4M
  rodata  (r)   : ORIGIN = 0x80200000, LENGTH = 1M
  data    (rw)  : ORIGIN = 0x80300000, LENGTH = 1M
}

SECTIONS
{
  /*
   * ensure that entry.S / _entry is at 0x80000000,
   * where qemu's -kernel jumps.
   */
  . = 0x80000000;

  .text : {
    KEEP(*(.text.boot))
    *(.text .text.*)
  } >code

  .rodata : {
    . = ALIGN(16);
    *(.rodata .rodata.*) *(.srodata .srodata.*)
  } >rodata

  .data : {
    . = ALIGN(4096);
    *(.sdata .sdata.*) *(.data .data.*)
  } >data

  .bss : {
    . = ALIGN(16);
    *(.sbss .sbss.*) *(.bss .bss.*)
  } >data
  
  PROVIDE(end = .);
}