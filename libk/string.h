#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

int memcmp(const void *ptr1, const void *ptr2, size_t size);
void *memcpy(void *restrict ptr1, const void *restrict ptr2, size_t size);
void *memmove(void *, const void *, size_t);

void memset(void *dest, char value, int n);

char *strcpy(char *dest, const char *src);
int strcmp(const char *dest, char *src);
int strlen(const char *str);
char *strcat(char *dest, const char *src);

char *strchr(const char *str, char c);
size_t strspn(const char *str1, const char *str2);
size_t strcspn(const char *str1, const char *str2);
char *strstr(char *str1, const char *str2);

char *strtok(char *str, const char *delim);

char *itoa(int value, char *result, int base);
int atoi(char *str);
#endif
