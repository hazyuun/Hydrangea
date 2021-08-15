#ifndef _SPINLOCK_H_
#define _SPINLOCK_H

typedef uint8_t volatile spinlock_t;

void spinlock_acquire(uint8_t volatile *spinlock);
void spinlock_release(uint8_t volatile *spinlock);

#endif
