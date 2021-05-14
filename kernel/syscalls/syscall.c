#include <syscalls/syscall.h>

void syscall(registers_t *regs){
  printk("usermode");
}
