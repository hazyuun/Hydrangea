#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <cpu/registers.h>


typedef struct { 
  uint32_t eax, ebx, ecx, edx, esi, edi, ebp;
} syscall_params_t;

typedef void (*syscall_t)(syscall_params_t *);

void syscall_handler(registers_t *r);

void sys_hello(syscall_params_t *params);
void sys_exit(syscall_params_t *params);

#endif
