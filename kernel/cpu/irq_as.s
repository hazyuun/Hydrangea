.section .text
.align 4

.macro irq num, mapped_to
    .global irq\num
    irq\num:
        cli
        pushl $0
        pushl $\mapped_to
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

.extern irq_common_handler

irq_common_handler_wrapper:
  cli

  push %edi
  push %esi
  push %ebp
  push %ebx
  push %edx
  push %ecx
  push %eax

  push %ds
  push %es
  push %fs
  push %gs

  mov $0x10, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs

  cld
  push %esp    
  call irq_common_handler

  add $4, %esp

.global irq_common_handler_pop_and_iret
irq_common_handler_pop_and_iret:
  pop %gs
  pop %fs
  pop %es
  pop %ds

  pop %eax
  pop %ecx
  pop %edx
  pop %ebx
  pop %ebp
  pop %esi
  pop %edi

  add $8, %esp
  
  iret
