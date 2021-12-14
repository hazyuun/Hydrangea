#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"

/*
  TODO : Optimize these functions ?
*/

void *memcpy(void *restrict s1, const void *restrict s2, size_t n){
  const char *s = s2;
  char *d = s1;
  char *result = s1;
  while (n--)
    *d++ = *s++;
  return result;
}

void *memmove(void *s1, const void *s2, size_t n){
  char tmp[n];
  memcpy(tmp, s2, n);
  memcpy(s1, tmp, n);
  return s1;
}

char *strcpy(char *restrict s1, const char *restrict s2){
  char *save = s1;
  while ((*s1++ = *s2++))
    ;
  return save;
}

char *strncpy(char *restrict s1, const char *restrict s2, size_t n){
  char *d = s1;
  const char *s = s2;
  do {
    if (!(*d++ = *s++)) {
      while (--n)
        *d++ = 0;
      break;
    }
  } while (--n);
  return s1;
}

char *strcat(char *restrict s1, const char *restrict s2){
  strcpy(s1 + strlen(s1), s2);
  return s1;
}

char *strncat(char *restrict s1, const char *restrict s2, size_t n){
  char *d = s1;
  const char *s = s2;

  while (*d != 0)
    d++;
  
  do {
    if ((*d = *s++) == 0) break;
    d++;
  } while (--n);
  
  *d = 0;
	
  
  return s1;
}

int memcmp(const void *s1, const void *s2, size_t n){
  const char *p1 = (const char *)s1;
  const char *p2 = (const char *)s2;
  for (size_t i = 0; i < n; i++) {
    if (p1[i] < p2[i])
      return -1;
    else if (p1[i] > p2[i])
      return 1;
  }
  return 0;
}

int strcmp(const char *s1, const char *s2){
  int i = 0;
  while (s1[i] == s2[i]) {
    if (!s2[i++])
      return 0;
  }
  return 1;
}

int strcoll(const char *s1, const char *s2){
  /* TODO : implement this properly */
  return strcmp(s1, s2);
}

int strncmp(const char *s1, const char *s2, size_t n){
  int i = 0;

  while (n-- && s1[i] && s2[i]) {
    if(s1[i] != s2[i])
      return 1;
    ++i;
  }

  return 0;
}

size_t strxfrm(char *restrict s1, const char *restrict s2, size_t n){
  /* locale is supposed to be "C" */
  /* TODO : Implement this properly */
  size_t s2_len = strlen(s2) + 1;
  size_t to_copy = s2_len > n ? n : s2_len;

  memcpy(s1, s2, to_copy);
  
  return s2_len - 1;
}

void *memchr(const void *s, int c, size_t n){
  const char *p = s;
  while (*p != c){
    ++p;
    if (!--n)
      return 0;
  }
  return (char *)p;
}

char *strchr(const char *s, int c){
  while (*s != c)
    if (!*s++)
      return 0;
  return (char *)s;
}

size_t strcspn(const char *s1, const char *s2){
  size_t ret = 0;
  while (*s1)
    if (strchr(s2, *s1))
      return ret;
    else
      s1++, ret++;
  return ret;
}

char *strpbrk(const char *s1, const char *s2){
 
  while(*s1){

    for(const char *p = s2; *p; p++){
      if(*p == *s1) return s1; 
    }
    ++s1;
  }
  return NULL;
}

char *strrchr(const char *s, int c){
  const char *ret = 0;
  size_t i = 0;
  while (s[i]){
    if (s[i] == c)
      ret = s + i;
    ++i;
  }
  return ret;
}

size_t strspn(const char *s1, const char *s2){
  size_t ret = 0;
  while (*s1 && strchr(s2, *s1++))
    ret++;
  return ret;
}

char *strstr(const char *s1, const char *s2){
  size_t N1 = strlen(s1);
  size_t N2 = strlen(s2);
  if(N1 < N2) return NULL;
  
  for(size_t i = 0; i < N1 - N2 + 1; i++){
    if(!memcmp(s1 + i, s2, N2))
      return s1 + i;
  }
  
  return NULL;
}

char *strtok(char *restrict s1, const char *restrict s2){
  static char *i = 0;
  if (s1)
    i = s1;
  else if (!i)
    return 0;

  s1 = i + strspn(i, s2);
  i = s1 + strcspn(s1, s2);
  if (i == s1)
    return i = 0;
  i = *i ? *i = 0, i + 1 : 0;
  return s1;
}

void *memset(void *s, int c, size_t n){
  char *tmp = (char*) s;
  for (size_t i = 0; i < n; i++) {
    *(tmp + i) = c;
  }
  return s;
}

char *strerror(int errnum){
  /* TODO: Implement this */
  (void) errnum;
  return 0;
}

size_t strlen(const char *s){
  int len = 0;
  while (*s++)
    len++;
  return len;
}

#pragma GCC diagnostic pop
