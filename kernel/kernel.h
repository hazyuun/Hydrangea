#ifndef _KERNEL_H_
#define _KERNEL_H_

#define KERNEL_VERSION "v0.2"

#include <multitasking/scheduler.h>
#include <term/term.h>
#include <stdio.h>

void hang(void);
void panic(char *err_msg);

#endif
