.section .init
    /* gcc will put the contents of crtend.o's .init section here */
    popl %ebp
    ret

.section .fini
    popl %ebp
    ret