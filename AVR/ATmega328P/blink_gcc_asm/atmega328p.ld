OUTPUT_FORMAT("elf32-avr")
OUTPUT_ARCH(avr:5)
ENTRY(_start)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x0000, LENGTH = 32K
  SRAM (rwx) : ORIGIN = 0x800100, LENGTH = 2K
  EEPROM (rwx) : ORIGIN = 0x810000, LENGTH = 1K
}

SECTIONS
{
  /* 1. TEXT SECTION */
  .text :
  {
    *(.vectors)
    KEEP(*(.vectors))
    *(.progmem*)
    *(.rodata*)
    *(.text*)
    
    . = ALIGN(2);
    _etext = . ;
  } > FLASH

  /* 2. DATA SECTION */
  /* LOADADDR gets the physical address in Flash where data is stored */
  .data : AT (ADDR(.text) + SIZEOF(.text))
  {
    PROVIDE (__data_start = .);
    *(.data*)
    . = ALIGN(2);
    PROVIDE (__data_end = .);
  } > SRAM

  /* Define the symbol for the startup code to find the data in Flash */
  __data_load_start = LOADADDR(.data);

  /* 3. BSS SECTION */
  .bss (NOLOAD) :
  {
    PROVIDE (__bss_start = .);
    *(.bss*)
    *(COMMON)
    . = ALIGN(2);
    PROVIDE (__bss_end = .);
  } > SRAM

  /* 4. EEPROM SECTION */
  .eeprom (NOLOAD) :
  {
    *(.eeprom*)
  } > EEPROM

  /* 5. STACK DEFINITION (Optional but good practice) */
  /* The standard crt usually sets SP to RAMEND, but defining it helps debugging */
  .stack (NOLOAD):
  {
     . = ALIGN(2);
     _estack = .; 
  } > SRAM
}
