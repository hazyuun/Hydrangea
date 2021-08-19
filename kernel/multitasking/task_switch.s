.section .text
.align 4

.extern cur_task
.extern staged_task

.global mt_switch_task

/* void mt_switch_task(task_t *task); */
mt_switch_task:

  push %ebp
  mov %esp, %ebp
  push %eax
  push %ebx
  push %ecx
  push %edx
  push %esi
  push %edi
  pushf

  movl cur_task, %esi
  movl %esp, (%esi)

  movl 8(%ebp), %esi
  movl %esi, cur_task
  
  movl (%esi), %esp

  popf
  pop %edi  
  pop %esi
  pop %edx
  pop %ecx
  pop %ebx
  pop %eax
  pop %ebp
  ret


.global go_usermode
go_usermode:
  cli
  mov $0x23, %ax
  mov %ax, %ds
  mov %ax, %es
  mov %ax, %fs
  mov %ax, %gs

  mov %esp, %eax

  pushl $0x23
  pushl %eax
  pushl $0x200
  pushl $0x1B
  pushl $0x1000000
  iret
