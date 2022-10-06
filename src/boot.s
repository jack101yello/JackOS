; This is where the kernel starts, and it defines the multiboot header

MBOOT_PAGE_ALIGN equ 1<<0 ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO equ 1<<1 ; Provide the kernel with memory info
MBOOT_HEADER_MAGIC equ 0x1BADB002 ; Multiboot Magic value
MBOOT_HEADER_FLAGS equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO ; Load settings to be passed over to GRUB
MBOOT_CHECKSUM equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS) ; Error check to ensure that the settings and the magic number are corrrect

[BITS 32]

[GLOBAL mboot] ; Makes mboot accessible from C
[EXTERN code] ; Start of the .text section
[EXTERN bss] ; Start of the .bss section
[EXTERN end] ; End of the last loadable section

mboot:
    dd MBOOT_HEADER_MAGIC ; GRUB searches for this value on each 4-byte boundary in the kernel
    dd MBOOT_HEADER_FLAGS ; How GRUB will load the file
    dd MBOOT_CHECKSUM ; Ensures that the above values are correct
    dd mboot ; Location of this descriptor
    dd code ; Start of Kernel .text (code) section
    dd bss ; End of kernel .data section
    dd end ; End of kernel
    dd start ; Kernel entry point

[GLOBAL start] ; Kernel entry
[EXTERN main] ; C entry

start:
    push ebx ; Load Multiboot header location

    ; Execute the kernel
    cli ; Disable interrupts
    call main ; Call the main() function
    jmp $ ; Enter an infinite loop to halt the processor
