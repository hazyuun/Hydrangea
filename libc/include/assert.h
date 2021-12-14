
#undef a
#undef static_assert

#include <stdio.h>

#define static_assert _Static_assert

#ifdef NDEBUG
#define a(ignore) ((void)0)
#else
#define a(expression)                                                     \
  do {                                                                         \
    if (!(expression)) {                                                       \
      printf("[!] Assertion failed : %s (" __FILE__ ":%d in function %s) ", #expression, __LINE__, __FUNCTION__);                        \
      fflush(stdout);\
      /* TODO : Call abort */                                                  \
      asm volatile("1: pause; jmp 1b");                                        \
    }                                                                          \
  } while (0)

#endif
