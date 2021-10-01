#include <syscalls/syscall.h>
#include <drivers/serial.h>
#include <util/logger.h>
#include <multitasking/scheduler.h>

syscall_t syscall_list[] = {
  &sys_hello,
  &sys_exit
};

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
  (void) params;
  //serial_write(SERIAL_COM1, params->ebx);
}

void sys_exit(syscall_params_t *params){
  (void) params;

  task_t *t = mt_get_current_task(); 

  log_info(NICE_YELLOW_0, "SYSCALL", "PID %d called exit()", t->pid);
  mt_task_terminate(t);

  mt_schedule();

  asm volatile("1: hlt; jmp 1b");
}
