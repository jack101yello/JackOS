.section .text
.global enter_usermode
.extern print_stack_debug

enter_usermode:
    cli
    mov 4(%esp), %eax ;# entry function pointer
    mov $0x00300000, %ebx # 5MB is well above where the kernel code lies
    # mov %esp, %ecx ;# current stack

    # lea user_stack_top, %ebx
    
    ;# mov %esp, %eax
    push $0x2B
    push %ebx
    push $0x202
    push $0x23
    push %eax ;# the entry point
    iret
