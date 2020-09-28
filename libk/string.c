#include <string.h>
#include <stdint.h>

int memcmp(const void* ptr1, const void* ptr2, size_t size){
    const uint8_t* p1 = (const uint8_t*) ptr1;
    const uint8_t* p2 = (const uint8_t*) ptr2;
    for(size_t i = 0; i < size; i++){
        if(p1[i] < p2[i]) return -1;
        else if(p1[i] > p2[i]) return 1;
    }
    return 0;     
}

void* memcpy(void* restrict dest, const void* restrict src, size_t size){
    const char* s = src;
    char* d = dest;
    char* result = dest;
    while(size--) *d++ = *s++;
    return result;
}

void* memmove(void* dest, const void* src, size_t size){
    const char* s = src;
    char* d = dest;
    if(dest < src){
        for(size_t i = 0; i < size; i++) d[i] = s[i];
    } else {
        for(size_t i = size; i != 0; i--) d[i-1] = s[i-1];
    }
    return dest;
}


void memset(void* dest, char value, int n){
    char* temp = (char *)dest;
    for (; n>0; n--){
        *temp = value;
        temp += sizeof(char);
    }
}



int strcmp(const char *dest, char *src){
    int i = 0;
    while(dest[i] == src[i]) {
        if(!src[i++])return 0;
    }
    return 1;
}

int strlen(const char* str){
    int len = 0;
    while(*str++) len++;
    return len;
}