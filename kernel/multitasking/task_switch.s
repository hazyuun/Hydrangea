.section .text
.align 4

.extern cur_task

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

  mov cur_task, %esi
  mov %esp, (%esi)

  mov 8(%ebp), %edi
  mov %edi, cur_task
  mov (%edi), %esp

  popf
  pop %edi  
  pop %esi
  pop %edx
  pop %ecx
  pop %ebx
  pop %eax
  pop %ebp
  ret
