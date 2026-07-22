.section .text
.global trapEntry
.align 4
trapEntry:
    csrr t0, sepc
    addi t0, t0, 4
    csrw sepc, t0
    sret