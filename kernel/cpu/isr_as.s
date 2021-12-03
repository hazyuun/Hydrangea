.section .text
.align 4

/* A macro to create an ISR that doesn't push error code */ 
.macro isr num
    .global isr\num
    isr\num:
        cli
        push $0
        push $\num
        jmp isr_common_handler_wrapper		
.endm

/* Same thing for those that push error code */
.macro isr_werr num
    .global isr\num
    isr\num:
        cli
        push $\num
        jmp isr_common_handler_wrapper

.endm

/*
 * This generates ISRs
 *  eg : 
 * isr_gen_handlers 10, 5, 1
 * will generate from isr10 to isr15
 * and those will push error code
 */
.macro isr_gen_handlers start, num, werr
    .altmacro
    .set k, \start
    .rept \num
        .if \werr
            isr_werr %k
        .else
            isr %k
        .endif
        .set k, k+1
    .endr
.endm

/* Generate the first 31 ISR handlers */
isr_gen_handlers 0,8,0
isr_werr 8
isr 9
isr_gen_handlers 10,5,1
isr 15
isr 16
isr_werr 17
isr_gen_handlers 18,12,0
isr_werr 30
isr 31

.global isr32
isr32:
    push $0
    push $32
    jmp isr_common_handler_wrapper


.extern isr_common_handler

isr_common_handler_wrapper:
    push %edi
    push %esi
    push %ebp
    push %ebx
    push %edx
    push %ecx
    push %eax
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    cld
    push %esp    
    call isr_common_handler

    add $4, %esp
    popl %gs
    popl %fs
    popl %es
    popl %ds
    pop %eax
    pop %ecx
    pop %edx
    pop %ebx
    pop %ebp
    pop %esi
    pop %edi
    add $8, %esp

    iretl
