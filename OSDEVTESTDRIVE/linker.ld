ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);  /* Top of RAM, initial MSP */

SECTIONS
{
  /* Vector table */
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  /* Code + read-only data */
  .text :
  {
    *(.text*)
    *(.rodata*)
  } > FLASH

  /* Initialized data */
  .data : AT(ADDR(.text) + SIZEOF(.text))
  {
    /* Load address (in FLASH) for .data - used by startup to copy init values */
    _ldata = LOADADDR(.data);
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  /* Uninitialized data */
  .bss :
  {
    _sbss = .;
    *(.bss*)
    _ebss = .;
  } > RAM

  /* Optional: stack for PSP / RTOS tasks */
  .stack (NOLOAD) :
  {
    . = ALIGN(8);
    _stack_start = .;
    . = . + 0x1000; /* 4KB stack for tasks */
    _stack_end = .;
  } > RAM
}
