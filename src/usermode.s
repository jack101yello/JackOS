.section .text
.global enter_usermode

enter_usermode:
    mov $0x2B, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    mov 4(%esp), %eax ;# entry function pointer
    mov %esp, %ecx ;# current stack

    lea user_stack_top, %ebx
    
    ;# mov %esp, %eax
    push $0x2B
    push %ebx
    pushf
    push $0x23
    push %eax ;# the entry point
    iret

user_mode_start:
    jmp .

.section .bss
.align 16
user_stack_space:
    .space 0x4000
user_stack_top: