.set IRQ_BASE, 0x20

.section .text

.extern _ZN6jackos8hardware16InterruptManager15handleInterruptEhj

.global _ZN6jackos8hardware16InterruptManager22IgnoreInterruptRequestEv

.macro HandleException num
.global _ZN6jackos8hardware16InterruptManager16HandleException\num\()Ev
_ZN6jackos8hardware16InterruptManager16HandleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN6jackos8hardware16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN6jackos8hardware16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C

int_bottom:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    pushl %esp
    push (interruptnumber)
    call _ZN6jackos8hardware16InterruptManager15handleInterruptEhj
    ;# addl $5, %esp
    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

_ZN6jackos8hardware16InterruptManager22IgnoreInterruptRequestEv:

    iret

_stop:
    cli
    hlt
    jmp _stop

.data
    interruptnumber: .byte 0