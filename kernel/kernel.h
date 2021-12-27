#ifndef _KERNEL_H_
#define _KERNEL_H_

#define KERNEL_NAME "Hydrangea kernel"
#define KERNEL_VERSION                                                         \
  "v0.8 ("__DATE__                                                             \
  " "__TIME__                                                                  \
  ")"
#define KERNEL_ALIAS "YuunOS"

#include <multitasking/scheduler.h>
#include <stdio.h>
#include <term/term.h>

void hang(void);
void panic(char *err_msg);

#endif
