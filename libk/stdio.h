#ifndef _STDIO_H_
#define _STDIO_H_

#include <stdarg.h>
void v_printk(const char *format, va_list ap);
void printk(const char *format, ...);
void scank(const char *format, ...);

#endif
