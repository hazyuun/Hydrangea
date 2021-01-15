#include <stdint.h>
#include <string.h>

int memcmp(const void *ptr1, const void *ptr2, size_t size) {
  const uint8_t *p1 = (const uint8_t *)ptr1;
  const uint8_t *p2 = (const uint8_t *)ptr2;
  for (size_t i = 0; i < size; i++) {
    if (p1[i] < p2[i])
      return -1;
    else if (p1[i] > p2[i])
      return 1;
  }
  return 0;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t size) {
  const char *s = src;
  char *d = dest;
  char *result = dest;
  while (size--)
    *d++ = *s++;
  return result;
}

void *memmove(void *dest, const void *src, size_t size) {
  const char *s = src;
  char *d = dest;
  if (dest < src) {
    for (size_t i = 0; i < size; i++)
      d[i] = s[i];
  } else {
    for (size_t i = size; i != 0; i--)
      d[i - 1] = s[i - 1];
  }
  return dest;
}

void memset(void *dest, char value, int n) {
  char *temp = (char *)dest;
  for (; n > 0; n--) {
    *temp = value;
    temp += sizeof(char);
  }
}

char *strcpy(char *dest, const char *src) {
  char *save = dest;
  while ((*dest++ = *src++))
    ;
  return save;
}

int strcmp(const char *dest, char *src) {
  int i = 0;
  while (dest[i] == src[i]) {
    if (!src[i++])
      return 0;
  }
  return 1;
}

int strlen(const char *str) {
  int len = 0;
  while (*str++)
    len++;
  return len;
}

char *strcat(char *dest, const char *src) {
  strcpy(dest + strlen(dest), src);
  return dest;
}
/**/
char *strchr(const char *str, char c) {
  while (*str != c)
    if (!*str++)
      return 0;
  return (char *)str;
}
size_t strspn(const char *str1, const char *str2) {
  size_t ret = 0;
  while (*str1 && strchr(str2, *str1++))
    ret++;
  return ret;
}
size_t strcspn(const char *str1, const char *str2) {
  size_t ret = 0;
  while (*str1)
    if (strchr(str2, *str1))
      return ret;
    else
      str1++, ret++;
  return ret;
}

char *strtok(char *str, const char *delim) {
  static char *i = 0;
  if (str)
    i = str;
  else if (!i)
    return 0;

  str = i + strspn(i, delim);
  i = str + strcspn(str, delim);
  if (i == str)
    return i = 0;
  i = *i ? *i = 0, i + 1 : 0;
  return str;
}

char *itoa(int value, char *result, int base) {
  unsigned long tmp = value;

  int i = 0;
  do {
    tmp = value % base;
    result[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
  } while (value /= base);
  result[i--] = 0;

  for (int j = 0; j < i; j++, i--) {
    tmp = result[j];
    result[j] = result[i];
    result[i] = tmp;
  }

  return result;
}

int atoi(char *str) {
  int result = 0;
  int negative = 0;

  while (*str == ' ')
    str++;
  if (*str == '-') {
    negative = 1;
    str++;
  } else if (*str == '+')
    str++;

  while (1) {
    unsigned short n = *str++ - '0';
    if (n > 9)
      break;
    result = (10 * result) + n;
  }

  return negative ? -result : result;
}