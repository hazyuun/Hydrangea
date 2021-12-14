.global syscall_0
.global syscall_1
.global syscall_2
.global syscall_3
.global syscall_4
.global syscall_5

syscall_5:
  mov 24(%esp), %edi
syscall_4:
  mov 20(%esp), %esi
syscall_3:
  mov 16(%esp), %edx
syscall_2:
  mov 12(%esp), %ecx
syscall_1:
  mov 8(%esp), %ebx
syscall_0:
  mov 4(%esp), %eax
  int $0x80
  ret
