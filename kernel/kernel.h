#ifndef _KERNEL_H____
#define _KERNEL_H____

#define hang()                                                                 \
  do {                                                                         \
    __asm__("hlt\n\t");                                                        \
  } while (1);

void panic(char *err_msg);

#endif
