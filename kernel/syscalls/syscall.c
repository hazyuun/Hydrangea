#include <syscalls/syscall.h>
#include <drivers/serial.h>
#include <util/logger.h>

syscall_t syscall_list[] = {
  &sys_hello,
  &sys_exit
};

extern void do_syscall(void*);


void syscall_handler(registers_t *r){
  
  syscall_params_t params = { 
    .eax = r->eax,
    .ebx = r->ebx,
    .ecx = r->ecx,
    .edx = r->edx,
    .esi = r->esi,
    .edi = r->edi,
    .ebp = r->ebp 
  };

  (syscall_list[r->eax])(&params);
}

void sys_hello(syscall_params_t *params){
  serial_write(SERIAL_COM1, params->ebx);
}
#include <multitasking/scheduler.h>
void sys_exit(syscall_params_t *params){
  /* TODO: Implement this as well as other syscalls */
  log_info(NICE_YELLOW_0, "SYSCALL", "exit()");
  asm volatile("1: hlt; jmp 1b");
}