.section .text
.align 4

/* A macro to create an ISR that doesn't push error code */ 
.macro isr num
    .global isr\num
    isr\num:
        push $0
        push $\num
        jmp isr_common_handler_wrapper		
.endm

/* Same thing for those that push error code */
.macro isr_werr num
    .global isr\num
    isr\num:
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
/* Now it's the IRQs' turn */

.macro irq num, mapped_to
    .global irq\num
    irq\num:
        push $0
        push $\mapped_to
        jmp irq_common_handler_wrapper
.endm

.macro irq_gen_handlers
    .altmacro
    .set i, 0
    .rept 16
        irq %i, %i+32
        .set i, i+1
    .endr
.endm
irq_gen_handlers

.extern isr_common_handler
.extern irq_common_handler

isr_common_handler_wrapper:
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    push %esp
    
    call isr_common_handler
    
    add $4, %esp
    pop %gs
	pop %fs
	pop %es
	pop %ds
	popa
    add $8, %esp
    iret

irq_common_handler_wrapper:
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    push %esp
    
    call irq_common_handler
    
    add $4, %esp

    pop %gs
	pop %fs
	pop %es
	pop %ds

    popa
    
    add $8, %esp
    iret






