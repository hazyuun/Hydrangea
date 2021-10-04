#ifndef _FILE_OPS_
#define _FILE_OPS_

#include <stddef.h>
#include <stdint.h>

int open(char *path, uint32_t flags, uint32_t mode);
int read(int fd, void *buffer, size_t size);
int write(int fd, const void *buffer, size_t size);
int close(int fd);

#endif

