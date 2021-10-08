#ifndef _KERNEL_H_
#define _KERNEL_H_

#define KERNEL_VERSION "v0.3 ("__DATE__" "__TIME__")"

#include <multitasking/scheduler.h>
#include <term/term.h>
#include <stdio.h>

void hang(void);
void panic(char *err_msg);

#endif
