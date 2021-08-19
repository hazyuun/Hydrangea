#include <syscalls/syscall.h>
#include <drivers/serial.h>

void sys_call(registers_t *r){
  serial_write(SERIAL_COM1, 'u');
}
