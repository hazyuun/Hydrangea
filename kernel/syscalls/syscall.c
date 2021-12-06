#include <syscalls/syscall.h>
#include <drivers/serial.h>
#include <util/logger.h>
#include <multitasking/scheduler.h>

syscall_t syscall_list[] = {
  &sys_hello,
  &sys_exit,
  NULL,
  &sys_read,
  &sys_write,
  &sys_open,
  &sys_close,
  &sys_getpid,
  &sys_getppid,
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
  
  r->eax = params.eax;
  r->ebx = params.ebx;
  r->ecx = params.ecx;
  r->edx = params.edx;
  r->esi = params.esi;
  r->edi = params.edi;
  r->ebp = params.ebp;
}

void sys_hello(syscall_params_t *params){
  (void) params;
  //serial_write(SERIAL_COM1, params->ebx);
}

void sys_exit(syscall_params_t *params){
  (void) params;

  task_t *t = mt_get_current_task(); 

  //log_info(NICE_YELLOW_0, "SYSCALL", "PID %d called exit()", t->pid);
  mt_task_terminate(t);

  mt_schedule();

  asm volatile("1: hlt; jmp 1b");
}

#include <fs/file_ops.h>
void sys_read(syscall_params_t *params){
  
  int fd = (int) params->ebx;
  char *buffer = (char *) params->ecx;
  size_t size = (size_t) params->edx;
  //log_info(NICE_YELLOW_0, "SYSCALL", "read(%d, %d, %d)", fd, buffer, size);
  
  params->eax = read(fd, buffer, size);
}
void sys_write(syscall_params_t *params){

  int fd = (int) params->ebx;
  char *buffer = (char *) params->ecx;
  size_t size = (size_t) params->edx;
  //log_info(NICE_YELLOW_0, "SYSCALL", "write(%d, %d, %d)", fd, buffer, size);
  
  write(fd, buffer, size);
}
void sys_open(syscall_params_t *params){
  char *path = (char *) params->ebx;
  uint32_t flags = (uint32_t) params->ecx;
  uint32_t mode = (uint32_t) params->edx;
  //log_info(NICE_YELLOW_0, "SYSCALL", "open(%s, %d, %d)", path, flags, mode);
  
  params->eax = open(path, flags, mode);
}
void sys_close(syscall_params_t *params){
  (void) params;
  int fd = (int) params->ebx;
  
  params->eax = close(fd);
}

void sys_getpid(syscall_params_t *params){
  (void) params;
  params->eax = mt_get_current_task()->pid;
}
void sys_getppid(syscall_params_t *params){
  (void) params;
  params->eax = mt_get_current_task()->ppid;
}