/* elf self supporting loop */

_start: .global _start
	.asciz "PRG"
    b 0f
    reloc_num: .word 0
    0:
    push {r0, r1}
    bkpt #1
    ldr r0, reloc_num
    adr r2, reloc_list
    mov r1, #4
    mla r1, r0, r1, r2

reloc_loop:
    cmp r0, #0
    beq reloc_finish

    ldr r3, [r2] /* load pointer to pointer offset */
    add r12, r1, r3 /* get absolute address of pointer */
    ldr r3, [r12] /* load pointer */
    add r3, r1, r3 /* fix up */
    str r3, [r12] /* store back */

    add r2, r2, #4
    sub r0, r0, #1
    b reloc_loop

reloc_finish:
    pop {r0, r1}
    /* r2 now points to start of the executable */
    add r2, r2, #4 /* add 4 to skip magic number */
    mov pc, r2

reloc_list: