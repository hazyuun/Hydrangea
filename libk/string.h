#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

int memcmp(const void* ptr1, const void* ptr2, size_t size);
void* memcpy(void* restrict ptr1, const void* restrict ptr2, size_t size);
void* memmove(void*, const void*, size_t);


void memset(void* dest, char value, int n);

int strcmp(const char *dest, char *src);

#endif