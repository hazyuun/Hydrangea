#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/* syscall_n takes n parameters plus the syscall number (%eax) */
int syscall_0(int);
int syscall_1(int, int);
int syscall_2(int, int, int);
int syscall_3(int, int, int, int);
int syscall_4(int, int, int, int, int);
int syscall_5(int, int, int, int, int, int);

#define SYSCALL_0(eax) syscall_0((int)(eax))
#define SYSCALL_1(eax, ebx) syscall_1((int)(eax), (int)(ebx))
#define SYSCALL_2(eax, ebx, ecx) syscall_2((int)(eax), (int)(ebx), (int)(ecx))
#define SYSCALL_3(eax, ebx, ecx, edx)                                          \
  syscall_3((int)(eax), (int)(ebx), (int)(ecx), (int)(edx))
#define SYSCALL_4(eax, ebx, ecx, edx, esi)                                     \
  syscall_4((int)(eax), (int)(ebx), (int)(ecx), (int)(edx), (int)(esi))
#define SYSCALL_5(eax, ebx, ecx, edx, esi, edi)                                \
  syscall_5((int)(eax), (int)(ebx), (int)(ecx), (int)(edx), (int)(esi),        \
            (int)(edi))

#endif
