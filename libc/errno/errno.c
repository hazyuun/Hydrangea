#include <errno.h>

volatile int _errno;

volatile int *errno_ptr(void) { return &_errno; }
