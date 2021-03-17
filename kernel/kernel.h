#ifndef _KERNEL_H____
#define _KERNEL_H____

#define KERNEL_VERSION "v0.0.1"

#define hang()                                                                 \
  do {                                                                         \
    __asm__("hlt\n\t");                                                        \
  } while (1);

#include <term/term.h>
#define panic(err_msg)                                                         \
  do {                                                                         \
    term_use_color(NICE_RED);                                                  \
    term_print("\n KERNEL PANIC : ");                                          \
    term_print(err_msg);                                                       \
    while (1)                                                                  \
      ;                                                                        \
  } while (0);

#endif
