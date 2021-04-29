.section .text
.align 4

.extern cur_task

.global mt_switch_task

/* void mt_switch_task(task_t *task); */
mt_switch_task:
  cli
  push %eax
  push %ebx
  push %esi
  push %edi
  push %ebp

  mov cur_task, %esi
  mov %esp, (%esi)

  mov 24(%esp), %edi
  mov %edi, cur_task;
  mov (%edi), %esp

  /* Check if we should change cr3 */
  push %eax
  push %ebx

  mov %cr3, %eax
  mov 16(%esi), %ebx
  
  cmp %eax, %ebx
  je same_cr3
  mov %ebx, %cr3

same_cr3:
  pop %ebx
  pop %eax

  pop %ebp
  pop %edi
  pop %esi
  pop %ebx
  pop %eax
  sti
  ret
  