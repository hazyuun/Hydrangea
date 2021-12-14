.extern entry

.global _start
_start:
  /* Setup the stack */
  /* TODO : Let the kernel do that instead ? */
  mov $0xF000FFF, %esp

  xor %eax, %eax

  /* TODO : argc and argv ? */
  /* TODO : and maybe even envp ? */
  push $111
  push $222
  call entry

  add $8, %esp

  /* exit(); */
  mov $0x1, %eax
  int $0x80
    
1:
  pause
  jmp 1b
