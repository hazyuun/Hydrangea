#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include <cpu/registers.h>

void sys_call(registers_t *r);

#endif
