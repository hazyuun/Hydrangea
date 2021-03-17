#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <term/term.h>

#define assert(e)                                                              \
  if (!(e)) {                                                                  \
    tty_print("Assertion failed !");                                           \
    while (1)                                                                  \
      ;                                                                        \
  }

#endif