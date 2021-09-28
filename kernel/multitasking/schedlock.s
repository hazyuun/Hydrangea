.section .text
.align 4

.global get_eflags_and_cli
get_eflags_and_cli:
  pushfl
  popl %eax
  cli
  ret

.global set_eflags
set_eflags:
  movl 4(%esp), %eax
  pushl %eax
  popfl
  ret
