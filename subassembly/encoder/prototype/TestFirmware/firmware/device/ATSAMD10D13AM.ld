/*--------------------------------------------------------------------------
 * MPLAB XC32 Compiler -  ATSAMD10D13AM linker script
 * 
 * Copyright (c) 2021, Microchip Technology Inc. and its subsidiaries ("Microchip")
 * All rights reserved.
 * 
 * This software is developed by Microchip Technology Inc. and its
 * subsidiaries ("Microchip").
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * 1.      Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 * 2.      Redistributions in binary form must reproduce the above 
 *         copyright notice, this list of conditions and the following 
 *         disclaimer in the documentation and/or other materials provided 
 *         with the distribution.
 * 3.      Microchip's name may not be used to endorse or promote products
 *         derived from this software without specific prior written 
 *         permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

/*  Set the entry point in the ELF file. Once the entry point is in the ELF
 *  file, you can then use the --write-sla option to xc32-bin2hex to place
 *  the address into the hex file using the SLA field (RECTYPE 5). This hex
 *  record may be useful for a bootloader that needs to determine the entry
 *  point to the application.
 */
ENTRY(main)

/*************************************************************************
 * Memory-Region Macro Definitions
 * The XC32 linker preprocesses linker scripts. You may define these
 * macros in the MPLAB X project properties or on the command line when
 * calling the linker via the xc32-gcc shell.
 *************************************************************************/

 

/*************************************************************************
 * Memory-Region Definitions
 * The MEMORY command describes the location and size of blocks of memory
 * on the target device. The command below uses the macros defined above.
 *************************************************************************/

/* SAMD10D13 - 8kb Flash, 4Kb RAM */
FLASH_SIZE = 0x2000;
SRAM_SIZE = 0x1000;

/* The stack size used by the application. NOTE: you need to adjust according to your application. */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : DEFINED(__stack_size__) ? __stack_size__ : 0x0400;

MEMORY
{
  rom (rx) : ORIGIN = 0x0, LENGTH = FLASH_SIZE
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = SRAM_SIZE
}

/*************************************************************************
 * Output region definitions.
 * CODE_REGION defines the output region for .text/.rodata.
 * DATA_REGION defines the output region for .data/.bss
 * VECTOR_REGION defines the output region for .vectors.
 * 
 * CODE_REGION defaults to 'rom', if rom is present (non-zero length),
 * and 'ram' otherwise.
 * DATA_REGION defaults to 'ram', which must be present.
 * VECTOR_REGION defaults to CODE_REGION, unless 'boot_rom' is present.
 */


__rom_end = ORIGIN(rom) + LENGTH(rom);
__ram_end = ORIGIN(ram) + LENGTH(ram);

/*************************************************************************
 * Section Definitions - Map input sections to output sections
 *************************************************************************/
SECTIONS
{
  /*
    * Code Sections - Note that standard input sections such as
    * *(.text), *(.text.*), *(.rodata), & *(.rodata.*)
    * are not mapped here. The best-fit allocator locates them,
    * so that input sections may flow around absolute sections
    * as needed.
    */
  .text :
  {
    . = ALIGN(4);
    
    _sfixed = .;

    KEEP(*(.vectors .vectors.* .vectors_default .vectors_default.*))
    KEEP(*(.isr_vector))
    KEEP(*(.reset*))
    KEEP(*(.after_vectors))

    *(.text .text.* .gnu.linkonce.t.*)
    *(.rodata .rodata* .gnu.linkonce.r.*)

    . = ALIGN(4);
    KEEP(*(.init))
    . = ALIGN(4);
    __preinit_array_start = .;
    KEEP (*(.preinit_array))
    __preinit_array_end = .;

    . = ALIGN(4);
    __init_array_start = .;
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    __init_array_end = .;

    . = ALIGN(4);
    KEEP(*(.fini))

    . = ALIGN(4);
    __fini_array_start = .;
    KEEP (*(.fini_array))
    KEEP (*(SORT(.fini_array.*)))
    __fini_array_end = .;

    . = ALIGN(4);
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend.o(.ctors))

    . = ALIGN(4);
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend.o(.dtors))

    . = ALIGN(4);
    _efixed = .;
  } > rom

  .ARM.extab : {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > rom

  .ARM.exidx : {
    PROVIDE(__exidx_start = .);
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    PROVIDE(__exidx_end = .);
  } > rom

  .relocate :
  {
    . = ALIGN(4);
    _srelocate = .;
    *(.ramfunc .ramfunc.*);
    *(.data .data.*);
    . = ALIGN(4);
    _erelocate = .;
  } > ram AT> rom

  _etext = LOADADDR(.relocate);

  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    _szero = .;
    *(.bss .bss.*)
    *(COMMON)
    . = ALIGN(4);
    _ezero = .;
  } > ram

  .stack (NOLOAD):
  {
    . = ALIGN(8);
    _sstack = .;
    . = . + STACK_SIZE;
    . = ALIGN(8);
    _estack = .; /* Initial stack pointer, used to configure the vector table */
  } > ram
  
  . = ALIGN(4);
  PROVIDE(_heap_start = .);
  _end = . ;

}

