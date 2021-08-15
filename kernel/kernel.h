#ifndef _KERNEL_H____
#define _KERNEL_H____

#define KERNEL_VERSION "v0.0.1"

#include <multitasking/scheduler.h>
#include <term/term.h>
#include <stdio.h>


void hang(void);

void panic(char *err_msg);

#endif
