.set IRQ_BASE, 0x20

.section .text

.extern _ZN6jackos8hardware16InterruptManager15handleInterruptEhj

.global _ZN6jackos8hardware16InterruptManager22IgnoreInterruptRequestEv

.macro HandleException num
.global _ZN6jackos8hardware16InterruptManager19HandleException\num\()Ev
_ZN6jackos8hardware16InterruptManager19HandleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN6jackos8hardware16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN6jackos8hardware16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0
    jmp int_bottom
.endm

HandleException 0x00
HandleException 0x01
HandleException 0x02
HandleException 0x03
HandleException 0x04
HandleException 0x05
HandleException 0x06
HandleException 0x07
HandleException 0x08
HandleException 0x09
HandleException 0x0A
HandleException 0x0B
HandleException 0x0C
HandleException 0x0D
HandleException 0x0E
HandleException 0x0F
HandleException 0x10
HandleException 0x11
HandleException 0x12
HandleException 0x13


HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x02
HandleInterruptRequest 0x03
HandleInterruptRequest 0x04
HandleInterruptRequest 0x05
HandleInterruptRequest 0x06
HandleInterruptRequest 0x07
HandleInterruptRequest 0x08
HandleInterruptRequest 0x09
HandleInterruptRequest 0x0A
HandleInterruptRequest 0x0B
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x0D
HandleInterruptRequest 0x0E
HandleInterruptRequest 0x0F

HandleInterruptRequest 0x80

int_bottom:
    ;# Save registers
    ;#pusha
    ;#pushl %ds
    ;#pushl %es
    ;#pushl %fs
    ;#pushl %gs

    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    ;# Load ring 0 segment register
    ;#cld
    ;#mov $0x10, %eax
    ;#mov %eax, %eds
    ;#mov %eax, %ees

    ;# Call C++ handler
    pushl %esp
    push (interruptnumber)
    call _ZN6jackos8hardware16InterruptManager15handleInterruptEhj
    ;# addl $5, %esp
    movl %eax, %esp

    ;# Restore registers
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    ;#popl %gs
    ;#popl %fs
    ;#popl %es
    ;#popl %ds
    ;#popa

    add $4, %esp

_ZN6jackos8hardware16InterruptManager22IgnoreInterruptRequestEv:

    iret

_stop:
    cli
    hlt
    jmp _stop

.data
    interruptnumber: .byte 0
