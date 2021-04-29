#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <stdint.h>

typedef struct {
  uint32_t gs, fs, es, ds;
  uint32_t edi, esi, ebp, useless, ebx, edx, ecx, eax;
  uint32_t int_num, err_code;
  uint32_t eip, cs, eflags, esp, ss;
} registers_t;

#endif
