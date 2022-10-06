/* Linker script for the kernel */

ENTRY(start) /* Determines the start location of the binary */
SECTIONS
{
    .text 0x100000 : /* Location of all of the code */
    {
        code = .; _code = .; __code = .;
        *(.text)
        . = ALIGN(4096);
    }

    .data : /* Initialized static data */
    {
        data = .; _data = .; __data = .;
        *(.data)
        *(.rodata) /* Read-only initialized data */
        . = ALIGN(4096); /* Aligns to a page */
    }

    .bss : /* Uninitialized static data */
    {
        bss = .; _bss = .; __bss = .;
        *(.bss)
        . = ALIGN(4096);
    }

    end = .; _end = .; __end = .;
}