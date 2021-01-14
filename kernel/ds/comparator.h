#ifndef _COMPARATOR_H_
#define _COMPARATOR_H_

#include <stdint.h>

typedef int8_t (*comparator_t)(void *, void *);

static inline int8_t default_comparator(void *x, void *y) {
  if (x == y)
    return 0;
  return (x > y) ? 1 : -1;
}

#endif