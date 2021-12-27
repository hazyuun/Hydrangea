#ifndef _ERRNO_H_
#define _ERRNO_H_

#define EDOM
#define EILSEQ
#define ERANGE

volatile int *errno_ptr(void);

#define errno (*errno_ptr())

#endif
